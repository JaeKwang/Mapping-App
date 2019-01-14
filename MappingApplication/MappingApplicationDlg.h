
// MappingApplicationDlg.h : 헤더 파일
//

#pragma once
#include "stdafx.h"
#include "afxwin.h"

#define SCREEN_W 800
#define SCREEN_H 800

// CMappingApplicationDlg 대화 상자
class CMappingApplicationDlg : public CDialogEx
{
// 생성입니다.
public:
	CMappingApplicationDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	
	// ROBOT
	RobotPose g_robotPos = RobotPose(0, 0, 0);
	bool g_bActibate = true;
	int g_nSpeed = 5;

	// MAP
	Mat g_imgMap;
	BuildMap g_mapBuilder;
	int g_width, g_height;

	// NAV
	NAV350Interface * g_NAV350;
	SOCKET g_sSocket;
	string g_strIPNav = "192.168.30.100";

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPPINGAPPLICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_picture;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int getDistanceFromRobot(Mat map, int nDegree, int nDegreeResolution);
	int* getLaserToImage(int nDegreeResolution);

	Mat map_img;
	CImage cimage_mfc;
};
