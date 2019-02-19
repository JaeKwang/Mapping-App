#pragma once

#include "GridMap.h"
#include <vector>

#define PI 3.141592

// ���� ����Ǵ� Grid Map�� ũ��
#define MAP_WIDTH 3000 //PIXEL
#define MAP_HEIGHT 3000

#define CELL_SIZE 10			// 1 �ȼ��� �� mm�ϱ��?
#define MAX_PROBABILITY 0.8
#define MIN_PROBABILITY 0.2
#define GAUSSIAN_SD 0.4
#define THICKNESS_OF_WALL 20	// mm ����
#define LASER_DATA_MAX 15000	// mm ����

using namespace std;

class CPAGVDlg;

class RobotPose {
public:
	int x, y;
	double theta; // mm, mm, radian
	RobotPose(int dX, int dY, double dTheta);
};

class BuildMap
{
private :
	RobotPose * m_curPos;
	GridMap * m_GridMap;

public:
	BuildMap();
	void BuildMap::drawLine(RobotPose RobotPos, double dDist, int nIndex, int nDegreeResolution);
	void saveImgTemp();
	void getImgTemp();

	int ** getMap();
	int getWidth();
	int getHeight();
	void clearMap();

};
