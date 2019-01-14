#pragma once

#include "GridMap.h"
#include <vector>

#define PI 3.141592

// 실제 저장되는 Grid Map의 크기
#define MAP_WIDTH 4000
#define MAP_HEIGHT 4000

#define CELL_SIZE 10			// 1 픽셀당 몇 mm일까요?
#define MAX_PROBABILITY 0.8
#define MIN_PROBABILITY 0.2
#define GAUSSIAN_SD 0.4
#define THICKNESS_OF_WALL 100	// mm 단위
#define LASER_DATA_MAX 15000	// mm 단위

using namespace std;

class CPAGVDlg;

class RobotPose {
public:
	double m_x, m_y, m_theta; // mm, mm, radian

public:
	RobotPose(double dX, double dY, double dTheta);
	double getX();
	double getY();
	double getTheta();
};

class BuildMap
{
private :
	RobotPose * m_curPos;
	GridMap * m_GridMap;

public:
	BuildMap();
	void BuildMap::drawLine(RobotPose RobotPos, double dDist, int nIndex, int nDegreeResolution);
	int ** getMap();
	int getWidth();
	int getHeight();

};
