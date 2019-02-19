
// MappingApplicationDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MappingApplication.h"
#include "MappingApplicationDlg.h"
#include "NAV350Interface.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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
	DDX_Control(pDX, IDC_BUTTON6, m_PowerBtn);
	DDX_Control(pDX, IDC_LANDMARK, m_LandMarkBtn);
	DDX_Control(pDX, IDC_REFLECTOR, m_SetReflectBtn);
}
BEGIN_MESSAGE_MAP(CMappingApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CANCEL, OnClickedCancel)
	ON_BN_CLICKED(IDC_SAVE, OnClickedSave)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON6, &CMappingApplicationDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_LANDMARK, &CMappingApplicationDlg::OnBnClickedLandmark)
	ON_BN_CLICKED(IDC_REFLECTOR, &CMappingApplicationDlg::OnBnClickedReflector)
	ON_BN_CLICKED(IDC_NAVIGATION, &CMappingApplicationDlg::OnBnClickedNavigation)
	ON_STN_CLICKED(IDC_PICTURE, &CMappingApplicationDlg::OnStnClickedPicture)
	ON_BN_CLICKED(IDC_RESET, &CMappingApplicationDlg::OnBnClickedReset)
END_MESSAGE_MAP()
BOOL CMappingApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.

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
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	ShowWindow(SW_SHOWMAXIMIZED);
	
	g_imgMap = Mat(SCREEN_H, SCREEN_W, CV_8UC3);
	g_mapBuilder = BuildMap();
	g_width = g_mapBuilder.getWidth();
	g_height = g_mapBuilder.getHeight();
	g_navFunc = NavFunction();

	ERR_MESSAGE * msg = new ERR_MESSAGE();
	if (!g_navFunc.connectionNAV350(msg)) {
		if (*msg == ALREADY_CONNECTED)
			MessageBox(_T("�̹� NAV350�� ����Ǿ��ֽ��ϴ�"), _T("Connection"));
		else if (*msg == LOGIN_FAILED)
			MessageBox(_T("NAV350�� ������ �����Ͽ����ϴ�"), _T("Connection"));
	}
	else {
		m_PowerBtn.EnableWindow(TRUE);
		m_LandMarkBtn.EnableWindow(TRUE);
		m_SetReflectBtn.EnableWindow(TRUE);
	}

	SetTimer(1000, 30, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}
void CMappingApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
		CDialogEx::OnSysCommand(nID, lParam);
}
void CMappingApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (g_navFunc.getNAVMode() != OFF_MODE) {
		g_navFunc.changeOperationMode(STANDBY_MODE);
		g_navFunc.changeOperationMode(OFF_MODE);
	}
}
int CMappingApplicationDlg::getDistanceFromRobot(Mat map, int nDegree, int nDegreeResolution) {
	int cellSize = 50;
	int width = map.cols;
	int height = map.rows;
	double Theta = g_robotPos.theta + (nDegree / nDegreeResolution)*PI / 180;
	double dx = (LASER_DATA_MAX / cellSize)*cos(Theta);
	double dy = (LASER_DATA_MAX / cellSize)*sin(Theta);
	int x = g_robotPos.x / cellSize + width / 2;
	int y = g_robotPos.y / cellSize + height / 2;
	int count = 0;

	int startX = x;
	int startY = y;
	// ������ ����
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

	// ���� Ŭ��
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
	// ���� ������
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
void CMappingApplicationDlg::OnTimer(UINT_PTR nIDEvent)
{
	int_1DArray arrLaser;
	double tempTheta;
	switch (g_navFunc.getNAVMode()) {
	case OFF_MODE:

		break;
	case STANDBY_MODE:
		break;
	case LANDMARK_MODE:
		//g_mapBuilder.clearMap();
		g_navFunc.LandmarkMode();
		g_landmark = g_navFunc.getLandmarkData();
		arrLaser = g_navFunc.getNAVData();
		//for (int i = 0; i < 360 * DEGREE_RESOLUTION; i++)
			//g_mapBuilder.drawLine(g_robotPos, arrLaser[i], i, DEGREE_RESOLUTION);
		break;
	case MAPPING_MODE:
		
		break;
	case NAVIGATION_MODE:
		g_navFunc.NavigationMode();
		tempTheta = g_robotPos.theta;
		g_navFunc.getRobotPos(&(g_robotPos.x), &(g_robotPos.y), &(g_robotPos.theta));
		g_landmark = g_navFunc.getLandmarkData();
		arrLaser = g_navFunc.getNAVData();
		if (abs(tempTheta - g_robotPos.theta) > 0.1 || g_landmark.size() < 3) break;
		for (int i = 0; i < 360 * DEGREE_RESOLUTION; i++) {
			if(arrLaser[i] > 0 && arrLaser[i] < LASER_DATA_MAX)
				g_mapBuilder.drawLine(g_robotPos, arrLaser[i], i, DEGREE_RESOLUTION);
		}
		break;
	}

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
	// LandMark�� �׸��ϴ�
	list<LandmarkData> tempLandmark = g_landmark;
	list<LandmarkData> tempStoredLandmark;
	while (!tempLandmark.empty()) {
		int x_L_pos = (g_robotPos.x + cos(g_robotPos.theta)*tempLandmark.front().nX - sin(g_robotPos.theta)*tempLandmark.front().nY)*SCREEN_W / CELL_SIZE / MAP_WIDTH + SCREEN_W / 2;
		int y_L_pos = (g_robotPos.y + sin(g_robotPos.theta)*tempLandmark.front().nX + cos(g_robotPos.theta)*tempLandmark.front().nY)*SCREEN_H / CELL_SIZE / MAP_HEIGHT + SCREEN_H / 2;
		tempLandmark.pop_front();
		cv::circle(g_imgMap, Point(x_L_pos, y_L_pos), 5, Scalar(0, 255, 0), -1);
	}
	tempStoredLandmark = g_storedLandmark;
	while (!tempStoredLandmark.empty()) {
		int x_L_pos = tempStoredLandmark.front().nX*SCREEN_W / CELL_SIZE / MAP_WIDTH + SCREEN_W / 2;
		int y_L_pos = tempStoredLandmark.front().nY*SCREEN_H / CELL_SIZE / MAP_HEIGHT + SCREEN_H / 2;
		tempStoredLandmark.pop_front();
		cv::line(g_imgMap, Point(x_L_pos-7, y_L_pos - 7), Point(x_L_pos+ 7, y_L_pos + 7), Scalar(0, 0, 255), 3);
		cv::circle(g_imgMap, Point(x_L_pos, y_L_pos), 7, Scalar(0, 0, 255), 3);
	}
	// �κ��� �׸��ϴ�
	int x_pos = int(g_robotPos.x * SCREEN_W / CELL_SIZE / MAP_WIDTH + SCREEN_W / 2);
	int y_pos = int(g_robotPos.y * SCREEN_H / CELL_SIZE / MAP_HEIGHT + SCREEN_H / 2);
	cv::circle(g_imgMap, Point(x_pos, y_pos), 10, Scalar(255, 0, 0), -1);
	cv::line(g_imgMap, Point(x_pos, y_pos), Point(x_pos + int(cos(g_robotPos.theta) * 30), y_pos + int(sin(g_robotPos.theta) * 30)), Scalar(0, 0, 255), 5);

	cv::flip(g_imgMap, g_imgMap, 0);

	// �κ� ��ġ, ���� ���
	string strX = "X pos: " + to_string(g_robotPos.x) +"mm";
	string strY = "Y pos: " + to_string(g_robotPos.y) + "mm";
	string strT = "Theta: " + to_string(int(g_robotPos.theta * 180 / PI)) + "deg";
	cv::putText(g_imgMap, strX, Point(SCREEN_W - 300, 30), 2, 0.8, Scalar::all(0), 1, -1);
	cv::putText(g_imgMap, strY, Point(SCREEN_W - 300, 60), 2, 0.8, Scalar::all(0), 1, -1);
	cv::putText(g_imgMap, strT, Point(SCREEN_W - 300, 90), 2, 0.8, Scalar::all(0), 1, -1);

	//ȭ�鿡 �����ֱ� ���� ó���Դϴ�.
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
	//�׷��̽����� �ΰ�� �ȷ�Ʈ�� �ʿ�
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
	if (MessageBox(_T("���α׷��� �����Ͻðڽ��ϱ�?"), _T("Mapping App"), MB_YESNO) == IDYES) {
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
	list<LandmarkData> tempLandmark = g_landmark;
	if (landmarkOut.is_open()) {
		landmarkOut << tempLandmark.size();
		landmarkOut << endl;
		while (!tempLandmark.empty()) {
			LandmarkData l = tempLandmark.front();
			landmarkOut << l.nGlobalID << ", ";
			landmarkOut << l.nX << ", ";
			landmarkOut << l.nY << endl;
			tempLandmark.pop_front();
		}
	}

	ofstream nodeOut("nodepoint.txt");
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

void CMappingApplicationDlg::OnBnClickedButton6()
{
	if (g_navFunc.getNAVMode() == OFF_MODE) {
		g_navFunc.changeOperationMode(STANDBY_MODE);
		m_PowerBtn.SetWindowTextW(_T("PowerDown"));
	}
	else {
		g_navFunc.changeOperationMode(STANDBY_MODE);
		g_navFunc.changeOperationMode(OFF_MODE);
		m_PowerBtn.SetWindowTextW(_T("PowerOn"));
	}
}


void CMappingApplicationDlg::OnBnClickedLandmark()
{
	if (g_navFunc.getNAVMode() == LANDMARK_MODE) {
		MessageBox(_T("�̹� LandMark ��� �Դϴ�."), _T("Mode Change"));
		return;
	}
	//g_mapBuilder.saveImgTemp();
	if(g_navFunc.getNAVMode() != STANDBY_MODE)
		g_navFunc.changeOperationMode(STANDBY_MODE);
	g_navFunc.changeOperationMode(LANDMARK_MODE);
}


void CMappingApplicationDlg::OnBnClickedReflector()
{
	g_navFunc.setReflector(g_robotPos.x, g_robotPos.y, g_robotPos.theta);
}

void CMappingApplicationDlg::OnBnClickedNavigation()
{
	if (g_storedLandmark.size() < 3) {
		if (g_landmark.size() < 3)
			MessageBox(_T("�ݻ���� 3���̻� ���̴� ��ġ�� �μ���"), _T("Mode Change"));
		
		else {
			MessageBox(_T("���� ��ġ�� �ʱ� ��ġ�� �����մϴ�."), _T("Mode Change"));
			list<LandmarkData> temp = g_landmark;
			while (!temp.empty()) {
				LandmarkData landmark = temp.front();
				landmark.nGlobalID = g_storedLandmark.size();
				landmark.nRefType = 2;
				landmark.nSize = 88;
				landmark.nLocalID = g_storedLandmark.size();
				landmark.nLayerID = 1;
				landmark.nLayersID = 0;
				g_storedLandmark.push_back(landmark);
				temp.pop_front();
			}

			g_navFunc.addLandmark(g_storedLandmark.size(), g_storedLandmark, g_robotPos);
			if (g_navFunc.getNAVMode() != STANDBY_MODE)
				g_navFunc.changeOperationMode(STANDBY_MODE);
			g_navFunc.changeOperationMode(NAVIGATION_MODE);
		}
			return;
	}
	if (g_navFunc.getNAVMode() == NAVIGATION_MODE) {
		MessageBox(_T("�̹� MapDrawing ��� �Դϴ�."), _T("Mode Change"));
		return;
	}
	if (g_navFunc.getNAVMode() != STANDBY_MODE)
		g_navFunc.changeOperationMode(STANDBY_MODE);
	g_navFunc.changeOperationMode(NAVIGATION_MODE);
	//g_mapBuilder.getImgTemp();
}


void CMappingApplicationDlg::OnStnClickedPicture()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	RECT rect;
	m_picture.GetWindowRect(&rect);

	int xInImage = (cursorPos.x - rect.left) * SCREEN_W / (rect.right - rect.left);
	int yInImage = (cursorPos.y - rect.top) * SCREEN_H / (rect.bottom - rect.top);
	yInImage = SCREEN_H - yInImage;
	list<LandmarkData> temp = g_landmark;
	for (int i = 0; i < g_landmark.size(); i++) {
		LandmarkData landmark = temp.front();
		int x_pos = (g_robotPos.x + cos(g_robotPos.theta)*landmark.nX - sin(g_robotPos.theta)*landmark.nY) * SCREEN_W / CELL_SIZE / MAP_WIDTH + SCREEN_W / 2;
		int y_pos = (g_robotPos.y + sin(g_robotPos.theta)*landmark.nX + cos(g_robotPos.theta)*landmark.nY) * SCREEN_H / CELL_SIZE / MAP_HEIGHT + SCREEN_H / 2;
		double distance =sqrt( pow((x_pos - xInImage), 2) + pow((y_pos - yInImage), 2));
		if (distance < 5.5)
			if (IDYES == AfxMessageBox(L"�ش� Landmark�� ����Ͻðڽ��ϱ�?", MB_YESNO))
			{
				landmark.nGlobalID = g_storedLandmark.size();
				landmark.nRefType = 2;
				landmark.nSize = 88;
				landmark.nLocalID = g_storedLandmark.size();
				landmark.nLayerID = 1;
				landmark.nLayersID = 0;
				list<LandmarkData> temp;
				temp.push_back(landmark);
				
				if (g_navFunc.addLandmark(1, temp, g_robotPos)) {
					landmark.nX += (g_robotPos.x + cos(g_robotPos.theta)*landmark.nX - sin(g_robotPos.theta)*landmark.nY);
					landmark.nY += (g_robotPos.y + sin(g_robotPos.theta)*landmark.nX + cos(g_robotPos.theta)*landmark.nY);
					g_storedLandmark.push_back(landmark);
					return;
				}

				else
					AfxMessageBox(L"��Ͽ� �����Ͽ����ϴ�");
				return;
			}
			else return;
		temp.pop_front();
	}
	
}


void CMappingApplicationDlg::OnBnClickedReset()
{
	g_navFunc.ResetNAV350();
}
