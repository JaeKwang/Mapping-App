#include "stdafx.h"
#include "NAV350Interface.h"

NAV350Interface::NAV350Interface(void)
{
	m_nRSSIData = m_putil.generateIntType1DArray(1440);
	m_nLaserData = m_putil.generateIntType1DArray(360);
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
#if SICK_LMS100_WINSOCK_INIT
	if (WSAStartup(MAKEWORD(2,2), &lms_wsad)) 
	{
		WSACleanup();
	}
#endif

	(*pSocket) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*pSocket == INVALID_SOCKET) 
	{
		printf("Error at socket\n");		
		return false;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
//	clientService.sin_addr.s_addr = inet_addr( szAddr );
	clientService.sin_port = htons( port );

	//----------------------
	// Connect to server.
	if ( connect( *pSocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) 
	{
		printf("Failed to connect\n" );
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

bool NAV350Interface::NAV350_Set_Operating_Mode(SOCKET * pSocket, int nMode)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	char * psend_buf = send_buff;

	send_buff[0] = SICK_LIDAR_STX;
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

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

bool NAV350Interface::NAV350_Read_Deviceid(SOCKET * pSocket)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };
	
	send_buff[0] = SICK_LIDAR_STX;
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

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
bool NAV350Interface::NAV350_Set_User_level(SOCKET * pSocket, int nUserLevel)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };


	send_buff[0] = SICK_LIDAR_STX;
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

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
bool NAV350Interface::NAV350_send_comm_msg(SOCKET * pSocket, char * szmsg, int msg_len)
{
	int send_len = 0;
	send_len = send(*pSocket, szmsg, msg_len, 0);
	if (send_len != msg_len)
	{
		return false;
	}
	return true;
}
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
bool NAV350Interface::NAV350_Set_Positioningdata(SOCKET * pSocket, bool bOutputmode, bool bShowOptParam)
{
	bool snd_result, rcv_result;

	int  recv_msg_len = -1;
	char recv_buff[SICK_MAX_RCV_BUFF] = { 0 };

	int  send_msg_len = 0;
	char send_buff[32] = { 0 };

	send_buff[0] = SICK_LIDAR_STX;
	send_buff[strlen(send_buff)] = SICK_LIDAR_ETX;

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