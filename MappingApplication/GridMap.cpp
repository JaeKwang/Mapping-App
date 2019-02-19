#include "stdafx.h"
#include "GridMap.h"

GridMap::GridMap(int nX, int nY) {
	m_width = nX;
	m_height = nY;

	// Grid Map �޸� �Ҵ�
	m_Map = new int*[nX];
	if (m_Map)
		for (int i = 0; i < nX; i++)
			m_Map[i] = new int[nY];
	
	// 2���� �迭�� �ʱ�ȭ �մϴ�
	for (int i = 0; i<m_width; i++)
		for (int j = 0; j<m_height; j++)
			m_Map[i][j] = UNKNOWN_AREA;
}
void GridMap::clearMap() {
	// 2���� �迭�� �ʱ�ȭ �մϴ�
	for (int i = 0; i<m_width; i++)
		for (int j = 0; j<m_height; j++)
			m_Map[i][j] = UNKNOWN_AREA;
}
void GridMap::saveMapTemp() {
	m_MapTemp = m_Map;
}
void GridMap::getMapTemp() {
	m_Map = m_MapTemp;
}
GridMap::~GridMap() {
	// Grid Map �޸� ����
	if (m_Map != nullptr) {
		for (int i = 0; i < m_width; i++) {
			delete[] m_Map[i];
			m_Map[i] = nullptr;
		}
		delete[] m_Map;
	}
}

// getter, setter
int GridMap::getWidth(){	return m_width;}
int GridMap::getHeight(){	return m_height;}
int** GridMap::getMap() {	return m_Map;}
void GridMap::setMap(int ** Map) {	m_Map = Map;}

