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

//OpenCV Mat을 OpenGL Texture로 변환 
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
	// NAV 초기화
	/*
	m_NAV350 = new NAV350Interface;
	m_NAV350->NAV350_Connect(&m_sSocket, (char *)strIPNav.c_str(), 2111);
	m_NAV350->NAV350_Read_Deviceid(&m_sSocket);
	m_NAV350->NAV350_Set_User_level(&m_sSocket, 3);
	printf("NAV350 CONNECTION\n\n");
	m_NAV350->NAV350_Set_Operating_Mode(&m_sSocket,4);
	m_NAV350->NAV350_Set_Positioningdata(&m_sSocket, 1, 0);
	*/
	// Map Builder 초기화
	g_mapBuilder = BuildMap();
	g_width = g_mapBuilder.getWidth();
	g_height = g_mapBuilder.getHeight();

	glGenTextures(1, &g_textureMap);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
}
void display() {

	//화면을 지운다. (컬러버퍼와 깊이버퍼)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//이후 연산은 ModelView Matirx에 영향을 준다.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	g_textureMap = MatToTexture(g_imgMap);
	if (g_textureMap < 0) return;

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f); //큐브나 좌표축 그릴 때 사용한 색의 영향을 안받을려면 필요
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
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); //윈도우 크기로 뷰포인트 설정 

	glMatrixMode(GL_PROJECTION); //이후 연산은 Projection Matrix에 영향을 준다.
	glLoadIdentity();

	//Field of view angle(단위 degrees), 윈도우의 aspect ratio, Near와 Far Plane설정
	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //이후 연산은 ModelView Matirx에 영향을 준다. 
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
	// 증가분 정의
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

	// 기울기 클때
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
	// 기울기 작을때
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
	// Laser센서 배열을 초기화 합니다
	int * arrLaser = new int[360* nDegreeResolution];
	for (int i = 0; i < 360 * nDegreeResolution; i++)
		arrLaser[i] = 0;

	// 영상을 읽어옵니다
	Mat img = imread("./Map.png", 'r');

	// Robot의 위치로부터 가상의 Laser센서값을 생성합니다
	for (int i = 0; i < 360 * nDegreeResolution; i++)
		arrLaser[i] = getDistanceFromRobot(img, i, nDegreeResolution);

	return arrLaser;
}
void timer(int value) {	
	// 처리시간을 계산합니다
	/*
	std::clock_t start;
	double duration;
	start = std::clock();
	// Your Function
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "printf: " << duration << '\n';
	*/

	// 레이져 데이터를 Image에서 가져옵니다. Test용 입니다.
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

	// 로봇을 그립니다
	int x_pos = int(g_robotPos.getX() / CELL_SIZE / (MAP_WIDTH / SCREEN_W) + SCREEN_W / 2);
	int y_pos = int(g_robotPos.getY() / CELL_SIZE / (MAP_HEIGHT / SCREEN_H) + SCREEN_H / 2);
	circle(g_imgMap, Point(x_pos, y_pos), 10, Scalar(255, 0, 0), -1);
	line(g_imgMap, Point(x_pos, y_pos), Point(x_pos + int(cos(g_robotPos.getTheta()) * 30), y_pos + int(sin(g_robotPos.getTheta())*30)), Scalar(0, 0, 255), 5);

	// 로봇을 움직입니다
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
