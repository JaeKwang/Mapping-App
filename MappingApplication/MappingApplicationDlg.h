
// MappingApplicationDlg.h : ��� ����
//

#pragma once
#include "stdafx.h"
#include "afxwin.h"

#define SCREEN_W 1920
#define SCREEN_H 1080

// CMappingApplicationDlg ��ȭ ����
class CMappingApplicationDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMappingApplicationDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	
	// ROBOT
	RobotPose g_robotPos = RobotPose(0, 0, 0);
	bool g_bActibate = true;
	int g_nSpeed = 20;

	// MAP
	Mat g_imgMap;
	BuildMap g_mapBuilder;
	int g_width, g_height;
	vector<vector<int>> g_landmark; // Landmark
	vector<vector<int>> g_nodePoint; // Landmark

	// NAV
	NAV350Interface * g_NAV350;
	SOCKET g_sSocket;
	string g_strIPNav = "192.168.30.100";

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPPINGAPPLICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClickedCancel();
	afx_msg void OnClickedSave();
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
