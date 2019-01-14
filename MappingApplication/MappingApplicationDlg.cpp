
// MappingApplicationDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MappingApplication.h"
#include "MappingApplicationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
CMappingApplicationDlg::CMappingApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAPPINGAPPLICATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CMappingApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
}
BEGIN_MESSAGE_MAP(CMappingApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CANCEL, OnClickedCancel)
	ON_BN_CLICKED(IDC_SAVE, OnClickedSave)
	ON_WM_TIMER()
END_MESSAGE_MAP()
BOOL CMappingApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	ShowWindow(SW_SHOWMAXIMIZED);
	
	g_imgMap = Mat(SCREEN_H, SCREEN_W, CV_8UC3);
	g_mapBuilder = BuildMap();
	g_width = g_mapBuilder.getWidth();
	g_height = g_mapBuilder.getHeight();

	SetTimer(1000, 30, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}
void CMappingApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
void CMappingApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
HCURSOR CMappingApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CMappingApplicationDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
int CMappingApplicationDlg::getDistanceFromRobot(Mat map, int nDegree, int nDegreeResolution) {
	int cellSize = 50;
	int width = map.cols;
	int height = map.rows;
	double Theta = g_robotPos.getTheta() + (nDegree / nDegreeResolution)*PI / 180;
	double dx = (LASER_DATA_MAX / cellSize)*cos(Theta);
	double dy = (LASER_DATA_MAX / cellSize)*sin(Theta);
	int x = g_robotPos.getX() / cellSize + width / 2;
	int y = g_robotPos.getY() / cellSize + height / 2;
	int count = 0;

	int startX = x;
	int startY = y;
	// 증가분 정의
	int addX, addY;
	if (dx < 0) {
		addX = -1;
		dx = -dx;
	}
	else addX = 1;
	if (dy < 0) {
		addY = -1;
		dy = -dy;
	}
	else addY = 1;

	// 기울기 클때
	if (dx >= dy) {
		for (int i = 0; i < dx; i++) {
			x += addX;
			count += dy;
			if (count >= dx) {
				y += addY;
				count -= dx;
			}
			if (x >= width || x < 0 || y >= height || y < 0)
				return LASER_DATA_MAX;
			if (map.at<uint8_t>(y, x) == 0)
				return int(sqrt((x- startX)*(x - startX)+ (y - startY)*(y - startY)) * cellSize);
		}
	}
	// 기울기 작을때
	else {
		for (int i = 0; i < dy; i++) {
			y += addY;
			count += dx;
			if (count >= dy) {
				x += addX;
				count -= dy;
			}
			if (x >= width || x < 0 || y >= height || y < 0)
				return LASER_DATA_MAX;
			if (map.at<uint8_t>(y, x) == 0)
				return sqrt((x - startX)*(x - startX) + (y - startY)*(y - startY)) * cellSize;
			
		}
	}

	return 0;
}
int* CMappingApplicationDlg::getLaserToImage(int nDegreeResolution) {
	// Laser센서 배열을 초기화 합니다
	int * arrLaser = new int[360* nDegreeResolution];
	for (int i = 0; i < 360 * nDegreeResolution; i++)
		arrLaser[i] = 0;

	// 영상을 읽어옵니다
	Mat img = imread("./Map.png", 'r');

	// Robot의 위치로부터 가상의 Laser센서값을 생성합니다
	for (int i = 0; i < 360 * nDegreeResolution; i++)
		arrLaser[i] = getDistanceFromRobot(img, i, nDegreeResolution);

	return arrLaser;
}
void CMappingApplicationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 레이져 데이터를 Image에서 가져옵니다. Test용 입니다.
	int * arrLaser;
	int degreeResolution = 4;
	arrLaser = getLaserToImage(degreeResolution);
	for (int i = 0; i < 360 * degreeResolution; i++)
		g_mapBuilder.drawLine(g_robotPos, arrLaser[i], i, degreeResolution);

	// map -> image map
	int** map = g_mapBuilder.getMap();
	for (int i = 0; i < SCREEN_W; i++)
		for (int j = 0; j < SCREEN_H; j++) {
			int Lx = int(i*MAP_WIDTH / SCREEN_W);
			int Ly = int(j*MAP_HEIGHT / SCREEN_H);
			if (map[Lx][Ly] == FREE_AREA)
				g_imgMap.at<Vec3b>(j, i) = Vec3b(255, 255, 255);
			else if (map[Lx][Ly] == OCCUPIED_AREA)
				g_imgMap.at<Vec3b>(j, i) = Vec3b(0, 0, 0);
			else if (map[Lx][Ly] == UNKNOWN_AREA)
				g_imgMap.at<Vec3b>(j, i) = Vec3b(128, 128, 128);
		}

	// 로봇을 그립니다
	int x_pos = int(g_robotPos.getX() * SCREEN_W / CELL_SIZE / MAP_WIDTH + SCREEN_W / 2);
	int y_pos = int(g_robotPos.getY() * SCREEN_H / CELL_SIZE / MAP_HEIGHT + SCREEN_H / 2);
	circle(g_imgMap, Point(x_pos, y_pos), 10, Scalar(255, 0, 0), -1);
	line(g_imgMap, Point(x_pos, y_pos), Point(x_pos + int(cos(g_robotPos.getTheta()) * 30), y_pos + int(sin(g_robotPos.getTheta()) * 30)), Scalar(0, 0, 255), 5);

	// 로봇을 움직입니다
	if (g_bActibate) {
		g_robotPos.m_x += g_nSpeed*cos(g_robotPos.m_theta);
		g_robotPos.m_y += g_nSpeed*sin(g_robotPos.m_theta);
	}

	//화면에 보여주기 위한 처리입니다.
	int bpp = 8 * g_imgMap.elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	int padding = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
		padding = 4 - (g_imgMap.cols % 4);
	if (padding == 4)
		padding = 0;
	int border = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
		border = 4 - (g_imgMap.cols % 4);
	Mat mat_temp;
	if (border > 0 || g_imgMap.isContinuous() == false)
		cv::copyMakeBorder(g_imgMap, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	else
		mat_temp = g_imgMap;
	RECT r;
	m_picture.GetClientRect(&r);
	cv::Size winSize(r.right, r.bottom);
	cimage_mfc.Create(winSize.width, winSize.height, 24);
	BITMAPINFO *bitInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bitInfo->bmiHeader.biBitCount = bpp;
	bitInfo->bmiHeader.biWidth = mat_temp.cols;
	bitInfo->bmiHeader.biHeight = -mat_temp.rows;
	bitInfo->bmiHeader.biPlanes = 1;
	bitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo->bmiHeader.biCompression = BI_RGB;
	bitInfo->bmiHeader.biClrImportant = 0;
	bitInfo->bmiHeader.biClrUsed = 0;
	bitInfo->bmiHeader.biSizeImage = 0;
	bitInfo->bmiHeader.biXPelsPerMeter = 0;
	bitInfo->bmiHeader.biYPelsPerMeter = 0;
	//그레이스케일 인경우 팔레트가 필요
	if (bpp == 8){
		RGBQUAD* palette = bitInfo->bmiColors;
		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
	// Image is bigger or smaller than into destination rectangle
	// we use stretch in full rect
	if (mat_temp.cols == winSize.width  && mat_temp.rows == winSize.height){
		// source and destination have same size
		// transfer memory block
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width

		SetDIBitsToDevice(cimage_mfc.GetDC(),
			//destination rectangle
			0, 0, winSize.width, winSize.height,
			0, 0, 0, mat_temp.rows,
			mat_temp.data, bitInfo, DIB_RGB_COLORS);
	}
	else{
		// destination rectangle
		int destx = 0, desty = 0;
		int destw = winSize.width;
		int desth = winSize.height;

		// rectangle defined on source bitmap
		// using imgWidth instead of mat_temp.cols will ignore the padding border
		int imgx = 0, imgy = 0;
		int imgWidth = mat_temp.cols - border;
		int imgHeight = mat_temp.rows;

		StretchDIBits(cimage_mfc.GetDC(),
			destx, desty, destw, desth,
			imgx, imgy, imgWidth, imgHeight,
			mat_temp.data, bitInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	
	HDC dc = ::GetDC(m_picture.m_hWnd);
	cimage_mfc.BitBlt(dc, 0, 0);
	::ReleaseDC(m_picture.m_hWnd, dc);
	cimage_mfc.ReleaseDC();
	cimage_mfc.Destroy();

	CDialogEx::OnTimer(nIDEvent);
}
void CMappingApplicationDlg::OnClickedCancel() {
	if (MessageBox(_T("프로그램을 종료하시겠습니까?"), _T("Mapping App"), MB_YESNO) == IDYES) {
		OnCancel();
		OnDestroy();
	}
}
void CMappingApplicationDlg::OnClickedSave() {
	int width = g_mapBuilder.getWidth();
	int height = g_mapBuilder.getHeight();
	Mat img(height, width, CV_8UC1);
	int ** map = g_mapBuilder.getMap();
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++) {
			if (map[i][j] == FREE_AREA)
				img.at<uint8_t>(j, i) = 255;
			else if (map[i][j] == OCCUPIED_AREA)
				img.at<uint8_t>(j, i) = 0;
			else if (map[i][j] == UNKNOWN_AREA)
				img.at<uint8_t>(j, i) = 128;
		}
	imwrite("./citeMap.png", img);
	
	ofstream landmarkOut("landmark.txt");
	ofstream nodeOut("nodepoint.txt");
	if (landmarkOut.is_open()) {
		landmarkOut << g_landmark.size();
		for (int i = 0; i < g_landmark.size(); i++) {
			vector<int> line = g_landmark[i];
			for (int j = 0; j < line.size(); j++) {
				landmarkOut << line[j];
				landmarkOut << ", ";
			}
			landmarkOut << endl;
		}
	}
	if (nodeOut.is_open()) {
		nodeOut << g_nodePoint.size();
		for (int i = 0; i < g_nodePoint.size(); i++) {
			vector<int> line = g_nodePoint[i];
			for (int j = 0; j < line.size(); j++) {
				nodeOut << line[j];
				nodeOut << ", ";
			}
			nodeOut << endl;
		}
	}
}
