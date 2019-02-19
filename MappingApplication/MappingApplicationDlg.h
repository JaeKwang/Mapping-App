
// MappingApplicationDlg.h : 헤더 파일
//

#pragma once
#include "stdafx.h"
#include "afxwin.h"
#include "NavFunction.h"

#define SCREEN_W 1920
#define SCREEN_H 1080

#define DEGREE_RESOLUTION 4

// CMappingApplicationDlg 대화 상자
class CMappingApplicationDlg : public CDialogEx
{
// 생성입니다.
public:
	CMappingApplicationDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	
	// ROBOT
	RobotPose g_robotPos = RobotPose(0, 0, 0);
	bool g_bActibate = true;
	int g_nSpeed = 20;

	// MAP
	Mat g_imgMap;
	BuildMap g_mapBuilder;
	int g_width, g_height;
	list<LandmarkData> g_landmark; // Landmark
	list<LandmarkData> g_storedLandmark; // Landmark
	vector<vector<int>> g_nodePoint; // Landmark

	// NAV Function
	NavFunction g_navFunc;

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
	afx_msg void OnClickedCancel();
	afx_msg void OnClickedSave();
	DECLARE_MESSAGE_MAP()

public:
	CStatic m_picture;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int getDistanceFromRobot(Mat map, int nDegree, int nDegreeResolution);
	int* getLaserToImage(int nDegreeResolution);
	Mat overlayImage(Mat background, Mat foreground, Point2i location);

	Mat map_img;
	Mat map_temp;
	Mat landMark_img;
	CImage cimage_mfc;

	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedButton6();
	CButton m_PowerBtn;
	CButton m_LandMarkBtn;
	afx_msg void OnBnClickedLandmark();
	CButton m_SetReflectBtn;
	afx_msg void OnBnClickedReflector();
	afx_msg void OnBnClickedMappig();
	afx_msg void OnBnClickedNavigation();
	afx_msg void OnStnClickedPicture();
	afx_msg void OnBnClickedReset();
};
