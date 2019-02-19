//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "NAV350Interface.h"

NAV350Interface::NAV350Interface(void)
{
	m_nRSSIData = m_putil.generateIntType1DArray(1440);
	m_nLaserData = m_putil.generateIntType1DArray(1440);
	m_nRobotPose = m_putil.generateIntType1DArray(3);
}
NAV350Interface::~NAV350Interface(void)
{
}

/**
@brief Korean: NAV350 센서와 컴퓨터 간의 통신 연결을 수행하는 함수
@brief English: Function for the connection between NAV350 and computer
*/
bool NAV350Interface::NAV350_Connect(SOCKET * pSocket, char *szAddr, int port)
{
	WSADATA lms_wsad;
	if (WSAStartup(MAKEWORD(2, 2), &lms_wsad))
		WSACleanup();

	(*pSocket) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*pSocket == INVALID_SOCKET)
	{
		TRACE("Error at socket\n");
		return false;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(szAddr);
	clientService.sin_port = htons(port);
	
	//----------------------
	// Connect to server.
	if (connect(*pSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		TRACE("Failed to connect\n");
		return false;
	}
	return true;
}

/**
@brief Korean: NAV350 센서와 컴퓨터 간의 통신 연결을 종료하는 함수
@brief English: Function for disconnecting the connection between NAV350 and computer
*/
void NAV350Interface::NAV350_Disconnect(SOCKET * pSocket)
{
	closesocket(*pSocket);

#if SICK_LMS100_WINSOCK_INIT
	WSACleanup();
#endif
}

/**
@brief Korean: NAV350 센서에 특정 메시지 패킷을 보내는 함수
@brief English: Function sending a message packet to NAV350
*/
bool NAV350Interface::NAV350_send_comm_msg(SOCKET * pSocket, char * szmsg, int msg_len)
{
	int send_len = 0;
	send_len = send(*pSocket, szmsg, msg_len, 0);
	if (send_len != msg_len)
	{
		//TRACE "Send Msg length is not same.\n");
		return false;
	}
	return true;
}

/**
@brief Korean: NAV350 센서에 보낸 메시지 패킷에 따른 답신을 받아오는 함수
@brief English: Function for receive a message according to the message sended
*/
bool NAV350Interface::NAV350_recv_comm_msg(SOCKET * pSocket, char * psz_ret_msg, int max_msg_len, int * pret_msg_len)
{
	int rcv_len = SOCKET_ERROR;
	//Sleep(100);
	rcv_len = recv(*pSocket, psz_ret_msg, max_msg_len, 0);

	if (rcv_len == 0 || rcv_len == WSAECONNRESET)
	{
		*pret_msg_len = 0;
	}
	else
	{
		*pret_msg_len = rcv_len;
	}
	return true;
}

/**
@brief Korean: NAV350 센서의 ID를 읽어오는 함수
@brief English: Function for reading the ID of NAV350
*/
bool NAV350Interface::NAV350_Read_Deviceid(SOCKET * pSocket)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];

	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s%c", SICK_LIDAR_STX, "sRN DeviceIdent", SICK_LIDAR_ETX);
	//send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sRA DeviceIdent")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;
	}
	return false;
}

/**
@brief Korean: NAV350 센서의 현재 layer를 설정하는 함수
@brief English: Function for setting current layer
*/
bool NAV350Interface::NAV350_Set_Currentlayer(SOCKET * pSocket, int nLayer)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d%c", SICK_LIDAR_STX, "sWN NEVACurrLayer", nLayer, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NEVACurrLayer")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: Mapping 모드에 적용되는 Parameter를 설정하는 함수
@brief English: Function for setting the parameter to control the Mapping
*/
bool NAV350Interface::NAV350_Configure_Mapping(SOCKET * pSocket, int nScanNumber, bool bIsPositive, int nx, int ny, int nheading)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d %d %d %d %d%c", SICK_LIDAR_STX, "sWN NMAPMapCfg", nScanNumber, bIsPositive, nx, ny, nheading, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NMAPMapCfg")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}
		return rcv_result;
	}
	return false;
}

/**
@brief Korean: Positioning mode에서 미끄럼도를 설정하는 함수
@brief English: Function for setting a sliding mean
*/
bool NAV350Interface::NAV350_Set_Slidingmean(SOCKET * pSocket, int nSlidingmean)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d%c", SICK_LIDAR_STX, "sWN NPOSSlidingMean", nSlidingmean, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);

	if (snd_result == true)
	{

		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NPOSSlidingMean")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;
	}

	return false;
}

/**
@brief Korean: 위치 데이터를 전송 시간을 고려하여 추정된 데이터를 받을 것인지, 즉각 받은 데이터를 사용할 것인지를 결정하는 함수.
@brief English: Function for setting positioning data format. format : instant(0), extrapolated(1)
*/
bool NAV350Interface::NAV350_Set_Positioningdata(SOCKET * pSocket, bool bOutputmode, bool bShowOptParam)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d %d%c", SICK_LIDAR_STX, "sWN NPOSPoseDataFormat", bOutputmode, bShowOptParam, SICK_LIDAR_ETX);
	
	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NPOSPoseDataFormat")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;
	}
	return false;
}
bool NAV350Interface::NAV350_Store_Data_permanent(SOCKET * pSocket)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s%c", SICK_LIDAR_STX, "sMN mEEwriteall", SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mEEwriteall")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;
	}
	return false;
}
/**
@brief Korean: Landmark 데이터의 종류를 설정하는 함수.
@brief English: Function for set format of landmarks(cartesian or polar)
*/
bool NAV350Interface::NAV350_Set_LandmarkFormat(SOCKET * pSocket, bool bIsPolar, bool bIsEnable, int nLandmarkFilter)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d %d %d%c", SICK_LIDAR_STX, "sWN NLMDLandmarkDataFormat", bIsPolar, bIsEnable, nLandmarkFilter, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NLMDLandmarkDataFormat")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;
	}
	return false;
}

/**
@brief Korean: 레이저 스캔 정보 형식을 설정하는 함수.
@brief English: Function for setting format of scan data(distance, and remission)
*/
bool NAV350Interface::NAV350_Set_Scanformat(SOCKET * pSocket, int nMode, bool bRSSI)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d %d%c", SICK_LIDAR_STX, "sWN NAVScanDataFormat", nMode, bRSSI, SICK_LIDAR_ETX);
	
	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NAVScanDataFormat")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: 반사판의 크기를 설정하는 함수
@brief English: Function for the setting a size of reflectors
*/
bool NAV350Interface::NAV350_Set_Reflector_Size(SOCKET * pSocket, int nSize)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %x%c", SICK_LIDAR_STX, "sWN NLMDReflSize", nSize, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NLMDReflSize")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: 반사판의 종류를 설정하는 함수.
@brief English: Function for setting the reflector type (round or flat)
*/
bool NAV350Interface::NAV350_Set_Reflector_Type(SOCKET * pSocket, int nType)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX] = { 0 };

	char * psend_buf = send_buff;

	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d%c", SICK_LIDAR_STX, "sWN NLMDReflType", nType, SICK_LIDAR_ETX);
	

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NLMDReflType")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: 좌표계의 방향을 설정하는 함수. (시계방향 또는 반시계방향)
@brief English: Function for setting the coordinate system.(CW or CCW)
*/
bool NAV350Interface::NAV350_Set_Coordinate_System(SOCKET * pSocket, int nCoordinate)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d%c", SICK_LIDAR_STX, "sWN NEVACoordOrientation", nCoordinate, SICK_LIDAR_ETX);
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NEVACoordOrientation")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: NAV350이 인식할 수 있는 반사판의 수를 가까운 N개로 제한하는 함수
@brief English: Function for limiting the quantity of valid reflectors that are closest to NAV350
*/
bool NAV350Interface::NAV350_Set_Number_closest_reflector(SOCKET * pSocket, int nclosest)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	sprintf_s(send_buff, NAV_SEND_MAX, "%s %d", "sWN NLMDnClosest", nclosest);
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NLMDnClosest")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: NAV350 센서의 감지 거리를 설정하는 함수
@brief English: Function for defining an sensible area in the surrounding of NAV350
*/
bool NAV350Interface::NAV350_Set_Action_Radius(SOCKET * pSocket, int nMinActRadius, int nMaxActRadius)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	sprintf_s(send_buff, NAV_SEND_MAX, "%s %x %x", "sWN NLMDActionRadius", nMinActRadius, nMaxActRadius);
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NLMDActionRadius")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: 반사판 감지 오차를 고려하여 threshold를 설정하는 함수
@brief English: Function for setting threshold with respect to error of sensing reflector
*/
bool NAV350Interface::NAV350_Set_Reflector_Threshold(SOCKET * pSocket, int nPercent)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	sprintf_s(send_buff, NAV_SEND_MAX, "%s %d", "sWN NLMDReflThreshold", nPercent);
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sWA NLMDReflThreshold")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: 작동 모드를 설정하는 함수
@brief English: Function for setting operating mode
*/
bool NAV350Interface::NAV350_Set_Operating_Mode(SOCKET * pSocket, int nMode)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d%c", SICK_LIDAR_STX, "sMN mNEVAChangeState", nMode, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sMA mNEVAChangeState")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}
		//cout << "NAV350_Mode : " << nMode <<endl;
		return rcv_result;

	}
	return false;
}

/**
@brief Korean: User level을 설정하는 함수
@brief English: Function for setting user level
*/
bool NAV350Interface::NAV350_Set_User_level(SOCKET * pSocket, int nUserLevel)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[64] = { 0 };

	char * psend_buf = send_buff;


	if (nUserLevel == 2) {
		sprintf_s(send_buff, NAV_SEND_MAX, "%s %d %s", "sMN SetAccessMode", nUserLevel, "B21ACE26");
	}
	else if (nUserLevel == 3) {
		sprintf_s(send_buff, sizeof(send_buff), "%c%s %d %s%c", SICK_LIDAR_STX, "sMN SetAccessMode", nUserLevel, "F4724744", SICK_LIDAR_ETX);
	}

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN SetAccessMode")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: NAV350의 설정 상태를 초기 설정으로 되돌리는 함수
@brief English: Function for recalling default parameters
*/
bool NAV350Interface::NAV350_Device_Reset(SOCKET * pSocket)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX] = { 0 };

	char * psend_buf = send_buff;

	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s%c", SICK_LIDAR_STX, "sMN mNAVReset", SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNAVReset")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: Landmark를 추가하는 함수
@brief English: Function for adding landmarks
*/
bool NAV350Interface::NAV350_Add_Landmark(SOCKET * pSocket, int nNumLandmark, list<LandmarkData> Landmarks)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };
	int  send_msg_len = 0;
	char send_buff[800] = { 0 };//랜드마크 사이즈 개당 50잡고 현재800이니 16개
	char * pcur_rcv_msg = NULL;
	char   parse_buff[16] = { 0 };

	char * psend_buf = send_buff;

	sprintf_s(send_buff, 800, "%c%s %d", SICK_LIDAR_STX, "sMN mNLAYAddLandmark", nNumLandmark);
	
	list<LandmarkData> temp_Landmarks = Landmarks;
	for (int i = 0; i < nNumLandmark; i++) {
		LandmarkData Landmark = temp_Landmarks.front();
		sprintf_s(send_buff, 800, "%s %+d %+d %d %d %+d %d %d", psend_buf, Landmark.nX, Landmark.nY, Landmark.nType, Landmark.nRefType, Landmark.nSize, Landmark.nLayerID, Landmark.nLayersID);
		temp_Landmarks.pop_front();
	}

	sprintf_s(send_buff, 800, "%s %c", psend_buf, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);

	unsigned int tmp_unsigned_int = 0;
	int          tmp_signed_int = 0;
	int			 nErrorCode = 0;
	int			 nNumberLandmark = 0;

	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLAYAddLandmark")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);
		}

		if (pcur_rcv_msg) {

			pcur_rcv_msg += strlen("mNLAYAddLandmark") + 1; //set msg ptr to data start idx

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			nErrorCode = tmp_unsigned_int;

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
			nNumberLandmark = tmp_unsigned_int;

			list<int> nGlobalID;

			for (int ni = 0; ni < nNumLandmark; ni++) {
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
				int nID = tmp_unsigned_int;
				nGlobalID.push_back(nID);
			}
		}
		return rcv_result;
	}
	return false;
}

/**
@brief Korean: 특정 Landmark의 정보를 수정하는 함수
@brief English: Function for editing information of a landmark
*/
bool NAV350Interface::NAV350_Edit_Landmark(SOCKET * pSocket, int nNumLandmark, LandmarkData * Landmark)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	sprintf_s(send_buff, NAV_SEND_MAX, "%s %d", "sMN mNLAYSetLandmark", nNumLandmark);
	for (int ni = 0; ni < nNumLandmark; ni++) {
		sprintf_s(send_buff, NAV_SEND_MAX, " %d %d %d %d %d %d %d %d", Landmark[ni].nGlobalID, Landmark[ni].nX, Landmark[ni].nY,
			Landmark[ni].nType, Landmark[ni].nRefType, Landmark[ni].nSize, Landmark[ni].nLayerID, Landmark[ni].nLayersID);
	}
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLAYSetLandmark")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: 특정 Landmark의 정보를 삭제하는 함수
@brief English: Function for deleting the information of a landmark
*/
bool NAV350Interface::NAV350_Delete_Landmark(SOCKET * pSocket, int nNumLandmark, LandmarkData * Landmark)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	sprintf_s(send_buff, NAV_SEND_MAX, "%s %d", "sMN mNLAYDelLandmark", nNumLandmark);
	for (int ni = 0; ni < nNumLandmark; ni++) {
		sprintf_s(send_buff, NAV_SEND_MAX, " %d", Landmark[ni].nGlobalID);
	}
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLAYDelLandmark")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;

	}
	return false;
}

/**
@brief Korean: 현재 로봇 주위에 있는 Landmark 정보를 읽어오는 함수
@brief English: Function for reading information of landmarks around the robot
*/
bool NAV350Interface::NAV350_Read_Landmark(SOCKET * pSocket, int nNumLandmark, LandmarkData * Landmark)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };
	char * pcur_rcv_msg = NULL;
	char   parse_buff[16] = { 0 };

	char * psend_buf = send_buff;

	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d %d %d %d%c", SICK_LIDAR_STX, "sMN mNLAYGetLandmark", 3, 0, 1, 2, SICK_LIDAR_ETX);
	
	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);

	unsigned int tmp_unsigned_int = 0;
	int          tmp_signed_int = 0;
	int			 nErrorCode = 0;
	int			 nNumberLandmark = 0;

	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLAYGetLandmark")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		//TRACE"-- LMS LMCstartmeas Response (%d) -- \n %s \n", recv_msg_len, recv_buff);

		if (pcur_rcv_msg) {

			pcur_rcv_msg += strlen("mNLAYGetLandmark") + 1; //set msg ptr to data start idx

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			nErrorCode = tmp_unsigned_int;

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
			nNumberLandmark = tmp_unsigned_int;

			list<LandmarkData> nLandmark;

			for (int ni = 0; ni < nNumLandmark; ni++) {
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
				int nGlobalID = tmp_unsigned_int;
				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 6, &tmp_signed_int);
				int nX = tmp_unsigned_int;
				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 6, &tmp_signed_int);
				int nY = tmp_unsigned_int;
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nType = tmp_unsigned_int;
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nSubType = tmp_unsigned_int;
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
				int nSize = tmp_unsigned_int;
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 3, &tmp_unsigned_int);
				int nLayerID = tmp_unsigned_int;
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 3, &tmp_unsigned_int);
				int nLayersID = tmp_unsigned_int;

				LandmarkData tmp_Landmark;

				tmp_Landmark.nGlobalID = nGlobalID;
				tmp_Landmark.nX = nX;
				tmp_Landmark.nY = nY;
				tmp_Landmark.nRefType = nSubType;
				tmp_Landmark.nSize = nSize;
				tmp_Landmark.nLayerID = nLayerID;
				tmp_Landmark.nLayersID = nLayersID;

				nLandmark.push_back(tmp_Landmark);
			}
		}

		return rcv_result;
	}
	return false;
}

/**
@brief Korean: Mapping을 수행하는 함수
@brief English: Function for executing 'Mapping'
*/
int NAV350Interface::NAV350_Mapping(SOCKET * pSocket)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX] = { 0 };
	char * pcur_rcv_msg = NULL;
	char   parse_buff[16] = { 0 };

	char * psend_buf = send_buff;
	list<LandmarkData> nLandmark;

	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s%c", SICK_LIDAR_STX, "sMN mNMAPDoMapping", SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);

	unsigned int tmp_unsigned_int = 0;
	int          tmp_signed_int = 0;
	int			 nErrorCode = 10;

	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sMA mNMAPDoMapping")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		//TRACE"-- LMS LMCstartmeas Response (%d) -- \n %s \n", recv_msg_len, recv_buff);

		if (pcur_rcv_msg) {

			pcur_rcv_msg += strlen("sAN mNMAPDoMapping") + 1; //set msg ptr to data start idx

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			nErrorCode = tmp_unsigned_int;
			if (nErrorCode == 0) {
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nLandmarkAvailable = tmp_unsigned_int;

				if (nLandmarkAvailable == 1) {

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
					int nLandmarkFilter = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nNumLandmark = tmp_unsigned_int;

					for (int ni = 0; ni < nNumLandmark; ni++) {
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nCartesian = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 10, &tmp_signed_int);
						int nX = tmp_signed_int;
						pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 10, &tmp_signed_int);
						int nY = tmp_signed_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nPolar = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nOptLandmarkData = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
						int nLocalID = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
						int nGlobalID = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nType = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nSubType = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
						int nQuality = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
						int nTimestamp = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
						int nSize = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
						int nHitcount = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
						int nMeanEcho = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
						int nIndexBegin = tmp_unsigned_int;
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
						int nIndexEnd = tmp_unsigned_int;

						LandmarkData tmp_Landmark;

						tmp_Landmark.nGlobalID = nGlobalID;
						tmp_Landmark.nLocalID = nLocalID;
						tmp_Landmark.nX = nX;
						tmp_Landmark.nY = nY;
						tmp_Landmark.nRefType = nSubType;
						tmp_Landmark.nSize = nSize;

						nLandmark.push_back(tmp_Landmark);
					}

				}

				m_Landmark = nLandmark;

			}
			else return nErrorCode;
		}

		return nErrorCode;
	}
	return false;
}

/**
@brief Korean: 로봇의 현 위치 정보를 얻어오는 함수
@brief English: Function for getting current robot pose
*/
bool NAV350Interface::NAV350_Position_Request(SOCKET * pSocket, bool bWait)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };
	char * pcur_rcv_msg = NULL;
	char   parse_buff[16] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	sprintf_s(send_buff, NAV_SEND_MAX, "%s %d", "sMN mNPOSGetPose", bWait);
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);

	unsigned int tmp_unsigned_int = 0;
	int          tmp_signed_int = 0;
	int			 nErrorCode = 0;
	int			 nNumLandmark = 0;

	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sMA mNMAPDoMapping")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		//TRACE"-- LMS LMCstartmeas Response (%d) -- \n %s \n", recv_msg_len, recv_buff);
		if (pcur_rcv_msg) {

			pcur_rcv_msg += strlen("mNPOSGetData") + 1; //set msg ptr to data start idx

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			nErrorCode = tmp_unsigned_int;

			if (nErrorCode == 0) {
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nWait = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
				int nPoseData = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 10, &tmp_signed_int);
				int nX = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 10, &tmp_signed_int);
				int nY = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 7, &tmp_unsigned_int);
				int nHeading = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
				int nOptPoseData = tmp_unsigned_int;

				if (nOptPoseData == 1) {
					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nOutputMode = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nTimeStamp = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nMeanDev = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nNavMode = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nInfoState = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nNumReflectors = tmp_unsigned_int;
				}

				m_nRobotPose[0] = nX;
				m_nRobotPose[1] = nY;
				m_nRobotPose[2] = nHeading;
			}
			else if (nErrorCode == 4) return false;
			else return false;
		}
		return rcv_result;
	}
	return false;
}

/**
@brief Korean: 로봇의 현 위치 정보 및 스캔 정보와 반사판 정보를 얻어오는 함수
@brief English: Function for getting current robot pose, scan data, and reflector data around the robot
*/
int NAV350Interface::NAV350_Position_Data_Request(SOCKET * pSocket, bool bWait, int nDataset)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	char * pcur_rcv_msg = NULL;
	char   parse_buff[16] = { 0 };
	int	 nScanData1440[1440];
	int  nScanData360[360];
	int	 nRSSIData[1440];

	char * psend_buf = send_buff;

	unsigned int tmp_unsigned_int = 0;
	int          tmp_signed_int = 0;
	float		 tmp_float = 0;
	int			 nErrorCode = 10;
	list<LandmarkData> nLandmark;
	
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d %d%c", SICK_LIDAR_STX, "sMN mNPOSGetData", bWait, nDataset, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNPOSGetData")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		//TRACE"-- LMS LMCstartmeas Response (%d) -- \n %s \n", recv_msg_len, recv_buff);
		if (pcur_rcv_msg) {

			pcur_rcv_msg += strlen("sAN mNPOSGetData") + 1; //set msg ptr to data start idx

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			int nVersion = tmp_unsigned_int;

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			nErrorCode = tmp_unsigned_int;

			if (nErrorCode == 0) {
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nwait = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nmask = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nPoseData = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 10, &tmp_signed_int);
				int nX = tmp_signed_int;

				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 10, &tmp_signed_int);
				int nY = tmp_signed_int;

				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 7, &tmp_signed_int);
				int nHeading = tmp_signed_int;

				m_nRobotPose[0] = nX;
				m_nRobotPose[1] = nY;
				m_nRobotPose[2] = nHeading;
				
				//cout << "InterfaceNavPos : "<< m_nRobotPose[0] << " "<< m_nRobotPose[1]<< " " <<m_nRobotPose[2]<<endl;
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
				int nOptPoseData = tmp_unsigned_int;

				if (nOptPoseData == 1) {
					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
					int nOutputMode = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 9, &tmp_unsigned_int);
					int nTimeStamp = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
					int nMeanDev = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
					int nNavState = tmp_unsigned_int;

					m_nNavState = nNavState;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
					int nInfoState = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nUsedLandmark = tmp_unsigned_int;
				}
			}


			if (nErrorCode == 4 || nErrorCode == 0) {

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nIsLandmarkData = tmp_unsigned_int;

				if (nIsLandmarkData == 1) {
					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
					int nLandmarkFilter = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nNumReflector = tmp_unsigned_int;

					for (int ni = 0; ni < nNumReflector; ni++) {

						LandmarkData tmp_Landmark;

						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nCartesian = tmp_unsigned_int;

						if (nCartesian) {
							pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 6, &tmp_signed_int);
							int nX = tmp_signed_int;

							pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 6, &tmp_signed_int);
							int nY = tmp_signed_int;

							tmp_Landmark.nX = nX;
							tmp_Landmark.nY = nY;
							if (nX>80000 || nY>80000)
							{
								return true;//예외처리 반사판 쓰레기값 들어왔을 때//리턴값 0또는4가 제대로 된 결과일 때
							}

						}

						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nPolar = tmp_unsigned_int;

						if (nPolar) {
							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nDist = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 7, &tmp_unsigned_int);
							int nPhi = tmp_unsigned_int;
						}

						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nOptLandmarkData = tmp_unsigned_int;

						if (nOptLandmarkData)
						{
							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
							int nLocalID = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nGlobalID = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
							int nType = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
							int nRefType = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nQuality = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
							int nTimeStamp = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nSize = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nHitCount = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nMeanEcho = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nIndexBegin = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nIndexEnd = tmp_unsigned_int;


							tmp_Landmark.nGlobalID = nGlobalID;
							tmp_Landmark.nLocalID = nLocalID;
							tmp_Landmark.nRefType = nRefType;
							tmp_Landmark.nSize = nSize;

						}

						nLandmark.push_back(tmp_Landmark);

					}

				}

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nScanData32 = tmp_unsigned_int;

				pcur_rcv_msg += strlen("DIST1") + 1;

				pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
				float fScaleFactor = tmp_float;

				pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
				float fScaleOffset = tmp_float;

				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 8, &tmp_signed_int);
				int nStartangle = tmp_signed_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
				int nAngleRes = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
				int nTimeStampStart = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
				int nData = tmp_unsigned_int;
				for (int ni = 0; ni < nData; ni++) {
					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 8, &tmp_unsigned_int);
					nScanData1440[ni] = tmp_unsigned_int;
				}

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nScanData16 = tmp_unsigned_int;

				if (nScanData16 == 1) {

					pcur_rcv_msg += strlen("RSSI1") + 1;

					pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
					int nScaleFactor = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
					int nScaleOffset = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 8, &tmp_signed_int);
					int nStartangle = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
					int nAngleRes = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
					int nTimeStampStart = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
					int nNumData = tmp_unsigned_int;

					for (int ni = 0; ni < nNumData; ni++) {
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
						nRSSIData[ni] = tmp_unsigned_int;
					}
				}
			}
			if (nErrorCode != 0 && nErrorCode != 4) return nErrorCode;

			m_Landmark = nLandmark;

			for (int i = 0; i<1440; i++)
					m_nLaserData[i] = nScanData1440[i];
			
			for (int ni = 0; ni < 1440; ni++) {
				m_nRSSIData[ni] = nRSSIData[ni];
			}
		}
		return nErrorCode;
	}
	return true;
}

/**
@brief Korean: 로봇의 현재 위치를 입력하는 함수
@brief English: Function for setting current robot pose
*/
bool NAV350Interface::NAV350_Set_CurrentPose(SOCKET * pSocket, int nx, int ny, int nheading)
{
	bool snd_result, rcv_result;
	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	//sprintf_s(++psend_buf,"%s %d %d %d", "sMN mNPOSSetPose", nx, ny, nheading);
	sprintf_s(send_buff, NAV_SEND_MAX, "%s %x %x %d", "sMN mNPOSSetPose", nx, ny, nheading);			// %d가 아닌 %x 는 16진수로 표현하기 위함	
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLAYDelLandmark")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		return rcv_result;
	}
	return false;
}

/**
@brief Korean: 로봇 주변의 Landmark정보를 얻어오는 함수
@brief English: Function for getting landmarks currently recognized around the robot
*/
bool NAV350Interface::NAV350_Get_Landmark(SOCKET * pSocket, bool bWait, int nDataformat)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	char * pcur_rcv_msg = NULL;
	char   parse_buff[16] = { 0 };
	int	 nScanData1440[1440];
	int  nScanData360[360];
	int	 nRSSIData[1440];
	list<LandmarkData> nLandmark;

	char * psend_buf = send_buff;

	unsigned int tmp_unsigned_int = 0;
	int          tmp_signed_int = 0;
	float		 tmp_float = 0;
	int			 nErrorCode = 10;

	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d %d%c", SICK_LIDAR_STX, "sMN mNLMDGetData", bWait, nDataformat, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLMDGetData")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		}

		if (pcur_rcv_msg) {

			pcur_rcv_msg += strlen("sAN mNLMDGetData") + 1; //set msg ptr to data start idx

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			int nVersion = tmp_unsigned_int;

			pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
			nErrorCode = tmp_unsigned_int;

			if (nErrorCode == 0) {
				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
				int nwait = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
				int nmask = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
				int nLandmarkData = tmp_unsigned_int;

				if (nLandmarkData == 1) {
					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nLandmarkFilter = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
					int nNumReflector = tmp_unsigned_int;

					for (int ni = 0; ni < nNumReflector; ni++) {

						LandmarkData tmp_Landmark;

						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nCartesian = tmp_unsigned_int;

						if (nCartesian) {
							pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 6, &tmp_signed_int);
							int nX = tmp_signed_int;

							pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 6, &tmp_signed_int);
							int nY = tmp_signed_int;

							tmp_Landmark.nX = nX;
							tmp_Landmark.nY = nY;
						}

						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nPolar = tmp_unsigned_int;

						if (nPolar) {
							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nDist = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 7, &tmp_unsigned_int);
							int nPhi = tmp_unsigned_int;
						}

						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
						int nOptLandmarkData = tmp_unsigned_int;

						if (nOptLandmarkData) {
							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 2, &tmp_unsigned_int);
							int nLocalID = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nGlobalID = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
							int nType = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
							int nRefType = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nQuality = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
							int nTimeStamp = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
							int nSize = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nHitCount = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nMeanEcho = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nIndexBegin = tmp_unsigned_int;

							pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
							int nIndexEnd = tmp_unsigned_int;


							tmp_Landmark.nGlobalID = nGlobalID;
							tmp_Landmark.nLocalID = nLocalID;
							tmp_Landmark.nRefType = nRefType;
							tmp_Landmark.nSize = nSize;

						}

						nLandmark.push_back(tmp_Landmark);

					}

				}

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nScanData32 = tmp_unsigned_int;

				pcur_rcv_msg += strlen("DIST1") + 1;

				pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
				float fScaleFactor = tmp_float;

				pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
				float fScaleOffset = tmp_float;

				pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 8, &tmp_signed_int);
				int nStartangle = tmp_signed_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
				int nAngleRes = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
				int nTimeStampStart = tmp_unsigned_int;

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
				int nData = tmp_unsigned_int;

				for (int ni = 0; ni < nData; ni++) {
					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 8, &tmp_unsigned_int);
					nScanData1440[ni] = tmp_unsigned_int;
				}

				pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 1, &tmp_unsigned_int);
				int nScanData16 = tmp_unsigned_int;

				if (nScanData16 == 1) {

					pcur_rcv_msg += strlen("RSSI1") + 1;

					pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
					int nScaleFactor = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_real(pcur_rcv_msg, 10, &tmp_float);
					int nScaleOffset = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_signed_int(pcur_rcv_msg, 8, &tmp_signed_int);
					int nStartangle = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 5, &tmp_unsigned_int);
					int nAngleRes = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 10, &tmp_unsigned_int);
					int nTimeStampStart = tmp_unsigned_int;

					pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
					int nNumData = tmp_unsigned_int;

					for (int ni = 0; ni < nNumData; ni++) {
						pcur_rcv_msg = NAV350_util_parse_hex_unsigned_int(pcur_rcv_msg, 4, &tmp_unsigned_int);
						nRSSIData[ni] = tmp_unsigned_int;
					}
				}
			}
			if (nErrorCode != 0 && nErrorCode != 4) return nErrorCode != 0 ? true : false;
		}

		m_Landmark = nLandmark;
		for (int i = 0; i<1440; i++) {
				m_nLaserData[i] = nScanData1440[i];
		}

		
		for (int ni = 0; ni < 1440; ni++) {
			m_nRSSIData[ni] = nRSSIData[ni];
		}

		return nErrorCode != 0 ? true : false;
	}
	return false;
}

/**
@brief Korean: unsigned_int 타입 변수를 분석하여 char * 형으로 반환하는 함수
@brief English: Function for parsing a unsigned_int type variable and returning char * type variable
*/
char * NAV350Interface::NAV350_util_parse_hex_unsigned_int(char * pmsg, int size, unsigned int * pvalue)
{
	int template_idx = -1;
	char * ptemp_dest = 0;
	char * template_msg = "0123456789abcdefABCDEF";

	char buff[SICK_DATA_TYPE_BUFF_MAX] = { 0 };
	char * pcur_idx = pmsg;

	unsigned int cur_value = 0;
	unsigned int tmp_value = 0;
	int max_digit = -1; //최대 자릿수

	if (pcur_idx == NULL)
	{
		return NULL;
	}

	if (pcur_idx[0] == ' ')
	{
		pcur_idx++;
	}

	for (int i = 0; i < size * 2; i++)
	{
		buff[i] = pcur_idx[0];
		max_digit++;

		pcur_idx++;
		if (pcur_idx[0] == ' ' || pcur_idx[0] == SICK_LIDAR_ETX)
		{
			break;
		}
	}

	//데이터가 잘못되었음
	if (max_digit < 0)
	{
		return NULL;
	}

	if (pcur_idx[0] == ' ')
	{
		pcur_idx++;
	}

	for (int i = 0; i <= max_digit; i++)
	{
		ptemp_dest = strchr(template_msg, buff[i]);
		template_idx = (int)(ptemp_dest - template_msg);
		if (ptemp_dest == NULL)
		{
			//data is wrong
			return NULL;
		}
		tmp_value = template_idx;
		if (tmp_value > 15)
		{
			tmp_value -= 6;
		}
		cur_value |= tmp_value << (max_digit - i)*SICK_HALF_BYTE_SIZE_IN_BIT;
	}
	*pvalue = cur_value;

	return pcur_idx;
}

/**
@brief Korean: signed_int 타입 변수를 분석하여 char * 형으로 반환하는 함수
@brief English: Function for parsing a signed_int type variable and returning char * type variable
*/
char * NAV350Interface::NAV350_util_parse_hex_signed_int(char * pmsg, int size, int * pvalue)
{
	int template_idx = -1;
	char * ptemp_dest = 0;
	char * template_msg = "0123456789abcdefABCDEF";

	char buff[SICK_DATA_TYPE_BUFF_MAX] = { 0 };
	char * pcur_idx = pmsg;

	int cur_value = 0;
	int tmp_value = 0;
	int max_digit = -1; //최대 자릿수

	if (pcur_idx == NULL)
	{
		return NULL;
	}

	if (pcur_idx[0] == ' ')
	{
		pcur_idx++;
	}

	for (int i = 0; i < size * 2; i++)
	{
		buff[i] = pcur_idx[0];
		max_digit++;

		pcur_idx++;
		if (pcur_idx[0] == ' ' || pcur_idx[0] == SICK_LIDAR_ETX)
		{
			break;
		}
	}

	//데이터가 잘못되었음
	if (max_digit < 0)
	{
		return NULL;
	}

	if (pcur_idx[0] == ' ')
	{
		pcur_idx++;
	}

	for (int i = 0; i <= max_digit; i++)
	{
		ptemp_dest = strchr(template_msg, buff[i]);
		template_idx = (int)(ptemp_dest - template_msg);
		if (ptemp_dest == NULL)
		{
			//data is wrong
			return NULL;
		}
		tmp_value = template_idx;
		if (tmp_value > 15)
		{
			tmp_value -= 6;
		}
		cur_value |= tmp_value << (max_digit - i)*SICK_HALF_BYTE_SIZE_IN_BIT;
	}
	*pvalue = cur_value;

	return pcur_idx;
}

/**
@brief Korean: real 타입 변수를 분석하여 char * 형으로 반환하는 함수
@brief English: Function for parsing a real type variable and returning char * type variable
*/
char * NAV350Interface::NAV350_util_parse_hex_real(char * pmsg, int size, float * pvalue)
{
	int template_idx = -1;
	char * ptemp_dest = 0;
	char * template_msg = "0123456789abcdefABCDEF";

	char buff[SICK_DATA_TYPE_BUFF_MAX] = { 0 };
	char * pcur_idx = pmsg;

	unsigned int cur_value = 0;
	unsigned int tmp_value = 0;
	int max_digit = -1; //최대 자릿수

	unsigned int sign = 0;
	unsigned int exponent = 0;
	float        mantisse = 1.0f;

	float result = 0.0f;

	if (pcur_idx == NULL)
	{
		return NULL;
	}

	if (pcur_idx[0] == ' ')
	{
		pcur_idx++;
	}

	for (int i = 0; i < size * 2; i++)
	{
		buff[i] = pcur_idx[0];
		max_digit++;

		pcur_idx++;
		if (pcur_idx[0] == ' ' || pcur_idx[0] == SICK_LIDAR_ETX)
		{
			break;
		}
	}

	//데이터가 잘못되었음
	if (max_digit < 0)
	{
		return NULL;
	}

	if (pcur_idx[0] == ' ')
	{
		pcur_idx++;
	}

	for (int i = 0; i <= max_digit; i++)
	{
		ptemp_dest = strchr(template_msg, buff[i]);
		template_idx = (int)(ptemp_dest - template_msg);
		if (ptemp_dest == NULL)
		{
			//data is wrong
			return NULL;
		}
		tmp_value = template_idx;
		if (tmp_value > 15)
		{
			tmp_value -= 6;
		}
		cur_value |= tmp_value << (max_digit - i)*SICK_HALF_BYTE_SIZE_IN_BIT;
	}
	if (cur_value == 0)
	{
		*pvalue = 0.0f;
		return pcur_idx;
	}

	sign = (cur_value & (0x01 << 31)) >> 31;
	cur_value = cur_value << 1; cur_value = cur_value >> 1;

	exponent = (cur_value & (0xFF << 23)) >> 23;
	cur_value = cur_value << 9; cur_value = cur_value >> 9;

	for (int i = 0; i < 6; i++)
	{
		mantisse += ((float)((cur_value & (0x0F << (19 - i * 4))) >> (19 - i * 4))) / ((float)(0x10 << (i * 4)));
	}
	result = ((sign == 0) ? 1.0f : -1.0f) * mantisse * ((float)(0x01 << (exponent - 127)));

	*pvalue = result;

	return pcur_idx;
}

int_1DArray NAV350Interface::getLaserData()
{
	return m_nLaserData;
}

int_1DArray NAV350Interface::getRobotPose()
{
	return m_nRobotPose;
}

int_1DArray NAV350Interface::getRSSIData()
{
	return m_nRSSIData;
}

list<LandmarkData> NAV350Interface::getLandmarkData()
{
	return m_Landmark;
}

int NAV350Interface::getNavState()
{
	return m_nNavState;
}
bool NAV350Interface::NAV350_Store_Layout_permanent(SOCKET * pSocket)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s%c", SICK_LIDAR_STX, "sMN mNLAYStoreLayout", SICK_LIDAR_ETX);
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLAYStoreLayout")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);
		}

		return rcv_result;
	}
	return false;
}
/**
@brief Korean: Layout의 모든 Landmark 정보를 삭제하는 함수
@brief English: Function for deleting the information of a landmark
*/
bool NAV350Interface::NAV350_Erase_Layout(SOCKET * pSocket, int nErase)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[NAV_SEND_MAX];
	sprintf_s(send_buff, NAV_SEND_MAX, "%c%s %d%c", SICK_LIDAR_STX, "sMN mNLAYEraseLayout", nErase, SICK_LIDAR_ETX);

	send_msg_len = strlen(send_buff);
	snd_result = NAV350_send_comm_msg(pSocket, send_buff, send_msg_len);
	if (snd_result == true)
	{
		rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);

		char * pcur_rcv_msg = NULL;

		while (!pcur_rcv_msg) {

			if (pcur_rcv_msg = strstr(recv_buff, "sAN mNLAYEraseLayout")) break;
			rcv_result = NAV350_recv_comm_msg(pSocket, recv_buff, SICK_MAX_RCV_BUFF, &recv_msg_len);
		}
		return rcv_result;
	}
	return false;
}