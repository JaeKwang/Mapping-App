/*______________________________________________________________________________________________
PROJECT : Intelligent Robotics Lab. Department of Mechanical Engineering Korea University                                    
(c) Copyright 2013 KUME Intelligent Robotics Lab.                                             
All rights reserved                                                                         
$Directed by Jae-Bok Song                                                                     
$Designed by Dong-Il Kim                                                                
$Description : SICK NAV350(Omnidirectional Laser rangefinder able to recognize reflectors) operating code
$Data: 2013/11                                                                          
$Author: Dong-Il Kim                                                                   
______________________________________________________________________________________________*/

#define SICK_LIDAR_STX 0x02
#define SICK_LIDAR_ETX 0x03

#define SICK_DATA_TYPE_BUFF_MAX		32  
#define SICK_MAX_RCV_BUFF		20480
#define SICK_HALF_BYTE_SIZE_IN_BIT	4

#define USE_REAR_LASER		0
#define NAV350_SCAN_FREQ	50

#define SICK_LMS100_WINSOCK_INIT	1

#define SICK_NAV350_FRONT_ADDR "192.168.30.100"
//#define SICK_NAV350_FRONT_ADDR "163.152.55.3"
//#define SICK_NAV350_FRONT_ADDR "192.168.60.189"
//#define SICK_NAV350_FRONT_ADDR "192.168.0.47"
#define SICK_NAV350_FRONT_PORT 2111

#define SICK_NAV350_FRONT_ADDR "192.168.30.100"
//#define SICK_NAV350_REAR_ADDR "163.152.55.3"
//#define SICK_NAV350_REAR_ADDR "192.168.60.189"
//#define SICK_NAV350_FRONT_ADDR "192.168.0.47"
#define SICK_NAV350_REAR_PORT 2112

#define SICK_LMS100_MAX_QUERY_COUNT 120
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma once

#include <iostream>
#include <list>
#include "Winsock2.h"
#include "PUtil.h"

#pragma comment(lib, "ws2_32")

using namespace std;

struct LandmarkData
{
	int nGlobalID;
	int nLocalID;
	int nX;
	int nY;
	static const int nType = 1;
	int nRefType;
	int nSize;
	int nLayerID;
	int nLayersID;
};


class NAV350Interface
{
public:
	NAV350Interface(void);
	~NAV350Interface(void);

private:
	WSADATA lms_wsad;
	list<LandmarkData> m_Landmark;
	int_1DArray m_nRobotPose;
	int_1DArray m_nLaserData;
	int_1DArray m_nRSSIData;

	PUtil m_putil;

	/* NAV350을 통해서 얻은 위치값의 상태를 의미 ex)가상모드, 실제모드 등...*/
	int m_nNavState;

public:
	bool NAV350_Connect(SOCKET * pSocket, char *szAddr, int port);
	void NAV350_Disconnect(SOCKET * pSocket);

/* Communication */

	bool NAV350_send_comm_msg(SOCKET * pSocket, char * szmsg, int msg_len);
	bool NAV350_recv_comm_msg(SOCKET * pSocket, char * psz_ret_msg, int max_msg_len, int * pret_msg_len);

/* Setting & Read status */

	bool NAV350_Read_Deviceid(SOCKET * pSocket);
	bool NAV350_Set_Currentlayer(SOCKET * pSocket, int nLayer);
	bool NAV350_Store_Data_permanent(SOCKET * pSocket);
	bool NAV350_Configure_Mapping(SOCKET * pSocket, int nScanNumber, bool bIsPositive, int nx, int ny, int nheading);
	bool NAV350_Set_Slidingmean(SOCKET * pSocket, int nSlidingmean);
	bool NAV350_Set_Positioningdata(SOCKET * pSocket, bool bOutputmode, bool bShowOptParam);
	bool NAV350_Set_LandmarkFormat(SOCKET * pSocket, bool bIsPolar, bool bIsEnable, int nLandmarkFilter);
	bool NAV350_Set_Scanformat(SOCKET * pSocket, int nMode, bool bRSSI);
	bool NAV350_Set_Reflector_Size(SOCKET * pSocket, int nSize);
	bool NAV350_Set_Reflector_Type(SOCKET * pSocket, int nType);
	bool NAV350_Set_Coordinate_System(SOCKET * pSocket, int nCoordinate);
	bool NAV350_Set_Number_closest_reflector(SOCKET * pSocket, int nclosest);
	bool NAV350_Set_Action_Radius(SOCKET * pSocket, int nMinActRadius, int nMaxActRadius);
	bool NAV350_Set_Reflector_Threshold(SOCKET * pSocket, int nPercent);

	bool NAV350_Set_Operating_Mode(SOCKET * pSocket, int nMode);
	bool NAV350_Set_User_level(SOCKET * pSocket, int nUserLevel);
	bool NAV350_Device_Reset(SOCKET * pSocket);
	bool NAV350_Erase_Layout(SOCKET * pSocket, int nErase);
	bool NAV350_Store_Layout_permanent(SOCKET * pSocket);
/* STANDBY Mode */

	bool NAV350_Add_Landmark(SOCKET * pSocket, int nNumLandmark, list <LandmarkData> Landmark);
	bool NAV350_Edit_Landmark(SOCKET * pSocket, int nNumLandmark, LandmarkData * Landmark);
	bool NAV350_Delete_Landmark(SOCKET * pSocket, int nNumLandmark, LandmarkData * Landmark);
	bool NAV350_Read_Landmark(SOCKET * pSocket, int nNumLandmark, LandmarkData * Landmark);

/* MAPPING Mode */

	int NAV350_Mapping(SOCKET * pSocket);

/* NAVIGATION Mode */

	bool NAV350_Position_Request(SOCKET * pSocket, bool bWait);
	int NAV350_Position_Data_Request(SOCKET * pSocket, bool bWait, int nDataset);
	bool NAV350_Set_CurrentPose(SOCKET * pSocket, int nx, int ny, int nheading);

/* LANDMARK Mode */

	bool NAV350_Get_Landmark(SOCKET * pSocket, bool bWait, int nDataformat);
	
/* Parse */
	char * NAV350_util_parse_hex_unsigned_int(char * pmsg, int size, unsigned int * pvalue);
	char * NAV350_util_parse_hex_signed_int	 (char * pmsg, int size, int * pvalue);
	char * NAV350_util_parse_hex_real	 (char * pmsg, int size, float * pvalue);

/* Get Data */
	int_1DArray getLaserData();
	int_1DArray getRSSIData();
	int_1DArray getRobotPose();
	list<LandmarkData> getLandmarkData();
	int getNavState();
};

