#include "opencv2/opencv.hpp"
#include "build_map.h"
#include "NAV350Interface.h"
#include <iostream>  
#include <string> 
#include <glut.h>
#include <ctime>

#define SCREEN_W 800
#define SCREEN_H 800
using namespace cv;
using namespace std;

Mat g_imgMap(SCREEN_W, SCREEN_H, CV_8UC3);
GLuint g_textureMap;

// ROBOT
RobotPose g_robotPos = RobotPose(0, 0, 0);
bool g_bActibate = false;
int g_nSpeed = 5;

// MAP
BuildMap g_mapBuilder;
int g_width, g_height;

// NAV
NAV350Interface * g_NAV350;
SOCKET g_sSocket;
string g_strIPNav = "192.168.30.100";

//OpenCV Mat�� OpenGL Texture�� ��ȯ 
GLuint MatToTexture(Mat image){
	if (image.empty())  return -1;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());
	return textureID;
}
void init()
{
	// NAV �ʱ�ȭ
	/*
	m_NAV350 = new NAV350Interface;
	m_NAV350->NAV350_Connect(&m_sSocket, (char *)strIPNav.c_str(), 2111);
	m_NAV350->NAV350_Read_Deviceid(&m_sSocket);
	m_NAV350->NAV350_Set_User_level(&m_sSocket, 3);
	printf("NAV350 CONNECTION\n\n");
	m_NAV350->NAV350_Set_Operating_Mode(&m_sSocket,4);
	m_NAV350->NAV350_Set_Positioningdata(&m_sSocket, 1, 0);
	*/
	// Map Builder �ʱ�ȭ
	g_mapBuilder = BuildMap();
	g_width = g_mapBuilder.getWidth();
	g_height = g_mapBuilder.getHeight();

	glGenTextures(1, &g_textureMap);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
}
void display() {

	//ȭ���� �����. (�÷����ۿ� ���̹���)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//���� ������ ModelView Matirx�� ������ �ش�.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	g_textureMap = MatToTexture(g_imgMap);
	if (g_textureMap < 0) return;

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f); //ť�곪 ��ǥ�� �׸� �� ����� ���� ������ �ȹ������� �ʿ�
	glBindTexture(GL_TEXTURE_2D, g_textureMap);
	
	glPushMatrix();
	glTranslatef(0.0, 0.0, -3);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1, -1, 0.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(1, -1, 0.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(1, 1, 0.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(-1,  1, 0.0);
	glEnd();
	glPopMatrix();

	glutSwapBuffers();
}
void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //������ ũ��� ������Ʈ ���� 

	glMatrixMode(GL_PROJECTION); //���� ������ Projection Matrix�� ������ �ش�.
	glLoadIdentity();

	//Field of view angle(���� degrees), �������� aspect ratio, Near�� Far Plane����
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //���� ������ ModelView Matirx�� ������ �ش�. 
}
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
		g_bActibate = true;
		if (g_nSpeed < 200) g_nSpeed += 10;
		break;
	case 'd':
		g_robotPos.m_theta += 0.1;
		break;
	case 's':
		g_nSpeed = 0;
		g_bActibate = false;
		break;
	case 'a':
		g_robotPos.m_theta -= 0.1;
		break;
	case 'q':
		exit(0);
	}
}
int getDistanceFromRobot(Mat map, int nDegree, int nDegreeResolution) {
	int cellSize = 50;
	int width = map.cols;
	int height = map.rows;
	double Theta = g_robotPos.getTheta() + (nDegree / nDegreeResolution)*PI / 180;
	double dx = (LASER_DATA_MAX / cellSize)*cos(Theta);
	double dy = (LASER_DATA_MAX / cellSize)*sin(Theta);
	int x = g_robotPos.getX() / cellSize + width / 2;
	int y = g_robotPos.getY() / cellSize + height / 2;
	int count = 0;

	int startX = x;
	int startY = y;
	// ������ ����
	int addX, addY;
	if (dx < 0) {
		addX = -1;
		dx = -dx;
	}
	else addX = 1;
	if (dy < 0) {
		addY = -1;
		dy = -dy;
	}
	else addY = 1;

	// ���� Ŭ��
	if (dx >= dy) {
		for (int i = 0; i < dx; i++) {
			x += addX;
			count += dy;
			if (count >= dx) {
				y += addY;
				count -= dx;
			}
			if (x >= width || x < 0 || y >= height || y < 0)
				return LASER_DATA_MAX;
			if (map.at<uint8_t>(y, x) == 0)
				return int(sqrt((x- startX)*(x - startX)+ (y - startY)*(y - startY)) * cellSize);
		}
	}
	// ���� ������
	else {
		for (int i = 0; i < dy; i++) {
			y += addY;
			count += dx;
			if (count >= dy) {
				x += addX;
				count -= dy;
			}
			if (x >= width || x < 0 || y >= height || y < 0)
				return LASER_DATA_MAX;
			if (map.at<uint8_t>(y, x) == 0)
				return sqrt((x - startX)*(x - startX) + (y - startY)*(y - startY)) * cellSize;
			
		}
	}

	return 0;
}
int* getLaserToImage(int nDegreeResolution) {
	// Laser���� �迭�� �ʱ�ȭ �մϴ�
	int * arrLaser = new int[360* nDegreeResolution];
	for (int i = 0; i < 360 * nDegreeResolution; i++)
		arrLaser[i] = 0;

	// ������ �о�ɴϴ�
	Mat img = imread("./Map.png", 'r');

	// Robot�� ��ġ�κ��� ������ Laser�������� �����մϴ�
	for (int i = 0; i < 360 * nDegreeResolution; i++)
		arrLaser[i] = getDistanceFromRobot(img, i, nDegreeResolution);

	return arrLaser;
}
void timer(int value) {	
	// ó���ð��� ����մϴ�
	/*
	std::clock_t start;
	double duration;
	start = std::clock();
	// Your Function
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "printf: " << duration << '\n';
	*/

	// ������ �����͸� Image���� �����ɴϴ�. Test�� �Դϴ�.
	int * arrLaser;
	int degreeResolution = 4;
	arrLaser = getLaserToImage(degreeResolution);
	for (int i = 0; i < 360* degreeResolution; i++)
		g_mapBuilder.drawLine(g_robotPos, arrLaser[i], i, degreeResolution);
	
	// map -> image map
	int** map = g_mapBuilder.getMap();
	for (int i = 0; i < SCREEN_W; i++)
		for (int j = 0; j < SCREEN_H; j++) {
			int Lx = int(i*g_width/ SCREEN_W);
			int Ly = int(j*g_height/ SCREEN_H);
			if (map[Lx][Ly] == FREE_AREA)
				g_imgMap.at<Vec3b>(j, i) = Vec3b(255, 255, 255);
			else if (map[Lx][Ly] == OCCUPIED_AREA)
				g_imgMap.at<Vec3b>(j, i) = Vec3b(0, 0, 0);
			else if (map[Lx][Ly] == UNKNOWN_AREA)
				g_imgMap.at<Vec3b>(j, i) = Vec3b(128, 128, 128);
		}

	// �κ��� �׸��ϴ�
	int x_pos = int(g_robotPos.getX() / CELL_SIZE / (MAP_WIDTH / SCREEN_W) + SCREEN_W / 2);
	int y_pos = int(g_robotPos.getY() / CELL_SIZE / (MAP_HEIGHT / SCREEN_H) + SCREEN_H / 2);
	circle(g_imgMap, Point(x_pos, y_pos), 10, Scalar(255, 0, 0), -1);
	line(g_imgMap, Point(x_pos, y_pos), Point(x_pos + int(cos(g_robotPos.getTheta()) * 30), y_pos + int(sin(g_robotPos.getTheta())*30)), Scalar(0, 0, 255), 5);

	// �κ��� �����Դϴ�
	if (g_bActibate) {
		g_robotPos.m_x += g_nSpeed*cos(g_robotPos.m_theta);
		g_robotPos.m_y += g_nSpeed*sin(g_robotPos.m_theta);
	}

	glutPostRedisplay();
	glutTimerFunc(1, timer, 0); // 30FPS
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutInitWindowPosition(300, 0);
	glutCreateWindow("Mapping");

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
