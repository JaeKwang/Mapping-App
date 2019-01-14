#pragma once

#define INITIAL_PROBABILITY 0.5

#define FREE_AREA 0
#define UNKNOWN_AREA 1
#define OCCUPIED_AREA 2
class GridMap {
private:
	int **m_Map;
	int m_width;
	int m_height;
	
public:
	int getWidth();
	int getHeight();
	int** getMap();
	void setMap(int ** nMap);

	GridMap(int nX, int nY);
	~GridMap();

};