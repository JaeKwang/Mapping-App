#include "build_map.h"

/**
@brief Korean: 베이즈룰에 따른 확률값 업데이트 함수
@brief English:
*/
RobotPose::RobotPose(double dX, double dY, double dTheta) {
	m_x = dX;
	m_y = dY;
	m_theta = dTheta;
}
double RobotPose::getX() {	return m_x;}
double RobotPose::getY() {	return m_y;}
double RobotPose::getTheta() {	return m_theta;}

BuildMap::BuildMap() {
	m_GridMap = new GridMap(MAP_WIDTH, MAP_HEIGHT);
	m_curPos = new RobotPose(0, 0, 0);
}
void BuildMap::drawLine(RobotPose RobotPos, double dDist, int nIndex, int nDegreeResolution) {
	if (dDist == 0) return;
	int wall = 0;
	if (dDist < LASER_DATA_MAX)
		wall = THICKNESS_OF_WALL / CELL_SIZE;
	double Theta = RobotPos.getTheta() + (nIndex/ nDegreeResolution)*PI/180;
	double dx = (dDist / CELL_SIZE+ wall)*cos(Theta);
	double dy = (dDist / CELL_SIZE + wall)*sin(Theta);
	double dFreeDx = (dDist / CELL_SIZE)*cos(Theta);
	double dFreeDy = (dDist / CELL_SIZE)*sin(Theta);
	int x = RobotPos.getX() / CELL_SIZE + MAP_WIDTH / 2;
	int y = RobotPos.getY() / CELL_SIZE + MAP_HEIGHT / 2;
	int count = 0;

	// 증가분 정의
	int addX, addY;
	if (dx < 0) {
		addX = -1;
		dFreeDx = -dFreeDx;
		dx = -dx;
	}
	else addX = 1;
	if (dy < 0) {
		addY = -1;
		dy = -dy;
		dFreeDy = -dFreeDy;
	}
	else addY = 1;

	// Drawing
	int** map = m_GridMap->getMap();
	// 기울기 클때
	if (dx >= dy) {
		for (int i = 0; i < dx; i++) {
			x += addX;
			count += dy;
			if (count >= dx) {
				y += addY;
				count -= dx;
			}
			if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0) break;
			if(dFreeDx > i)
				map[x][y] = FREE_AREA;
			else
				map[x][y] = OCCUPIED_AREA;
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
			if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0) break;
			if (dFreeDy > i)
				map[x][y] = FREE_AREA;
			else
				map[x][y] = OCCUPIED_AREA;
		}
	}
			
	m_GridMap->setMap(map);
}

// getter, setter
int ** BuildMap::getMap() { return m_GridMap->getMap(); }
int BuildMap::getWidth() { return MAP_WIDTH; }
int BuildMap::getHeight() { return MAP_HEIGHT; }