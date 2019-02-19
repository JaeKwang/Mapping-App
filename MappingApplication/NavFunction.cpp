#include "stdafx.h"
#include "afxwin.h"
#include "NavFunction.h"

NavFunction::NavFunction() {
	m_NAV350 = new NAV350Interface();
	isConnected = false;
}
bool NavFunction::connectionNAV350(ERR_MESSAGE * msg)
{
	if (isConnected) {
		*msg = ALREADY_CONNECTED;
		return false;
	}
	if (!m_NAV350->NAV350_Connect(&m_sSocket, (char *)strIPNav.c_str(), portNAV))
		return false;// 장치 연결
	if (!m_NAV350->NAV350_Read_Deviceid(&m_sSocket))
		return false;// device id 습득
	if (!m_NAV350->NAV350_Set_User_level(&m_sSocket, 3))
		return false;// User level 설정 (뒤에 3이 supervisor)
	
	TRACE("NAV350 CONNECTION\n\n");	

	changeOperationMode(STANDBY_MODE);
	m_NAV350->NAV350_Erase_Layout(&m_sSocket, 1);
	m_NAV350->NAV350_Set_Currentlayer(&m_sSocket, 0);
	m_NAV350->NAV350_Set_Positioningdata(&m_sSocket, 1, 0);
	
	isConnected = true;
	return isConnected;
}
void NavFunction::ResetNAV350()
{
	changeOperationMode(STANDBY_MODE);
	m_NAV350->NAV350_Device_Reset(&m_sSocket);
}
bool NavFunction::hasLandmark(){
	LandmarkData listLandmark;
	m_NAV350->NAV350_Read_Landmark(&m_sSocket, 3, &listLandmark);
	return false;
}
void NavFunction::changeOperationMode(int nMode)
{
	m_NAV350->NAV350_Set_Operating_Mode(&m_sSocket, nMode);	// 1번 모드는 Standby 2번 모드는 Mapping 3번 모드는 Detection 4번 모드는 Navigation
	NAV_nmode = nMode;
}
void NavFunction::NavigationMode()
{
	int nNAV_Data_Request = m_NAV350->NAV350_Position_Data_Request(&m_sSocket, 0, 2);

	while (nNAV_Data_Request != 0 && nNAV_Data_Request != 4)
	{
		//Sleep(10);
		nNAV_Data_Request = m_NAV350->NAV350_Position_Data_Request(&m_sSocket, 0, 2);
	}

	int_1DArray nLaserData = m_NAV350->getLaserData();
	list <LandmarkData> listLandmark = m_NAV350->getLandmarkData();
	int_1DArray pose = m_NAV350->getRobotPose();
	NAV350_X = pose[0];
	NAV350_Y = pose[1];
	NAV350_Theta = pose[2];
	
}
bool NavFunction::setReflector(int nX, int nY, int nTheta) {
	m_NAV350->NAV350_Set_LandmarkFormat(&m_sSocket, 0, 0, 0);
	
	// cylindric
	m_NAV350->NAV350_Set_Reflector_Type(&m_sSocket, 2);
	m_NAV350->NAV350_Set_Reflector_Size(&m_sSocket, 88);

	/*
	// flat
	m_NAV350->NAV350_Set_Reflector_Type(&m_sSocket, 1);
	m_NAV350->NAV350_Set_Reflector_Size(&m_sSocket, 150);
	*/
	
	return TRUE;
}
bool NavFunction::addLandmark(int nNumLandmark, list<LandmarkData> Landmark, RobotPose pose) {
	m_NAV350->NAV350_Set_Operating_Mode(&m_sSocket, MAPPING_MODE);
	m_NAV350->NAV350_Configure_Mapping(&m_sSocket, 50, 0, pose.x, pose.y, pose.theta);
	m_NAV350->NAV350_Set_Currentlayer(&m_sSocket, 0);
	m_NAV350->NAV350_Add_Landmark(&m_sSocket, nNumLandmark, Landmark);
	m_NAV350->NAV350_Set_Operating_Mode(&m_sSocket, NAVIGATION_MODE);
	return true;
}
int NavFunction::getNAVMode() {
	return NAV_nmode;
}
void NavFunction::LandmarkMode(){
	m_NAV350->NAV350_Get_Landmark(&m_sSocket, 1, 1);
}
int_1DArray NavFunction::getNAVData() {
	return m_NAV350->getLaserData();
}
list<LandmarkData> NavFunction::getLandmarkData() {
	return m_NAV350->getLandmarkData();
}
void NavFunction::getRobotPos(int * dX, int * dY, double * dTheta) {
	*dX = int(NAV350_X);
	*dY = int(NAV350_Y);
	*dTheta = (NAV350_Theta / 1000)*PI / 180; // mdeg -> deg -> raqdian
}