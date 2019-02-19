#pragma once

#include "stdafx.h"
#include "afxwin.h"
#include "NAV350Interface.h"

enum NAV_MODE {
	OFF_MODE, STANDBY_MODE, MAPPING_MODE, LANDMARK_MODE, NAVIGATION_MODE
};
enum ERR_MESSAGE {
	ALREADY_CONNECTED = -1, LOGIN_FAILED = -2
};
class NavFunction {
private:
	bool isConnected;
	NAV350Interface * m_NAV350;
	SOCKET m_sSocket;
	string strIPNav = "192.168.1.10";
	int portNAV = 2111;
	int NAV_nmode;
	double m_nXoffsetNav;
	double m_nYoffsetNav;
	double m_nZoffsetNav;

	// NAV350 variables
	double NAV350_X;
	double NAV350_Y;
	double NAV350_Theta;

public:
	NavFunction();
	bool hasLandmark();
	bool connectionNAV350(ERR_MESSAGE * msg);
	bool setReflector(int nX, int nY, int nTheta);
	bool addLandmark(int nNumLandmark, list<LandmarkData> Landmark, RobotPose pose);

	void ResetNAV350();
	void changeOperationMode(int nMode);
	void LandmarkMode();
	void NavigationMode();

	int getNAVMode();
	int_1DArray getNAVData();
	list<LandmarkData> getLandmarkData();
	void getRobotPos(int * dX, int * dY, double * dTheta);
};