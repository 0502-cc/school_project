
#include "Camera.h"
#include <QDebug>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <chrono>
#include <mutex>
using namespace dnn;
using namespace std;
using namespace cv;
bool results = false;
bool Cloresults = false;
bool ZX = true;
extern bool plcresult;//plc�����Ƿ�ɹ�
extern bool BiaoDingresult;//�Ƿ���б궨
extern bool Shizhiresult;//�Ƿ������ʾͼ��ʮ��
extern bool RLUDresult;//�Ƿ��ͼ�����ܱ߽��

bool plcSECONd = false;
bool plcstate = false;
bool plcReconnected = true;
bool IfWritePLC = false;
bool plcdisconnected = false;
int ClothCount = 0;//��ʼ���·�����
int plcCLnum = 0;
int plcnum[1] = { 1 };

extern QString srcDirPathOK;
extern QString srcDirPathNG;
extern std::vector<float>XYscale;//�洢xy����������  0-2
extern std::vector<float>XXDvalue; //�洢xy����������  0-2
extern std::vector<float>EXPvalue;//�洢�ع���� 

extern std::vector<float>RLDistance;//�洢ͼ������������
extern std::vector<float>UDDistance;//�洢ͼ������������
extern std::vector<float>Clothes_Distance;//�洢�����·����
extern std::vector<float>Clothes_Area;//�洢�·���������ж�
double getdistance(Point2f pt1, Point2f pt2);//��������֮�����
void ConvexityDefects(vector<vector<Point>>contours);//͹��ȱ��
double calangle_deg(Point2f p1, Point2f p2);//������Ϻ��ֱ����ˮƽ�ߵļн�
void SaveSide(vector<Point>contour);//ɸѡ�������Ϻ�϶̵ĶԱ��еĽϳ���(��б�ǻ�����)
double calculateAngle(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);//���������ڼн�
void calminAreaRect(Mat contours);//������ת��Ӿ��ο�����ĵ���Ƕ�
void saveangle(vector<Point>contour);//�洢�������Ϻ�ļн�
void DirectionConvexityDefects(vector<vector<Point>>contours);//�ж��·�����
void convertpoint(Point2f pt);
void convertangle(double ang);
void depthsort(vector<cv::Point>Direpoints, vector<int> Diredepths);//������Ƚ�������
float calculateIOU(const OutputSeg& box1, const OutputSeg& box2);//�ж����������IOU

std::vector<Point2f>ENDpoint;//�洢���ĵ�
std::vector<double>ENDAngle;//�洢�Ƕ�
std::vector<int>direnum;//�洢����  1--�ҷ�  2--��

std::vector<cv::Point>Direpoints;//�洢�·�����ȱ�������
std::vector<cv::Point>ENDDirepoints;//�洢�·�����ȱ�������
std::vector<int> Diredepths;//�洢͹��ȱ�����
std::vector<cv::Point>Farpoints;//�洢͹��ȱ�������SecFarpoints
std::vector<cv::Point>SecFarpoints;
std::vector<int> depths;//�洢͹��ȱ�����
std::vector<int> Secdepths;//�洢͹��ȱ�����
std::vector<double>Objangle;//�洢������ת��
std::vector<Point2f>ZXpoint;//�洢��ת���ο���ĸ��ǵ�
std::vector<double>ANGLE;//�洢�������Ϻ��ڼн�
std::vector<double>Dist;//�洢�������Ϻ��������
std::vector<int>sortidx;//�洢����

int Maxdis[1];//�洢���γ���
int Mindis[1];//�洢���ζ̱�

int EndMaxdis[1];//�洢���γ���
int EndMindis[1];//�洢���ζ̱�

int FinalpointX[1];//�洢�������ĵ��X����
int FinalpointY[1];//�洢�������ĵ��Y����
int Finalangle[1];//�洢���սǶ�
int Data[12];//�洢һ���Դ������
float showpointX[1];
float showpointY[1];
float showangle[1];

//��̬ѧ�����ߴ��
Mat kernelmm = getStructuringElement(MORPH_RECT, Size(9, 9), Point(-1, -1));
Mat kernelml = getStructuringElement(MORPH_RECT, Size(25, 25), Point(-1, -1));

std::vector<cv::Point>imagecenter;

//��32λ��������ת��2��16λ�ĸߵ�λ����
void SplitInt32toInt16(int32_t input, int16_t& lowpart, int16_t& highpart)
{
	lowpart = static_cast<int16_t>(input & 0xFFFF);   //��λ
	highpart = static_cast<int16_t>((input >> 16) & 0xFFFF);   //��λ
}

float calculateIOU(const OutputSeg& box1, const OutputSeg& box2)
{
	cv::Rect intersection = box1.box & box2.box;
	float intersectionArea = static_cast<float>(intersection.area());
	float unionArea = static_cast<float>(box1.box.area() + box2.box.area() - intersectionArea);
	float iou = intersectionArea / unionArea;
	return iou;
}

void depthsort(vector<cv::Point>Direpoints, vector<int> Diredepths)
{
	int max = 0;
	for (int i = 0; i < Direpoints.size(); i++)
	{
		if (Diredepths[i] > Diredepths[max])
		{
			max = i;
		}
	}
	ENDDirepoints.push_back(Direpoints[max]);
}

//�·������жϸ���
void DirectionConvexityDefects(vector<vector<Point>>contours)
{
	vector<std::vector<Point>> hull(contours.size());//���point���͵�͹��
	vector<std::vector<int>> inthull(contours.size());//���int���͵�͹��
	vector<vector<Vec4i> > hullDefect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
		convexHull(contours[i], inthull[i]);
		convexityDefects(cv::Mat(contours[i]), inthull[i], hullDefect[i]);
	}
	// ��ӡ������͹��ȱ�ݵ�
	for (size_t i = 0; i < hullDefect.size(); i++)
	{
		// ͹��ȱ�ݵ���������
		vector<Vec4i>::iterator iterDefects = hullDefect[i].begin();
		// �����õ�͹��ȱ�ݵ�4�������������л���
		while (iterDefects != hullDefect[i].end())
		{
			Vec4i& v = (*iterDefects);
			// ��ʼλ��
			int startidx = v[0];
			Point ptStart(contours[i][startidx]);
			// ��ֹλ��
			int endidx = v[1];
			Point ptEnd(contours[i][endidx]);
			// ��͹�ǵ���Զ�ĵ�ȱ��
			int faridx = v[2];
			Point ptFar(contours[i][faridx]);
			// ͹��֮�����Զ��
			int depth = v[3] / 256;
			if (depth > 20)//����ȱ����Ƚ���ɸѡ
			{
				Direpoints.push_back(ptFar);
				Diredepths.push_back(depth);
			}
			iterDefects++;
		}
	}
}

//͹��ȱ��
void ConvexityDefects(vector<vector<Point>>contours)
{
	vector<std::vector<Point>> hull(contours.size());//���point���͵�͹��
	vector<std::vector<int>> inthull(contours.size());//���int���͵�͹��
	vector<vector<Vec4i> > hullDefect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
		convexHull(contours[i], inthull[i]);
		convexityDefects(cv::Mat(contours[i]), inthull[i], hullDefect[i]);
	}
	// ��ӡ������͹��ȱ�ݵ�
	for (size_t i = 0; i < hullDefect.size(); i++)
	{
		// ͹��ȱ�ݵ���������
		vector<Vec4i>::iterator iterDefects = hullDefect[i].begin();
		// �����õ�͹��ȱ�ݵ�4�������������л���
		while (iterDefects != hullDefect[i].end())
		{
			Vec4i& v = (*iterDefects);
			// ��ʼλ��
			int startidx = v[0];
			Point ptStart(contours[i][startidx]);
			// ��ֹλ��
			int endidx = v[1];
			Point ptEnd(contours[i][endidx]);
			// ��͹�ǵ���Զ�ĵ�ȱ��
			int faridx = v[2];
			Point ptFar(contours[i][faridx]);
			// ͹��֮�����Զ��
			int depth = v[3] / 256;
			if (depth > 150)//����ȱ����Ƚ���ɸѡ
			{
				//cout << "������Ϊ��" << ptFar.x << "������Ϊ:" << ptFar.y
				//	<< "index��" << faridx << "���:" << depth << endl;
				if (ptFar.y < 200 || ptFar.y>800)
				{
					Farpoints.push_back(ptFar);
				}
				depths.push_back(depth);
			}
			iterDefects++;
		}
	}

}

//͹��ȱ��
void ConvexityDefectsTwo(vector<vector<Point>>contours)
{
	vector<std::vector<Point>> hull(contours.size());//���point���͵�͹��
	vector<std::vector<int>> inthull(contours.size());//���int���͵�͹��
	vector<vector<Vec4i> > hullDefect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
		convexHull(contours[i], inthull[i]);
		convexityDefects(cv::Mat(contours[i]), inthull[i], hullDefect[i]);
	}
	// ��ӡ������͹��ȱ�ݵ�
	for (size_t i = 0; i < hullDefect.size(); i++)
	{
		// ͹��ȱ�ݵ���������
		vector<Vec4i>::iterator iterDefects = hullDefect[i].begin();
		// �����õ�͹��ȱ�ݵ�4�������������л���
		while (iterDefects != hullDefect[i].end())
		{
			Vec4i& v = (*iterDefects);
			// ��ʼλ��
			int startidx = v[0];
			Point ptStart(contours[i][startidx]);
			// ��ֹλ��
			int endidx = v[1];
			Point ptEnd(contours[i][endidx]);
			// ��͹�ǵ���Զ�ĵ�ȱ��
			int faridx = v[2];
			Point ptFar(contours[i][faridx]);
			// ͹��֮�����Զ��
			int depth = v[3] / 256;
			if (depth > 150)//����ȱ����Ƚ���ɸѡ
			{
				//cout << "������Ϊ��" << ptFar.x << "������Ϊ:" << ptFar.y
				//	<< "index��" << faridx << "���:" << depth << endl;
				if (ptFar.y < 200 || ptFar.y>800)
				{
					SecFarpoints.push_back(ptFar);
				}
				Secdepths.push_back(depth);
			}
			iterDefects++;
		}
	}

}

//������Ϻ��ֱ����ˮƽ�ߵļн�
double calangle_deg(Point2f p1, Point2f p2)
{
	if (direnum.empty())
	{
		if ((p1.x >= p2.x && p1.y <= p2.y) || (p1.x <= p2.x && p1.y >= p2.y))
		{
			//cout << p1 << endl;
			//cout << p2 << endl;
			double slope = (p2.y - p1.y) / (p1.x - p2.x);
			double angle_rad = atan(slope);
			double angle_deg = angle_rad * (180.0 / CV_PI);
			return angle_deg;
		}

		if ((p1.x <= p2.x && p1.y <= p2.y) || (p1.x >= p2.x && p1.y >= p2.y))
		{

			double slope = (p2.y - p1.y) / (p2.x - p1.x);
			double angle_rad = atan(slope);
			double angle_deg = angle_rad * 180.0 / CV_PI;
			return 180 - angle_deg;
		}
	}
	if (!direnum.empty())
	{
		if (direnum[0] == 1)
		{
			if ((p1.x >= p2.x && p1.y <= p2.y) || (p1.x <= p2.x && p1.y >= p2.y))
			{
				double slope = (p2.y - p1.y) / (p1.x - p2.x);
				double angle_rad = atan(slope);
				double angle_deg = angle_rad * 180.0 / CV_PI;
				return angle_deg;
			}
			if ((p1.x <= p2.x && p1.y <= p2.y) || (p1.x >= p2.x && p1.y >= p2.y))
			{
				double slope = (p2.y - p1.y) / (p2.x - p1.x);
				double angle_rad = atan(slope);
				double angle_deg = angle_rad * 180.0 / CV_PI;
				return -angle_deg;
			}
		}
		if (direnum[0] == 2)
		{
			if ((p1.x > p2.x && p1.y < p2.y) || (p1.x < p2.x && p1.y > p2.y))
			{
				double slope = (p2.y - p1.y) / (p1.x - p2.x);
				double angle_rad = atan(slope);
				double angle_deg = angle_rad * 180.0 / CV_PI;
				return -(180 - angle_deg);
			}
			if ((p1.x < p2.x && p1.y < p2.y) || (p1.x > p2.x && p1.y > p2.y))
			{
				double slope = (p2.y - p1.y) / (p2.x - p1.x);
				double angle_rad = atan(slope);
				double angle_deg = angle_rad * 180.0 / CV_PI;
				return 180 - angle_deg;
			}
		}
	}

}

//ɸѡ�������Ϻ�϶̵ĶԱ��еĽϳ���(��б�ǻ�����)
void SaveSide(vector<Point>contour)
{
	double dist[10];
	for (int i = 0; i < contour.size(); ++i)
	{
		int prevIdx = (i == 0) ? contour.size() - 1 : i - 1;
		dist[i] = getdistance(contour[prevIdx], contour[i]);
		Dist.push_back(dist[i]);
	}

	for (int i = 0; i < contour.size(); ++i) {
		if (sortidx.empty())
			sortidx.push_back(0);
		else {
			for (auto it = sortidx.begin(); ; it++) {
				if (Dist[i] < Dist[*it]) {
					sortidx.insert(it, i);
					break;
				}
				else if (it == (--sortidx.end())) {
					sortidx.push_back(i);
					break;
				}
			}
		}
	}
}

//��������֮�����
double getdistance(Point2f pt1, Point2f pt2)
{
	return (sqrt(powf(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2)));
}

//����������yֵ֮�������ж�
int calYdistance(vector<Point>contours)
{
	if (contours.empty()) {
		// ��������������
		return 0; // ���߷���һ��ָ����ֵ����ʾ����Ϊ��
	}

	int minY = 5000; // ��ʼ����С y ֵΪ��һ����� y ����ֵ
	int maxY = 0; // ��ʼ����� y ֵΪ��һ����� y ����ֵ

	// �ֶ����������ڵ����е㣬������С����� y ֵ
	for (const Point& point : contours) {
		if (point.y < minY) {
			minY = point.y; // ������С y ֵ
		}
		if (point.y > maxY) {
			maxY = point.y; // ������� y ֵ
		}
	}

	// ������� y ֵ����С y ֵ�Ĳ���
	int yDifference = maxY - minY;
	return yDifference;
}

//���������ڼн�
double calculateAngle(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3)
{
	cv::Point2f v1 = p1 - p2;
	cv::Point2f v2 = p3 - p2;

	double dotProduct = v1.dot(v2);
	double magnitude1 = cv::norm(v1);
	double magnitude2 = cv::norm(v2);

	double cosTheta = dotProduct / (magnitude1 * magnitude2);
	double angleRad = std::acos(cosTheta);
	double angleDeg = angleRad * 180.0 / CV_PI;

	return angleDeg;
}

//������ת��Ӿ��ο�����ĵ���Ƕ�
void calminAreaRect(Mat contours)
{

	RotatedRect boundRect = minAreaRect(contours);
	Point2f pts[4];
	boundRect.points(pts);
	ZXpoint.push_back(pts[0]);
	ZXpoint.push_back(pts[1]);
	ZXpoint.push_back(pts[2]);
	ZXpoint.push_back(pts[3]);

}

//�洢�������Ϻ�ļн�
void saveangle(vector<Point>contour)
{
	double angles[10];
	for (int i = 0; i < contour.size(); ++i)
	{
		int prevIdx = (i == 0) ? contour.size() - 1 : i - 1;
		int nextIdx = (i + 1) % contour.size();
		angles[i] = calculateAngle(contour[prevIdx], contour[i], contour[nextIdx]);
		ANGLE.push_back(angles[i]);
	}
}

void convertpoint(Point2f pt)
{
	// �� x �� y ����ֱ�����λС����ת��Ϊint����
	int xIntValue = static_cast<int>(std::round((pt.x - imagecenter[0].x) * 100.0f) / 100); // ��������int����
	int yIntValue = static_cast<int>(std::round((pt.y - imagecenter[0].y) * 100.0f) / 100);
	float x = round((pt.x - imagecenter[0].x) * 100.0) / 100.0;
	float y = round((pt.y - imagecenter[0].y) * 100.0) / 100.0;
	showpointX[0] = x;
	showpointY[0] = y;
	// ��ת�����intֵ����������
	if (XYscale.empty())
	{
		XYscale.push_back(1.0);
		FinalpointX[0] = xIntValue * XYscale[0];
		FinalpointY[0] = yIntValue * XYscale[0];
	}
	else
	{
		FinalpointX[0] = xIntValue * XYscale[0];
		FinalpointY[0] = yIntValue * XYscale[0];
	}

}

void convertangle(double ang)
{
	// ������ת��Ϊ�޷��Ŷ�����
	int intValue = static_cast<int>(std::round(ang)); // �������뱣����λС��
	float angle = round(ang * 100.0) / 100.0;
	showangle[0] = angle;
	Finalangle[0] = intValue;
}

// �����߳�
static unsigned int __stdcall ConnectThread(void* pUser)   //�̵߳���ں���   ��̬����
{
	// �����ݸ��̵߳Ĳ���ǿ��ת��Ϊ BaslerCamera ָ��
	BaslerCamera* pBaslerCamera = (BaslerCamera*)pUser;
	// ���ת�����ָ���Ƿ�Ϊ NULL
	if (!pBaslerCamera)
	{
		printf("pBaslerCamera is NULL!\n");
		return -1;
	}

	pBaslerCamera->Connect();
	return 0;
}
static unsigned int __stdcall WritePLC(void* pUser)   //�̵߳���ں���   ��̬����
{

	// �����ݸ��̵߳Ĳ���ǿ��ת��Ϊ BaslerCamera ָ��
	BaslerCamera* pBaslerCamera2 = (BaslerCamera*)pUser;
	// ���ת�����ָ���Ƿ�Ϊ NULL
	if (!pBaslerCamera2)
	{
		printf("pBaslerCamera2 is NULL!\n");
		return -1;

	}

	pBaslerCamera2->Write();
	return 0;
}

BaslerCamera::BaslerCamera(void) :m_isExitConnectThread(false)   //m_isExitConnectThread������һ���������͵ı�����������Ϊ falseͨ�������ı������ڿ����̵߳�ִ�л��߱�ʾĳ��״̬
, m_threadHandle(NULL)                                        //m_threadHandle(NULL): ��ʼ������ĳ�Ա���� m_threadHandle
{
	stopState = FALSE;//������ʼ��
	mCamera = NULL;
	CameraRuning = FALSE;
	CameraOpening = FALSE; 
	nodemap = NULL;
	qRegisterMetaType<uint64_t>("uint64_t");
	model_path = "best.onnx";
	if (test.ReadModel(model_path, false, 0, false))
		std::cout << "read net ok!\n";
	else
		std::cout << "read net err!\n";

	// ���������߳�
	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, ConnectThread, this, CREATE_SUSPENDED, NULL);
	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, WritePLC, this, CREATE_SUSPENDED, NULL);
	//_beginthreadex ��һ���̴߳������� �����߳�  NULL����ʾ���߳�Ĭ��ʹ�ø��̵߳İ�ȫ����
	// 0����ʾ�߳�ջ�Ĵ�С��0��ʾʹ��Ĭ��ֵ    ConnectThread�����̵߳���ں��� Ҳ�������̻߳�����������ʼִ��
	//this���Ǵ��ݸ��̵߳Ĳ��� BaslerCamera �����ָ��
	//CREATE_SUSPENDED����ʾ���̴߳������ڹ���״̬ ��Ҫ����ResumeThread�����ŻῪʼִ��
	//NULL���Ǵ��ݸ����̵߳� ID���������Ҫ��ȡ�߳� ID������Դ��� NULL

	if (!m_threadHandle)
	{
		printf("Failed to create display thread!\n");
		Log("Failed to create display thread!\n");
	}
	else
	{
		ResumeThread(m_threadHandle);
		m_isExitConnectThread = false;
	}
}

BaslerCamera::~BaslerCamera(void)
{
	if (mCamera != NULL)
	{
		delete mCamera;
		mCamera = NULL;
		CameraRuning = false;
		CameraOpening = false;
	}

	Pylon::PylonTerminate();   //�ͷ���Դ
}

void BaslerCamera::Write()
{
	try
	{
		while (!m_isExitConnectThread)
		{
			while (!IfWritePLC)
			{
				std::lock_guard<std::mutex> lock(m_mutex); // �Զ�����
				//emit baslerConditionMet();
				if (plc.PLCWrite(0xA8, &plcnum[0], 1, 4036))
				{
					Log("D4036д��ɹ���");
				}
				else
				{
					Log("D4036д��ʧ�ܣ�");
				}
				Sleep(10000);
			}
		}
	}
	catch (const std::exception& ex)
	{
		// �������б�׼�쳣
		Log("�����쳣: " + std::string(ex.what()));
	}
	catch (...)
	{
		// �������������쳣���Ǳ�׼�쳣��
		Log("����δ֪�쳣��");
	}
}

void BaslerCamera::Connect()
{
	while (!m_isExitConnectThread)
	{
		if (!plcdisconnected)
		{
			Sleep(1);
			continue;
		}

		while (plcdisconnected)
		{
			Sleep(5000);
			emit baslerConditionMet();
			if (plc.PLCWrite(0xA8, &plcnum[0], 1, 4036))
			{
				Log("PLC����Ͽ�ʱ,PLC�Ͽ������ɹ�");
				plcresult = true;
				plcdisconnected = false;
			}

		}

	}
}

//�����
bool BaslerCamera::OpenCamera()
{
	if (!CameraOpening)
	{
		// ��ʹ���κ�pylon����֮ǰ�������ʼ��pylon����ʱ
		Pylon::PylonInitialize();

		try
		{
			// Ϊ�����ҵ�������豸����һ����ʱ�������
			if (mCamera != nullptr)
			{
				// �����������Ѿ����ڣ��ر����
				mCamera->Close();
				delete mCamera;
				mCamera = nullptr;
			}

			mCamera = new Pylon::CInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
			Log("------�����������ɹ�-------");
			if (mCamera != NULL)
			{
				nodemap = &mCamera->GetNodeMap();//����������
				// �����
				mCamera->Open();


				std::cout << GenApi::CStringPtr(nodemap->GetNode("DeviceVendorName"))->GetValue() << std::endl;
				if (mCamera && nodemap)
				{
					GenApi::CFloatPtr lineDebouncerTime(mCamera->GetNodeMap().GetNode("LineDebouncerTimeAbs"));
					if (GenApi::IsWritable(lineDebouncerTime))
					{

						if (XXDvalue.empty())
						{
							XXDvalue.push_back(2000);
						}
						lineDebouncerTime->SetValue(XXDvalue[0]);
						Log("��������������ɹ�");
					}

				}

				CameraOpening = true;
				QMessageBox::information(0, "Success", "Open Camera Success!");

			}
		}
		catch (const Pylon::GenericException& e)
		{
			// ������
			QMessageBox::warning(0, "Warning", "Open Camera Error!");
			std::cerr << "Error creating camera: " << e.GetDescription() << std::endl;
			Log("����ԭ��", e.GetDescription());
			// �����뻺������ɾ��ʣ���ַ�
			std::cin.ignore(std::cin.rdbuf()->in_avail());
			// �ر�����������´δ�ʱ�ĳ�ͻ
			if (mCamera)
			{
				mCamera->Close();
				delete mCamera;
				mCamera = nullptr;
			}
			CameraOpening = false;
			return false;
		}
	}
	else
	{
		Log("------����ϴδ��ڴ�ģʽ-------");
	}
	Log("------��������-------");

	//EXPvalue.clear();
	//EXPvalue.push_back(EXPtime);
	return true;

}

//�ر����
void BaslerCamera::CloseCamera()
{
	if (mCamera != NULL)
	{
		stopState = true;
		delete mCamera;
		mCamera = NULL;
		nodemap = NULL;
		CameraRuning = false;
		CameraOpening = false;

	}
	Pylon::PylonTerminate();
}

//��ʼ�ɼ�ͼ��
bool BaslerCamera::StartCamera()
{
	if (CameraOpening && !CameraRuning && mCamera && mCamera->CanWaitForFrameTriggerReady())//����ȴ�����������
	{
		//ͨ������grabLoopType������ʹ��ץȡѭ���߳�����ץȡ
		//��GrabLoop_ProvidedByInstantCamera��ץȡ��������ݸ�ͼ���¼��������
		//ʹ��GrabStrategy_OneByOneĬ��ץȡ����
		mCamera->StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByUser);

		CameraRuning = true;  //��ʼ�ɼ�
		return true;
	}
	else
	{
		return false;
	}

}
// ֹͣ�ɼ�ͼ��
void BaslerCamera::StopCamera()
{
	if (mCamera)
	{
		stopState = true;
		mCamera->StopGrabbing();
		CameraRuning = false;
	}
}

//�궨����
cv::Mat BaslerCamera::Dingwei(cv::Mat& img)
{
	if (XYscale.empty())
	{
		XYscale.push_back(1.000);
	}
	Log("�����������ӣ�", XYscale.size());
	Point center = Point(img.cols / 2, img.rows / 2);
	Mat gray, grayimg;
	cvtColor(img, gray, COLOR_BGR2GRAY);//�ҶȻ�
	GaussianBlur(gray, grayimg, Size(5, 5), 0);//��˹�˲�
	Mat thresh;
	threshold(grayimg, thresh, 200, 255, cv::THRESH_BINARY_INV);//����ֵ��
	erode(thresh, thresh, kernelmm);
	morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//����������С�׵�
	morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//�ղ�������С�ڵ�

	//��һ�β�����������
	vector<vector<Point>>contoursDingwei;
	vector<Vec4i>hierarchyDingwei;
	findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	Mat contour = Mat::zeros(img.size(), CV_8UC1);
	vector<double> g_dConArea(contoursDingwei.size());
	for (int i = 0; i < contoursDingwei.size(); i++)
	{
		g_dConArea[i] = contourArea(contoursDingwei[i]);
	}

	//Ѱ����ͼƬ�������������(Ŀ������)
	int MinIdx = 0;
	int Mindist = 2000;
	for (int i = 0; i < contoursDingwei.size(); i++)
	{
		//cout << g_dConArea[i] << endl;
		if (g_dConArea[i] > 5000 && g_dConArea[i] < 300000)//ɸѡһЩ��������
		{

			Moments m = moments(contoursDingwei[i], true);
			Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
			double a = getdistance(pt_c, center);
			if (Mindist > a)
			{
				Mindist = a;
				MinIdx = i;
			}
		}
	}

	Mat Target = Mat::zeros(img.size(), CV_8UC1);
	drawContours(Target, contoursDingwei, MinIdx, Scalar(255, 255, 255), -1);//����Ŀ������ 
	Mat roi = Mat::zeros(Target.size(), CV_8UC1);
	gray.copyTo(roi, Target);
	Mat dst;
	std::vector<Vec3f> circles;
	HoughCircles(roi, circles, HOUGH_GRADIENT, 1,
		5,  // minDist: Բ�ļ����С����
		100,  // Canny��Ե������ֵ��
		30,   // Բ�ļ����ֵ����С��ֵ��ζ�Ÿ��ϸ�ļ�⣩
		1,   // ��С�뾶
		150); // ���뾶  20

	// ��ͼ���ϻ��Ƽ�⵽��Բ
	for (size_t i = 0; i < circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(img, center, radius, Scalar(0, 0, 255), 2, LINE_AA);
		cout << "��" << i << "��Բ��Բ��X����Ϊ��" << circles[i][0] * XYscale[0] << "," << "Բ��Y����Ϊ��" << circles[i][1] * XYscale[0] << endl;
		std::string text = std::to_string(i + 1);
		cv::putText(img, text, cv::Point(center.x - 20, center.y - 20),
			cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

		std::ostringstream ss;
		ss << "Center " << i + 1 << ": ("
			<< std::fixed << std::setprecision(3) << circles[i][0] * XYscale[0]
			<< ", " << std::fixed << std::setprecision(3) << circles[i][1] * XYscale[0] << ")";
		std::string Text = ss.str();
		cv::putText(img, Text, cv::Point((img.rows - 320), (img.cols - 600 + 30 * i)),
			cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
	}
	return img;
}

bool BaslerCamera::GetFloatPara(const char* nameNode, double& para)
{
	if (nodemap)
	{
		GenApi::CFloatPtr tmp(nodemap->GetNode(nameNode));
		para = tmp->GetMin();
		return TRUE;
	}
	return FALSE;
}

bool BaslerCamera::GetIntPara(const char* nameNode, int64_t& para)
{
	if (nodemap) {
		GenApi::CIntegerPtr tmp(nodemap->GetNode(nameNode));
		para = tmp->GetValue();
		return true;
	}

	return false;
}

bool BaslerCamera::GetStringPara(const char* nameNode, std::string& para)
{
	if (nodemap)
	{
		GenApi::CEnumerationPtr tmp(nodemap->GetNode(nameNode));
		para = tmp->ToString();
		return true;
	}

	return false;
}

bool BaslerCamera::SetFloatPara(const char* nameNode, const double& para)
{
	if (nodemap) {
		GenApi::CFloatPtr tmp(nodemap->GetNode(nameNode));

		if (IsWritable(tmp))
		{
			tmp->SetValue(para);
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool BaslerCamera::SetIntPara(const char* nameNode, INT64 para)
{
	if (nodemap)
	{
		GenApi::CIntegerPtr tmp(nodemap->GetNode(nameNode));
		if (IsWritable(tmp))
		{
			tmp->SetValue(para);
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool BaslerCamera::SetStringPara(const char* nameNode, Pylon::String_t para)
{
	if (nodemap)
	{
		GenApi::CEnumerationPtr tmp(nodemap->GetNode(nameNode));

		if (IsWritable(tmp) && IsAvailable(tmp->GetEntryByName(para)))
		{
			tmp->FromString(para);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

//ִ���������
bool BaslerCamera::SetCmd(const char* nameNode)
{
	if (nodemap)
	{
		GenApi::CCommandPtr tmp(nodemap->GetNode(nameNode));
		if (IsWritable(tmp))
		{
			tmp->Execute();
		}
	}
	return false;
}

//��cv����ͼƬתΪqt��ʽ
QImage BaslerCamera::MatToQImage(const cv::Mat& mat)
{
	// ����mat�����ݴ���QImage
	QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);
	// ���ͼ����BGR��ʽ���������ɫͨ���Ľ���
	if (mat.channels() == 3 && mat.type() == CV_8UC3)
	{
		return image.rgbSwapped();
	}
	return image;
}

//��λ����
cv::Mat BaslerCamera::Detect(cv::Mat& img)
{
	clock_t detectstart = clock(); // ��¼����ʼʱ��
	results = false;
	IfWritePLC = true;
	if (plcresult)
	{
		int Choose = 1;
		if (Choose == 1)
		{
			clock_t TuiListart = clock(); // ��¼����ʼʱ��
			int EndOutput;
			std::vector<OutputSeg> output;
			bool find = test.OnnxDetect(img, output);
			clock_t TuiLifinish = clock();  // ��¼�������ʱ��
			double TuiLitime_length = (double)(TuiLifinish - TuiListart) / CLOCKS_PER_SEC; //��������ʱ�̵Ĳ����������ʱ�� 
			Log("ͼ���������ʱ:", TuiLitime_length, "ms");
			Log("Ŀ�����:", output.size());
			cout << "Ŀ�����:" << output.size() << endl;

			EndOutput = output.size();
			if (output.size() == 1)
			{
				/*cout << output[0].box.area() << endl;*/
			}
			if (output.size() == 2)
			{
				/*	cout << output[0].box.area() << endl;
					cout << output[1].box.area() << endl;*/
				float iou = calculateIOU(output[0], output[1]);
				//cout << iou << endl;
				if (iou > 0.2)
				{
					if (output[0].confidence >= output[1].confidence)
					{
						output[0] = output[0];
					}
					else
					{
						output[0] = output[1];
					}
					EndOutput = output.size() - 1;
				}
			}
			cout << "����Ŀ�����:" << EndOutput << endl;
			Log("����Ŀ�����:", EndOutput);
			Point center = Point(img.cols / 2, img.rows / 2);
			imagecenter.push_back(center);

			if (Shizhiresult)
			{
				line(img, Point(imagecenter[0].x - img.cols / 2, imagecenter[0].y), Point(imagecenter[0].x + img.cols / 2, imagecenter[0].y), Scalar(0, 0, 255), 2);
				line(img, Point(imagecenter[0].x, imagecenter[0].y - img.rows / 2), Point(imagecenter[0].x, imagecenter[0].y + img.rows / 2), Scalar(0, 0, 255), 2);
			}
			if (RLUDresult)
			{
				line(img, Point(RLDistance[0], 0), Point(RLDistance[0], img.rows), Scalar(0, 0, 255), 2);//�������
				line(img, Point(img.cols - RLDistance[0], 0), Point(img.cols - RLDistance[0], img.rows), Scalar(0, 0, 255), 2);//�ұ�����
				line(img, Point(0, UDDistance[0]), Point(img.cols, UDDistance[0]), Scalar(0, 0, 255), 2);//�ϱߺ���
				line(img, Point(0, img.rows - UDDistance[0]), Point(img.cols, img.rows - UDDistance[0]), Scalar(0, 0, 255), 2);//�ϱߺ���
			}
			if (EndOutput == 1 || EndOutput == 2)
			{
				if (EndOutput == 1)
				{
			/*		cout << "22332" << endl;*/
					//cv::rectangle(img, output[0].box, cv::Scalar(255, 250, 0), 3);
					Mat gray, grayimg;
					cvtColor(img, gray, COLOR_BGR2GRAY);//�ҶȻ�
					GaussianBlur(gray, grayimg, Size(5, 5), 0);//��˹�˲�
					Mat thresh;
					threshold(grayimg, thresh, 50, 255, cv::THRESH_BINARY_INV + THRESH_OTSU);//����ֵ��
					erode(thresh, thresh, kernelml);
					morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//����������С�׵�
					morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//�ղ�������С�ڵ�

					//��һ�β�����������
					vector<vector<Point>>contoursDingwei;
					vector<Vec4i>hierarchyDingwei;
					findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

					Mat contour = Mat::zeros(img.size(), CV_8UC1);
					vector<double> g_dConArea(contoursDingwei.size());
					for (int i = 0; i < contoursDingwei.size(); i++)
					{
						g_dConArea[i] = contourArea(contoursDingwei[i]);
					}

					//Ѱ����ͼƬ�������������(Ŀ������)
					int MinIdx = 0;
					int Mindist = 2000;
					for (int i = 0; i < contoursDingwei.size(); i++)
					{
						int Ydis = calYdistance(contoursDingwei[i]);
						//cout << g_dConArea[i] << endl;
	/*					cout << "Ydis" << Ydis << endl;*/
						if (g_dConArea[i] > 60000 && Ydis < 1100)//ɸѡһЩ�������� 100000 900
						{
	
							Moments m = moments(contoursDingwei[i], true);
							Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
							double a = getdistance(pt_c, center);
							if (RLDistance.empty())
							{
								RLDistance.push_back(100);
							}
							if (UDDistance.empty())
							{
								UDDistance.push_back(100);
							}

							if (pt_c.x > RLDistance[0] && pt_c.x < img.cols - RLDistance[0] && pt_c.y>UDDistance[0] && pt_c.y < img.rows - UDDistance[0])//�����������ĵ�����ж�NG���
							{
								/*cout << "2232" << endl;*/
								if (Mindist > a)
								{
									Mindist = a;
									MinIdx = i;
								}
								results = true;
							}
						}
					}

					if (results)
					{

						Mat Target = Mat::zeros(img.size(), CV_8UC1);
						drawContours(Target, contoursDingwei, MinIdx, Scalar(255), -1);//����Ŀ������
						//imshow("Target", Target);
						//waitKey(0);
						vector<vector<Point>>contoursTubao;
						vector<Vec4i>hierarchyTubao;
						findContours(Target, contoursTubao, hierarchyTubao, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
						ConvexityDefects(contoursTubao);//��Ŀ����������͹��ȱ�ݼ��   ����·��봫�ʹ���ճ�����
						Log("ȱ�ݵ����:", Farpoints.size());
						//cout << "ȱ�ݵ����:" << Farpoints.size() << endl;
						//circle(img, Farpoints[0], 4, Scalar(0, 0, 255), 4);
						//����·��봫�ʹ���ճ�����
						if (Farpoints.size() == 1 || Farpoints.size() == 2)
						{

							if (Farpoints.size() == 1)
							{
								line(Target, Point(0, Farpoints[0].y), Point(img.cols, Farpoints[0].y), Scalar(0, 0, 0), 5);
							}

							else if (Farpoints.size() == 2)
							{
								if (abs(Farpoints[0].y - Farpoints[1].y) < 500)
								{
									line(Target, Farpoints[0], Farpoints[1], Scalar(0, 0, 0), 5);
								}
								else
								{
									line(Target, Point(0, Farpoints[0].y), Point(img.cols, Farpoints[0].y), Scalar(0, 0, 0), 5);
									line(Target, Point(0, Farpoints[1].y), Point(img.cols, Farpoints[1].y), Scalar(0, 0, 0), 5);
								}
								
							}

							//��Ŀ���������в��� ������ճ�������
							vector<vector<Point>>Endcontours;//Ѱ�Ҵ�����Ŀ������
							vector<Vec4i>Endhierarchy;
							findContours(Target, Endcontours, Endhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

							int Idx = 0;
							double dist = 2000;
							for (int i = 0; i < Endcontours.size(); i++)
							{
								Moments m = moments(Endcontours[i], true);
								Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
								double a = getdistance(pt_c, center);
								if (dist > a)
								{
									dist = a;
									Idx = i;
								}
							}

							Mat ENDTarget = Mat::zeros(img.size(), CV_8UC1);
							drawContours(ENDTarget, Endcontours, Idx, Scalar(255), -1);
							//imshow("ENDTarget", ENDTarget);
							//waitKey(0);
							if (contourArea(Endcontours[Idx]) > 350000)
							{
				
								vector<vector<Point>>Secondcontours;
								vector<Vec4i>Secondhierarchy;
								findContours(ENDTarget, Secondcontours, Secondhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
								ConvexityDefectsTwo(Secondcontours);//��Ŀ����������͹��ȱ�ݼ��   ����·��봫�ʹ���ճ�����
								if (SecFarpoints.size() == 1 || SecFarpoints.size() == 2)
								{

									if (SecFarpoints.size() == 1)
									{
										line(ENDTarget, Point(0, SecFarpoints[0].y), Point(img.cols, SecFarpoints[0].y), Scalar(0, 0, 0), 5);
									}

									else if (SecFarpoints.size() == 2)
									{
										if (abs(SecFarpoints[0].y - SecFarpoints[1].y) < 500)
										{
											line(ENDTarget, SecFarpoints[0], SecFarpoints[1], Scalar(0, 0, 0), 5);
										}
										else
										{
											line(ENDTarget, Point(0, SecFarpoints[0].y), Point(img.cols, SecFarpoints[0].y), Scalar(0, 0, 0), 5);
											line(ENDTarget, Point(0, SecFarpoints[1].y), Point(img.cols, SecFarpoints[1].y), Scalar(0, 0, 0), 5);
										}

									}
								}

								//��Ŀ���������в��� ������ճ�������
								vector<vector<Point>>SecEndcontours;//Ѱ�Ҵ�����Ŀ������
								vector<Vec4i>SecEndhierarchy;
								findContours(ENDTarget, SecEndcontours, SecEndhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

								int Idx = 0;
								double dist = 2000;
								for (int i = 0; i < SecEndcontours.size(); i++)
								{
									Moments m = moments(SecEndcontours[i], true);
									Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
									double a = getdistance(pt_c, center);
									if (dist > a)
									{
										dist = a;
										Idx = i;
									}
								}

								Mat SecENDTarget = Mat::zeros(img.size(), CV_8UC1);
								drawContours(SecENDTarget, SecEndcontours, Idx, Scalar(255), -1);
								if (contourArea(SecEndcontours[Idx]) < Clothes_Area[0])//220000  250000
								{

									vector<vector<Point>>contoursENDMoban;
									vector<Vec4i>hierarchyENDMoban;
									findContours(SecENDTarget, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

									if (output[0].id == 1)
									{
										DirectionConvexityDefects(contoursENDMoban);
									}

									//��Ŀ����������͹������
									vector<vector<Point>>hull(contoursENDMoban.size());
									Mat dst = Mat::zeros(img.size(), CV_8UC1);
									for (int i = 0; i < contoursENDMoban.size(); i++)
									{
										convexHull(contoursENDMoban[i], hull[i], false, true);
										//����͹��
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}


									//�������Ͻ���ģ��ƥ�����
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 200, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);
									saveangle(outputcurve);//�洢�ڼн�
									//SaveSide(outputcurve);//�ҵ���׼��
	
									if (outputcurve.size() >= 4)
									{
										int num = 0;
										for (int i = 0; i < ANGLE.size(); i++)
										{
											if (ANGLE[i] < 60)
											{
												num++;
											}
										}
										num = 0;
										if (num == 0)
										{
											calminAreaRect(Mat(contoursENDMoban[0]));
											Moments m = moments(contoursENDMoban[0], true);
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
											double maxdis = 0;
											double mindis = 1000;
											for (int j = 0; j < 4; j++)
											{
												line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
												double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
												if (longdis > maxdis)
												{
													maxdis = longdis;
												}
												if (longdis < mindis)
												{
													mindis = longdis;
												}
											}
											Maxdis[0] = maxdis;

											Mindis[0] = mindis;
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
											EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
											ENDpoint.push_back(center);
											circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
											circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

											if (output[0].id == 1)
											{
												if (Direpoints.size() == 0)
												{
												}
												else
												{
													depthsort(Direpoints, Diredepths);
													if (ENDDirepoints[0].x >= ENDpoint[0].x)
													{
														direnum.push_back(1);
													}
													else
													{
														direnum.push_back(2);
													}
												}
											}
											double maxDistance = 0;
											int index1 = -1, index2 = -1;

											for (int i = 0; i < ZXpoint.size(); ++i)
											{
												int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

												double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

												if (distance > maxDistance)
												{
													maxDistance = distance;
													index1 = i;
													index2 = nextIndex;
												}
											}
											/*								line(img, ZXpoint[index1], ZXpoint[index2], Scalar(0, 0, 255), 4);*/
											double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
											sortidx.clear();
											ENDAngle.push_back(ang);
											results = true;
											std::ostringstream ss;
											ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
												<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
												<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
											std::string Text = ss.str();
											cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
										}
										else
										{
											results = false;
										}
									}
									else
									{
										results = false;
									}
								}
								else
								{
									results = false;
								}
							}

							else
							{
								if (contourArea(Endcontours[Idx]) < Clothes_Area[0])//220000  250000
								{

									vector<vector<Point>>contoursENDMoban;
									vector<Vec4i>hierarchyENDMoban;
									findContours(ENDTarget, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	
									if (output[0].id == 1)
									{
										DirectionConvexityDefects(contoursENDMoban);
									}

									//��Ŀ����������͹������
									vector<vector<Point>>hull(contoursENDMoban.size());
									Mat dst = Mat::zeros(img.size(), CV_8UC1);
									for (int i = 0; i < contoursENDMoban.size(); i++)
									{
										convexHull(contoursENDMoban[i], hull[i], false, true);
										//����͹��
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}


									//�������Ͻ���ģ��ƥ�����
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 100, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);


									saveangle(outputcurve);//�洢�ڼн�
									//SaveSide(outputcurve);//�ҵ���׼��

									//cout << "outputcurve" << outputcurve.size() << endl;
									if (outputcurve.size() >= 4)
									{
										int num = 0;
										for (int i = 0; i < ANGLE.size(); i++)
										{
											if (ANGLE[i] < 60)
											{
												num++;
											}
										}
										num = 0;
										if (num == 0)
										{
											calminAreaRect(Mat(contoursENDMoban[0]));
											Moments m = moments(contoursENDMoban[0], true);
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
											double maxdis = 0;
											double mindis = 1000;
											for (int j = 0; j < 4; j++)
											{
												line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
												double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
												if (longdis > maxdis)
												{
													maxdis = longdis;
												}
												if (longdis < mindis)
												{
													mindis = longdis;
												}
											}
											Maxdis[0] = maxdis;

											Mindis[0] = mindis;
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
											EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
											ENDpoint.push_back(center);
											circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
											circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

											if (output[0].id == 1)
											{
												if (Direpoints.size() == 0)
												{
												}
												else
												{
													depthsort(Direpoints, Diredepths);
													if (ENDDirepoints[0].x >= ENDpoint[0].x)
													{
														direnum.push_back(1);
													}
													else
													{
														direnum.push_back(2);
													}
												}
											}
											double maxDistance = 0;
											int index1 = -1, index2 = -1;

											for (int i = 0; i < ZXpoint.size(); ++i)
											{
												int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

												double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

												if (distance > maxDistance)
												{
													maxDistance = distance;
													index1 = i;
													index2 = nextIndex;
												}
											}
											/*								line(img, ZXpoint[index1], ZXpoint[index2], Scalar(0, 0, 255), 4);*/
											double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
											sortidx.clear();
											ENDAngle.push_back(ang);
											results = true;
											std::ostringstream ss;
											ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
												<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
												<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
											std::string Text = ss.str();
											cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
										}
										else
										{
											results = false;
										}
									}
									else
									{
										results = false;
									}
								}
								else
								{
									results = false;
								}
							}

						}

						//����·��պô��ڴ��ʹ�����
						else if (Farpoints.size() == 0)
						{
	
							if (contourArea(contoursDingwei[MinIdx]) < Clothes_Area[0])   //�����б��Ƿ���������·�  220000  250000
							{
				
								vector<vector<Point>>contoursENDMoban;
								vector<Vec4i>hierarchyENDMoban;
								findContours(Target, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

								if (output[0].id == 1)
								{
									DirectionConvexityDefects(contoursENDMoban);
								}

								vector<vector<Point>>hull(contoursENDMoban.size());
								Mat dst = Mat::zeros(img.size(), CV_8UC1);
								for (int i = 0; i < contoursENDMoban.size(); i++)
								{
									convexHull(contoursENDMoban[i], hull[i], false, true);
									//����͹��
									drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
								}

								//�������Ͻ���ģ��ƥ�����
								vector<Point> inputcurve = hull[0];
								vector<Point>outputcurve;
								approxPolyDP(inputcurve, outputcurve, 100, true);

								Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
								drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);

								saveangle(outputcurve);//�洢�ڼн�
								//SaveSide(outputcurve);//�ҵ���׼��

								if (outputcurve.size() >= 4)
								{
									int num = 0;
									for (int i = 0; i < ANGLE.size(); i++)
									{
										if (ANGLE[i] < 90)
										{
											num++;
										}
									}
									num = 0;
									if (num == 0)
									{
					
										calminAreaRect(Mat(contoursENDMoban[0]));
										Moments m = moments(contoursENDMoban[0], true);
										Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
										double maxdis = 0;
										double mindis = 1000;
										for (int j = 0; j < 4; j++)
										{
											line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
											double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
											if (longdis > maxdis)
											{
												maxdis = longdis;
											}
											if (longdis < mindis)
											{
												mindis = longdis;
											}
										}
										Maxdis[0] = maxdis;
										Mindis[0] = mindis;
										EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
										EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
										ENDpoint.push_back(center);
										circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
										circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

										if (output[0].id == 1)
										{
											if (Direpoints.size() == 0)
											{

											}
											else
											{
												depthsort(Direpoints, Diredepths);

												if (ENDDirepoints[0].x >= ENDpoint[0].x)
												{
													direnum.push_back(1);
												}
												else
												{
													direnum.push_back(2);
												}
											}
										}
										double maxDistance = 0;
										int index1 = -1, index2 = -1;
										for (int i = 0; i < ZXpoint.size(); ++i)
										{
											int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

											double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

											if (distance > maxDistance)
											{
												maxDistance = distance;
												index1 = i;
												index2 = nextIndex;
											}
										}
										/*					line(img, ZXpoint[index1], ZXpoint[index2], Scalar(0, 0, 255), 4);*/
										double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
										sortidx.clear();
										ENDAngle.push_back(ang);
										results = true;
										std::ostringstream ss;
										ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
											<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
											<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
										std::string Text = ss.str();
										cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
									}
									else
									{
										results = false;
									}
								}
								else
								{
									results = false;
								}
							}
							else
							{
								results = false;
							}
						}
						else
						{
							results = false;
						}
					}
					SecFarpoints.clear();
					ZXpoint.clear();
					Farpoints.clear();
					ANGLE.clear();
					Dist.clear();
					imagecenter.clear();
				}

				if (EndOutput == 2)
				{

					Cloresults = false;
					int X1 = output[0].box.x;
					int X2 = output[0].box.x + output[0].box.width;
					int X3 = output[1].box.x;
					int X4 = output[1].box.x + output[1].box.width;
					//cv::rectangle(img, output[0].box, cv::Scalar(255, 250, 0), 3);
					//cv::rectangle(img, output[1].box, cv::Scalar(255, 250, 0), 3);

					//cout << "x1:" << X1 << endl;
					//cout << "x2:" << X2 << endl;
					//cout << "x3:" << X3 << endl;
					//cout << "x4:" << X4 << endl;
					//cout << output[0].box.width << endl;
					//cout << output[0].box.height << endl;
					//cout << output[1].box.width << endl;
					//cout << output[1].box.height << endl;

					if ((X1 < X3 && X2 < X3 && X3 - X2 >= Clothes_Distance[0]) || (X3 < X1 && X4 < X1 && X1 - X4 >= Clothes_Distance[0]))
					{
						Cloresults = true;
					}
					if (Cloresults == false)
					{
						if ((abs(output[0].box.width - output[0].box.height) > 150 || abs(output[1].box.width - output[1].box.height) > 150) && (output[0].box.area() < 100000 || output[1].box.area() < 100000))
						{
							Cloresults = true;
						}
						//float iou = calculateIOU(output[0], output[1]);
						//if (iou < 0.1)
						//{
						//	Cloresults = true;
						//}
					}
					/*			cout << "���������·�" << endl;*/
					int area1 = output[0].box.area();
					int area2 = output[1].box.area();

					if (((area1 > 60000 && area1 < 600000) || (area2 > 60000 && area2 < 600000)) && Cloresults)  //400000 100000
					{

						Mat gray, grayimg;
						cvtColor(img, gray, COLOR_BGR2GRAY);//�ҶȻ�
						GaussianBlur(gray, grayimg, Size(5, 5), 0);//��˹�˲�
						Mat thresh;
						threshold(grayimg, thresh, 50, 255, cv::THRESH_BINARY_INV + THRESH_OTSU);//����ֵ��
						erode(thresh, thresh, kernelml);
						morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//����������С�׵�
						morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//�ղ�������С�ڵ�
						//��һ�β�����������
						vector<vector<Point>>contoursDingwei;
						vector<Vec4i>hierarchyDingwei;
						findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
						Mat contour = Mat::zeros(img.size(), CV_8UC1);
						vector<double> g_dConArea(contoursDingwei.size());
						for (int i = 0; i < contoursDingwei.size(); i++)
						{
							g_dConArea[i] = contourArea(contoursDingwei[i]);
						}
						//Ѱ����ͼƬ�������������(Ŀ������)
						int MinIdx = 0;
						int Mindist = 2000;
						for (int i = 0; i < contoursDingwei.size(); i++)
						{
							int Ydis = calYdistance(contoursDingwei[i]);
							if (g_dConArea[i] > 60000 && Ydis < 900)//ɸѡһЩ�������� 100000
							{

								Moments m = moments(contoursDingwei[i], true);
								Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
								double a = getdistance(pt_c, center);
								if (RLDistance.empty())
								{
									RLDistance.push_back(100);
								}
								if (UDDistance.empty())
								{
									UDDistance.push_back(100);
								}

								if (pt_c.x > RLDistance[0] && pt_c.x < img.cols - RLDistance[0] && pt_c.y>UDDistance[0] && pt_c.y < img.rows - UDDistance[0])//�����������ĵ�����ж�NG���
								{
									if (Mindist > a)
									{
										Mindist = a;
										MinIdx = i;
									}
									results = true;
								}
							}
						}

						if (results)
						{
					/*		cout << "111" << endl;*/
							Mat Target = Mat::zeros(img.size(), CV_8UC1);
							drawContours(Target, contoursDingwei, MinIdx, Scalar(255), -1);//����Ŀ������
							vector<vector<Point>>contoursTubao;
							vector<Vec4i>hierarchyTubao;
							findContours(Target, contoursTubao, hierarchyTubao, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
							ConvexityDefects(contoursTubao);//��Ŀ����������͹��ȱ�ݼ��   ����·��봫�ʹ���ճ�����

							/*cout <<"ȱ�ݵ������" << Farpoints.size() << endl;*/
							//����·��봫�ʹ���ճ�����
							if (Farpoints.size() == 1 || Farpoints.size() == 2)
							{
								if (Farpoints.size() == 1)
								{
									line(Target, Point(0, Farpoints[0].y), Point(img.cols, Farpoints[0].y), Scalar(0, 0, 0), 5);
								}
								else if (Farpoints.size() == 2)
								{
									if (abs(Farpoints[0].y - Farpoints[1].y) < 500)
									{
										line(Target, Farpoints[0], Farpoints[1], Scalar(0, 0, 0), 5);
									}
									else
									{
										line(Target, Point(0, Farpoints[0].y), Point(img.cols, Farpoints[0].y), Scalar(0, 0, 0), 5);
										line(Target, Point(0, Farpoints[1].y), Point(img.cols, Farpoints[1].y), Scalar(0, 0, 0), 5);
									}

								}
								//��Ŀ���������в��� ������ճ�������
								vector<vector<Point>>Endcontours;//Ѱ�Ҵ�����Ŀ������
								vector<Vec4i>Endhierarchy;
								findContours(Target, Endcontours, Endhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
								int Idx = 0;
								double dist = 2000;
								for (int i = 0; i < Endcontours.size(); i++)
								{
									//RotatedRect boundRect = minAreaRect(Mat(Endcontours[i]));
									//Point2f pts[4];
									//boundRect.points(pts);
									//Point2f pt_c = (pts[0] + pts[1] + pts[2] + pts[3]) / 4;
									Moments m = moments(Endcontours[i], true);
									Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
									double a = getdistance(pt_c, center);
									if (dist > a)
									{
										dist = a;
										Idx = i;
									}
								}
								Mat ENDTarget = Mat::zeros(img.size(), CV_8UC1);
								drawContours(ENDTarget, Endcontours, Idx, Scalar(255), -1);

								vector<vector<Point>>contoursENDMoban;
								vector<Vec4i>hierarchyENDMoban;
								findContours(ENDTarget, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
								//cout <<"����Ŀ�����������С��" << contourArea(Endcontours[Idx]) << endl;
								if (contourArea(Endcontours[Idx]) < Clothes_Area[0])//220000  250000
								{
									if (area1 > area2 && output[0].id == 1)
									{

										DirectionConvexityDefects(contoursENDMoban);
									}
									else if (area1 < area2 && output[1].id == 1)
									{

										DirectionConvexityDefects(contoursENDMoban);
									}
									//��Ŀ����������͹������
									vector<vector<Point>>hull(contoursENDMoban.size());
									Mat dst = Mat::zeros(img.size(), CV_8UC1);
									for (int i = 0; i < contoursENDMoban.size(); i++)
									{
										convexHull(contoursENDMoban[i], hull[i], false, true);
										//����͹��
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}
									//�������Ͻ���ģ��ƥ�����
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 100, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);
									saveangle(outputcurve);//�洢�ڼн�
									//SaveSide(outputcurve);//�ҵ���׼��
						/*			cout << "�������Ϻ������" << outputcurve.size() << endl;*/
									if (outputcurve.size() >= 4)
									{
										int num = 0;
										for (int i = 0; i < ANGLE.size(); i++)
										{
											if (ANGLE[i] < 60)
											{
												num++;
											}
										}
										/*cout << "�Ƕ�num��" << num << endl;*/
										num = 0;
										if (num == 0)
										{

											calminAreaRect(Mat(contoursENDMoban[0]));
											Moments m = moments(contoursENDMoban[0], true);
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
											double maxdis = 0;
											double mindis = 1000;
											for (int j = 0; j < 4; j++)
											{
												line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
												double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
												if (longdis > maxdis)
												{
													maxdis = longdis;
												}
												if (longdis < mindis)
												{
													mindis = longdis;
												}
											}
											Maxdis[0] = maxdis;
											Mindis[0] = mindis;
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
											EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
											ENDpoint.push_back(center);
											circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
											circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

											if (area1 > area2 && output[0].id == 1)
											{
												if (Direpoints.size() == 0)
												{
												}
												else
												{
													depthsort(Direpoints, Diredepths);
													if (ENDDirepoints[0].x >= ENDpoint[0].x)
													{
														direnum.push_back(1);
													}
													else
													{
														direnum.push_back(2);
													}
												}
											}
											else if (area1 < area2 && output[1].id == 1)
											{
												if (Direpoints.size() == 0)
												{
												}
												else
												{
													depthsort(Direpoints, Diredepths);
													if (ENDDirepoints[0].x >= ENDpoint[0].x)
													{
														direnum.push_back(1);
													}
													else
													{
														direnum.push_back(2);
													}
												}
											}
											double maxDistance = 0;
											int index1 = -1, index2 = -1;

											for (int i = 0; i < ZXpoint.size(); ++i)
											{
												int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

												double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

												if (distance > maxDistance)
												{
													maxDistance = distance;
													index1 = i;
													index2 = nextIndex;
												}
											}

											double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
											//double ang = calangle_deg(outputcurve[sortidx[2]], outputcurve[(sortidx[2] - 1 + outputcurve.size()) % outputcurve.size()]);
											sortidx.clear();
											ENDAngle.push_back(ang);
											results = true;
											std::ostringstream ss;
											ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
												<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
												<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
											std::string Text = ss.str();
											cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
										}
										else
										{
											results = false;
										}
									}
									else
									{
										results = false;
									}
								}
								else
								{
									results = false;
								}
							}
							//����·��պô��ڴ��ʹ�����
							else if (Farpoints.size() == 0)
							{
								//cout << "�·������м�λ��ʱ���������������" << contourArea(contoursDingwei[MinIdx]) << endl;
								if (contourArea(contoursDingwei[MinIdx]) < Clothes_Area[0]) //220000  250000
								{
									vector<vector<Point>>contoursENDMoban;
									vector<Vec4i>hierarchyENDMoban;
									findContours(Target, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
									if (area1 > area2 && output[0].id == 1)
									{
										DirectionConvexityDefects(contoursENDMoban);
									}
									else if (area1 < area2 && output[1].id == 1)
									{
										DirectionConvexityDefects(contoursENDMoban);
									}
									vector<vector<Point>>hull(contoursENDMoban.size());
									Mat dst = Mat::zeros(img.size(), CV_8UC1);
									for (int i = 0; i < contoursENDMoban.size(); i++)
									{
										convexHull(contoursENDMoban[i], hull[i], false, true);
										//����͹��
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}
									//�������Ͻ���ģ��ƥ�����
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 100, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);
									saveangle(outputcurve);//�洢�ڼн�
									//SaveSide(outputcurve);//�ҵ���׼��
									/*cout << "�·������м�λ��ʱ,�������ϱ�����" << outputcurve.size() << endl;*/
									if (outputcurve.size() >= 4)
									{
										int num = 0;
										for (int i = 0; i < ANGLE.size(); i++)
										{
											if (ANGLE[i] < 60)
											{
												num++;
											}
										}
										/*cout << "�·������м�λ��ʱ,�Ƕ���Ŀ��" << num << endl;*/
										num = 0;
										if (num == 0)
										{

											calminAreaRect(Mat(contoursENDMoban[0]));
											Moments m = moments(contoursENDMoban[0], true);
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
											double maxdis = 0;
											double mindis = 1000;
											for (int j = 0; j < 4; j++)
											{
												line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
												double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
												if (longdis > maxdis)
												{
													maxdis = longdis;
												}
												if (longdis < mindis)
												{
													mindis = longdis;
												}
											}
											Maxdis[0] = maxdis;
											Mindis[0] = mindis;
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
											EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
											ENDpoint.push_back(center);
											circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
											circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

											if (area1 > area2 && output[0].id == 1)
											{
												if (Direpoints.size() == 0)
												{
												}
												else
												{
													depthsort(Direpoints, Diredepths);
													if (ENDDirepoints[0].x >= ENDpoint[0].x)
													{
														direnum.push_back(1);
													}
													else
													{
														direnum.push_back(2);
													}
												}
											}
											else if (area1 < area2 && output[1].id == 1)
											{
												if (Direpoints.size() == 0)
												{
												}
												else
												{
													depthsort(Direpoints, Diredepths);
													if (ENDDirepoints[0].x >= ENDpoint[0].x)
													{
														direnum.push_back(1);
													}
													else
													{
														direnum.push_back(2);
													}
												}
											}
											double maxDistance = 0;
											int index1 = -1, index2 = -1;

											for (int i = 0; i < ZXpoint.size(); ++i)
											{
												int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

												double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

												if (distance > maxDistance)
												{
													maxDistance = distance;
													index1 = i;
													index2 = nextIndex;
												}
											}

											double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
											//double ang = calangle_deg(outputcurve[sortidx[2]], outputcurve[(sortidx[2] - 1 + outputcurve.size()) % outputcurve.size()]);
											sortidx.clear();
											ENDAngle.push_back(ang);
											results = true;
											std::ostringstream ss;
											ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
												<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
												<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
											std::string Text = ss.str();
											cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
										}
										else
										{
											results = false;
										}
									}
									else
									{
										results = false;
									}
								}
								else
								{
									results = false;
								}
							}
							else
							{
								results = false;
							}
						}

						ZXpoint.clear();
						Farpoints.clear();
						ANGLE.clear();
						Dist.clear();
						ENDDirepoints.clear();
						Diredepths.clear();
						Direpoints.clear();
						imagecenter.clear();
					}
					else
					{
						results = false;
					}
				}

			}

			else if (EndOutput == 0)
			{
				Mat gray, grayimg;
				cvtColor(img, gray, COLOR_BGR2GRAY);//�ҶȻ�
				GaussianBlur(gray, grayimg, Size(5, 5), 0);//��˹�˲�
				Mat thresh;
				threshold(grayimg, thresh, 50, 255, cv::THRESH_BINARY_INV + THRESH_OTSU);//����ֵ��
				erode(thresh, thresh, kernelml);
				morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//����������С�׵�
				morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//�ղ�������С�ڵ�

				//��һ�β�����������
				vector<vector<Point>>contoursDingwei;
				vector<Vec4i>hierarchyDingwei;
				findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

				Mat contour = Mat::zeros(img.size(), CV_8UC1);
				vector<double> g_dConArea(contoursDingwei.size());
				for (int i = 0; i < contoursDingwei.size(); i++)
				{
					g_dConArea[i] = contourArea(contoursDingwei[i]);
				}

				//Ѱ����ͼƬ�������������(Ŀ������)
				int MinIdx = 0;
				int Mindist = 2000;
				for (int i = 0; i < contoursDingwei.size(); i++)
				{
					int Ydis = calYdistance(contoursDingwei[i]);
					if (g_dConArea[i] > 60000 && Ydis < 900)//ɸѡһЩ�������� 100000
					{

						Moments m = moments(contoursDingwei[i], true);
						Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
						double a = getdistance(pt_c, center);
						if (RLDistance.empty())
						{
							RLDistance.push_back(100);
						}
						if (UDDistance.empty())
						{
							UDDistance.push_back(100);
						}

						if (pt_c.x > RLDistance[0] && pt_c.x < img.cols - RLDistance[0] && pt_c.y>UDDistance[0] && pt_c.y < img.rows - UDDistance[0])//�����������ĵ�����ж�NG���
						{
							if (Mindist > a)
							{
								Mindist = a;
								MinIdx = i;
							}
							results = true;
						}
					}
				}

				if (results)
				{
					//cout << "���봦������" << endl;
					Mat Target = Mat::zeros(img.size(), CV_8UC1);
					drawContours(Target, contoursDingwei, MinIdx, Scalar(255), -1);//����Ŀ������

					vector<vector<Point>>contoursTubao;
					vector<Vec4i>hierarchyTubao;
					findContours(Target, contoursTubao, hierarchyTubao, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
					ConvexityDefects(contoursTubao);//��Ŀ����������͹��ȱ�ݼ��   ����·��봫�ʹ���ճ�����
					Log("ȱ�ݵ����:", Farpoints.size());
					//circle(img, Farpoints[0], 4, Scalar(0, 0, 255), 4);
					//����·��봫�ʹ���ճ�����
					if (Farpoints.size() == 1 || Farpoints.size() == 2)
					{

						if (Farpoints.size() == 1)
						{
							line(Target, Point(0, Farpoints[0].y), Point(img.cols, Farpoints[0].y), Scalar(0, 0, 0), 5);
						}

						else if (Farpoints.size() == 2)
						{
							if (abs(Farpoints[0].y - Farpoints[1].y) < 500)
							{
								line(Target, Farpoints[0], Farpoints[1], Scalar(0, 0, 0), 5);
							}
							else
							{
								line(Target, Point(0, Farpoints[0].y), Point(img.cols, Farpoints[0].y), Scalar(0, 0, 0), 5);
								line(Target, Point(0, Farpoints[1].y), Point(img.cols, Farpoints[1].y), Scalar(0, 0, 0), 5);
							}

						}

						//��Ŀ���������в��� ������ճ�������
						vector<vector<Point>>Endcontours;//Ѱ�Ҵ�����Ŀ������
						vector<Vec4i>Endhierarchy;
						findContours(Target, Endcontours, Endhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

						int Idx = 0;
						double dist = 2000;
						for (int i = 0; i < Endcontours.size(); i++)
						{
							
							Moments m = moments(Endcontours[i], true);
							Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
							double a = getdistance(pt_c, center);
							if (dist > a)
							{
								dist = a;
								Idx = i;
							}
						}

						Mat ENDTarget = Mat::zeros(img.size(), CV_8UC1);
						drawContours(ENDTarget, Endcontours, Idx, Scalar(255), -1);

						//cout << contourArea(Endcontours[Idx]) << endl;

						if (contourArea(Endcontours[Idx]) > 350000)
						{
							vector<vector<Point>>Secondcontours;
							vector<Vec4i>Secondhierarchy;
							findContours(ENDTarget, Secondcontours, Secondhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
							ConvexityDefectsTwo(Secondcontours);//��Ŀ����������͹��ȱ�ݼ��   ����·��봫�ʹ���ճ�����
							if (SecFarpoints.size() == 1 || SecFarpoints.size() == 2)
							{

								if (SecFarpoints.size() == 1)
								{
									line(ENDTarget, Point(0, SecFarpoints[0].y), Point(img.cols, SecFarpoints[0].y), Scalar(0, 0, 0), 5);
								}

								else if (SecFarpoints.size() == 2)
								{
									if (abs(SecFarpoints[0].y - SecFarpoints[1].y) < 500)
									{
										line(ENDTarget, SecFarpoints[0], SecFarpoints[1], Scalar(0, 0, 0), 5);
									}
									else
									{
										line(ENDTarget, Point(0, SecFarpoints[0].y), Point(img.cols, SecFarpoints[0].y), Scalar(0, 0, 0), 5);
										line(ENDTarget, Point(0, SecFarpoints[1].y), Point(img.cols, SecFarpoints[1].y), Scalar(0, 0, 0), 5);
									}

								}
							}

							//��Ŀ���������в��� ������ճ�������
							vector<vector<Point>>SecEndcontours;//Ѱ�Ҵ�����Ŀ������
							vector<Vec4i>SecEndhierarchy;
							findContours(ENDTarget, SecEndcontours, SecEndhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

							int Idx = 0;
							double dist = 2000;
							for (int i = 0; i < SecEndcontours.size(); i++)
							{
								Moments m = moments(SecEndcontours[i], true);
								Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
								double a = getdistance(pt_c, center);
								if (dist > a)
								{
									dist = a;
									Idx = i;
								}
							}

							Mat SecENDTarget = Mat::zeros(img.size(), CV_8UC1);
							drawContours(SecENDTarget, SecEndcontours, Idx, Scalar(255), -1);
							if (contourArea(SecEndcontours[Idx]) < Clothes_Area[0])//220000  250000
							{

								vector<vector<Point>>contoursENDMoban;
								vector<Vec4i>hierarchyENDMoban;
								findContours(SecENDTarget, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
								Log("output����");

								//if (output[0].id == 1)
								//{
								//	DirectionConvexityDefects(contoursENDMoban);
								//}

								//��Ŀ����������͹������
								vector<vector<Point>>hull(contoursENDMoban.size());
								Mat dst = Mat::zeros(img.size(), CV_8UC1);
								for (int i = 0; i < contoursENDMoban.size(); i++)
								{
									convexHull(contoursENDMoban[i], hull[i], false, true);
									//����͹��
									drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
								}


								//�������Ͻ���ģ��ƥ�����
								vector<Point> inputcurve = hull[0];
								vector<Point>outputcurve;
								approxPolyDP(inputcurve, outputcurve, 100, true);
								Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
								drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);


								saveangle(outputcurve);//�洢�ڼн�
								//SaveSide(outputcurve);//�ҵ���׼��
						/*		cout << "outputcurve" << outputcurve.size() << endl;*/
								if (outputcurve.size() >= 4)
								{
									int num = 0;
									for (int i = 0; i < ANGLE.size(); i++)
									{
										if (ANGLE[i] < 60)
										{
											num++;
										}
									}
									num = 0;
									if (num == 0)
									{
										calminAreaRect(Mat(contoursENDMoban[0]));
										Moments m = moments(contoursENDMoban[0], true);
										Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
										double maxdis = 0;
										double mindis = 1000;
										for (int j = 0; j < 4; j++)
										{
											line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
											double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
											if (longdis > maxdis)
											{
												maxdis = longdis;
											}
											if (longdis < mindis)
											{
												mindis = longdis;
											}
										}
										Maxdis[0] = maxdis;

										Mindis[0] = mindis;
										EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
										EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
										ENDpoint.push_back(center);
										circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
										circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

										/*if (output[0].id == 1)
										{
											if (Direpoints.size() == 0)
											{
											}
											else
											{
												depthsort(Direpoints, Diredepths);
												if (ENDDirepoints[0].x >= ENDpoint[0].x)
												{
													direnum.push_back(1);
												}
												else
												{
													direnum.push_back(2);
												}
											}
										}*/
										double maxDistance = 0;
										int index1 = -1, index2 = -1;

										for (int i = 0; i < ZXpoint.size(); ++i)
										{
											int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

											double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

											if (distance > maxDistance)
											{
												maxDistance = distance;
												index1 = i;
												index2 = nextIndex;
											}
										}
										/*								line(img, ZXpoint[index1], ZXpoint[index2], Scalar(0, 0, 255), 4);*/
										double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
										sortidx.clear();
										ENDAngle.push_back(ang);
										results = true;
										std::ostringstream ss;
										ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
											<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
											<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
										std::string Text = ss.str();
										cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
									}
									else
									{
										results = false;
									}
								}
								else
								{
									results = false;
								}
							}
							else
							{
								results = false;
							}
						}

						else
						{
							if (contourArea(Endcontours[Idx]) < Clothes_Area[0])//220000  250000
							{

								vector<vector<Point>>contoursENDMoban;
								vector<Vec4i>hierarchyENDMoban;
								findContours(ENDTarget, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
								Log("output����");
								//if (output[0].id == 1)
								//{
								//	DirectionConvexityDefects(contoursENDMoban);
								//}

								//��Ŀ����������͹������
								vector<vector<Point>>hull(contoursENDMoban.size());
								Mat dst = Mat::zeros(img.size(), CV_8UC1);
								for (int i = 0; i < contoursENDMoban.size(); i++)
								{
									convexHull(contoursENDMoban[i], hull[i], false, true);
									//����͹��
									drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
								}


								//�������Ͻ���ģ��ƥ�����
								vector<Point> inputcurve = hull[0];
								vector<Point>outputcurve;
								approxPolyDP(inputcurve, outputcurve, 100, true);
								Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
								drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);


								saveangle(outputcurve);//�洢�ڼн�
								//SaveSide(outputcurve);//�ҵ���׼��

								//cout << "outputcurve" << outputcurve.size() << endl;
								if (outputcurve.size() >= 4)
								{
									int num = 0;
									for (int i = 0; i < ANGLE.size(); i++)
									{
										if (ANGLE[i] < 60)
										{
											num++;
										}
									}
									num = 0;
									if (num == 0)
									{
										calminAreaRect(Mat(contoursENDMoban[0]));
										Moments m = moments(contoursENDMoban[0], true);
										Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
										double maxdis = 0;
										double mindis = 1000;
										for (int j = 0; j < 4; j++)
										{
											line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
											double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
											if (longdis > maxdis)
											{
												maxdis = longdis;
											}
											if (longdis < mindis)
											{
												mindis = longdis;
											}
										}
										Maxdis[0] = maxdis;

										Mindis[0] = mindis;
										EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
										EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
										ENDpoint.push_back(center);
										circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
										circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

										/*if (output[0].id == 1)
										{
											if (Direpoints.size() == 0)
											{
											}
											else
											{
												depthsort(Direpoints, Diredepths);
												if (ENDDirepoints[0].x >= ENDpoint[0].x)
												{
													direnum.push_back(1);
												}
												else
												{
													direnum.push_back(2);
												}
											}
										}*/
										double maxDistance = 0;
										int index1 = -1, index2 = -1;

										for (int i = 0; i < ZXpoint.size(); ++i)
										{
											int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

											double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

											if (distance > maxDistance)
											{
												maxDistance = distance;
												index1 = i;
												index2 = nextIndex;
											}
										}
										/*								line(img, ZXpoint[index1], ZXpoint[index2], Scalar(0, 0, 255), 4);*/
										double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
										sortidx.clear();
										ENDAngle.push_back(ang);
										results = true;
										std::ostringstream ss;
										ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
											<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
											<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
										std::string Text = ss.str();
										cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
									}
									else
									{
										results = false;
									}
								}
								else
								{
									results = false;
								}
							}
							else
							{
								results = false;
							}
						}

					}

					//����·��պô��ڴ��ʹ�����
					else if (Farpoints.size() == 0)
					{
						//cout << contourArea(contoursDingwei[MinIdx]) << endl;
						if (contourArea(contoursDingwei[MinIdx]) < Clothes_Area[0])   //�����б��Ƿ���������·�  220000  250000
						{

							vector<vector<Point>>contoursENDMoban;
							vector<Vec4i>hierarchyENDMoban;
							findContours(Target, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

							vector<vector<Point>>hull(contoursENDMoban.size());
							Mat dst = Mat::zeros(img.size(), CV_8UC1);
							for (int i = 0; i < contoursENDMoban.size(); i++)
							{
								convexHull(contoursENDMoban[i], hull[i], false, true);
								//����͹��
								drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
							}

							//�������Ͻ���ģ��ƥ�����
							vector<Point> inputcurve = hull[0];
							vector<Point>outputcurve;
							approxPolyDP(inputcurve, outputcurve, 100, true);

							Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
							drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);

							saveangle(outputcurve);//�洢�ڼн�
							//SaveSide(outputcurve);//�ҵ���׼��

							if (outputcurve.size() >= 4)
							{

								int num = 0;

								if (num == 0)
								{

									calminAreaRect(Mat(contoursENDMoban[0]));
									Moments m = moments(contoursENDMoban[0], true);
									Point2f center(m.m10 / m.m00, m.m01 / m.m00);//��Ϊ����
									double maxdis = 0;
									double mindis = 1000;
									for (int j = 0; j < 4; j++)
									{
										line(img, ZXpoint[j], ZXpoint[(j + 1) % 4], Scalar(255, 0, 0), 5, 8);
										double longdis = getdistance(ZXpoint[j], ZXpoint[(j + 1) % 4]);
										if (longdis > maxdis)
										{
											maxdis = longdis;
										}
										if (longdis < mindis)
										{
											mindis = longdis;
										}
									}
									Maxdis[0] = maxdis;
									Mindis[0] = mindis;
									EndMaxdis[0] = Maxdis[0] * XYscale[0];//�洢���γ���
									EndMindis[0] = Mindis[0] * XYscale[0];//�洢���ζ̱�
									ENDpoint.push_back(center);
									circle(img, ENDpoint[0], 5, Scalar(0, 255, 0), 5);
									circle(img, imagecenter[0], 5, Scalar(255, 0, 0), 5);

									double maxDistance = 0;
									int index1 = -1, index2 = -1;

									for (int i = 0; i < ZXpoint.size(); ++i)
									{
										int nextIndex = (i + 1) % ZXpoint.size();  // Get the index of the next point, considering it's cyclic.

										double distance = getdistance(ZXpoint[i], ZXpoint[nextIndex]);

										if (distance > maxDistance)
										{
											maxDistance = distance;
											index1 = i;
											index2 = nextIndex;
										}
									}
									/*					line(img, ZXpoint[index1], ZXpoint[index2], Scalar(0, 0, 255), 4);*/
									double ang = calangle_deg(ZXpoint[index1], ZXpoint[index2]);
									sortidx.clear();
									ENDAngle.push_back(ang);
									results = true;
									std::ostringstream ss;
									ss << "X: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].x - imagecenter[0].x) * 100.0f) / 100) * XYscale[0]
										<< "   Y: " << std::fixed << std::setprecision(3) << static_cast<int>(std::round((ENDpoint[0].y - imagecenter[0].y) * 100.0f) / 100) * XYscale[0]
										<< "    Angle:" << std::fixed << std::setprecision(3) << ENDAngle[0];
									std::string Text = ss.str();
									cv::putText(img, Text, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 4);
								}
								else
								{
									results = false;
								}
							}
							else
							{
								results = false;
							}
						}
						else
						{
							results = false;
						}
					}
					else
					{
						results = false;
					}
				}
				ZXpoint.clear();
				Farpoints.clear();
				SecFarpoints.clear();
				ANGLE.clear();
				Dist.clear();
				imagecenter.clear();
				
				ENDDirepoints.clear();
				Diredepths.clear();
				Direpoints.clear();
			}
			else
			{
				results = false;
			}
		}
	}
	Log("�����:", results);
	clock_t detectfinish = clock();  // ��¼�������ʱ��
	double detecttime_length = (double)(detectfinish - detectstart) / CLOCKS_PER_SEC; //��������ʱ�̵Ĳ����������ʱ�� 
	Log("ͼ���⴦���ʱ:", detecttime_length, "ms");
	return img;
}

// ���ݷ���
void BaslerCamera::sendData()
{
	clock_t plcstart = clock(); // ��¼����ʼʱ��
	ClothCount++;
	Log("---------------------��", ClothCount, "��--------------------");
	int clothesSN[1] = { ClothCount };

	Log("��������ʱ,PLC����״̬", plcresult);
	if (plcresult)
	{
		plc.PLCWrite(0xA8, &clothesSN[0], 1, 4031);//��¼�ڶ��ټ��·�
	}
	if (results)
	{
		convertangle(ENDAngle[0]);
		convertpoint(ENDpoint[0]);

		int16_t Lowpart1, Lowpart2, Lowpart3, Lowpart4, Highpart1, Highpart2, Highpart3, Highpart4;
		SplitInt32toInt16(FinalpointX[0], Lowpart1, Highpart1);
		SplitInt32toInt16(FinalpointY[0], Lowpart2, Highpart2);
		SplitInt32toInt16(EndMaxdis[0], Lowpart3, Highpart3);
		SplitInt32toInt16(EndMindis[0], Lowpart4, Highpart4);
		//cout << "Lowpart1" << Lowpart1 << endl;
		//cout << "Highpart1" << Highpart1 << endl;
		if (plcresult)
		{
			Data[0] = 1;
			Data[1] = 0;
			Data[2] = Lowpart1;
			Data[3] = Highpart1;
			Data[4] = Lowpart2;
			Data[5] = Highpart2;
			Data[6] = Finalangle[0];
			Data[7] = 0;
			Data[8] = Lowpart3;
			Data[9] = Highpart3;
			Data[10] = Lowpart4;
			Data[11] = Highpart4;


			if (plc.PLCWriteBatch(0xA8, Data, 12, 4040))
			{
				Log("�ɹ����͵�", ClothCount, "����λ����");
			}
			else
			{
				Log("���͵�", ClothCount, "����λ����ʧ��");
			}
			//plc.PLCWrite(0xA8, &FinalpointX[0], 1, 4022);//���Ͷ�λ��־
			//plc.PLCWrite(0xA8, &FinalpointY[0], 1, 4024);//���Ͷ�λ��־
			//plc.PLCWrite(0xA8, &Finalangle[0], 1, 4026);//���Ͷ�λ��־
			//plc.PLCWrite(0xA8, &EndMaxdis[0], 1, 4032);//���;��γ���
			//plc.PLCWrite(0xA8, &EndMindis[0], 1, 4034);//���;��ο��
			//int OK[1] = { 1 };
			//plc.PLCWrite(0xA8, &OK[0], 1, 4020);//����ɨ���־
		}
		Log("�ɹ������", ClothCount, "��", " ", "���ĵ�X:", FinalpointX[0], " ", "���ĵ�Y:", FinalpointY[0], " ", "��б�� : ", Finalangle[0]);
	}
	else
	{
		if (plcresult)
		{
			int OK[1] = { 2 };
			int w = plc.PLCWrite(0xA8, &OK[0], 1, 4040);//����ɨ���־
		}
		Log("δ�ɹ������", ClothCount, "��");
	}
	clock_t plcfinish = clock();  // ��¼�������ʱ��
	double plctime_length = (double)(plcfinish - plcstart) / CLOCKS_PER_SEC; //��������ʱ�̵Ĳ����������ʱ�� 
	Log("PLC�������ݴ����ʱ:", plctime_length, "ms");
}

void BaslerCamera::SaveImage(cv::Mat& img, int OK)
{
	SYSTEMTIME st;
	QString m_strFolderPath, m_strImagePath;
	GetLocalTime(&st);
	if (OK == 1)
	{
		m_strFolderPath = QString("%1/%2-%3").arg(srcDirPathOK).arg(st.wMonth).arg(st.wDay);
		m_strImagePath = QString("%1/%2-%3/%4-%5-%6-Clothes%7.bmp").arg(srcDirPathOK).arg(st.wMonth).arg(st.wDay).arg(st.wHour).arg(st.wMinute).arg(st.wSecond).arg(ClothCount);
	}
	else
	{
		m_strFolderPath = QString("%1/%2-%3").arg(srcDirPathNG).arg(st.wMonth).arg(st.wDay);
		m_strImagePath = QString("%1/%2-%3/%4-%5-%6-Clothes%7.bmp").arg(srcDirPathNG).arg(st.wMonth).arg(st.wDay).arg(st.wHour).arg(st.wMinute).arg(st.wSecond).arg(ClothCount);
	}

	printf("%s\n", m_strFolderPath.toStdString().data());
	LPCWSTR pszPath = (LPCWSTR)m_strFolderPath.toStdWString().c_str();
	if (!PathIsDirectoryW(pszPath))
		CreateDirectoryW(pszPath, NULL);
	std::string imgPath = m_strImagePath.toStdString();
	imwrite(imgPath, img);
	IfWritePLC = false;
}

void BaslerCamera::run()
{
	if (!this->StartCamera())
	{
		return;
	}
	stopState = false;

	// pylonimgתopencvimg��ʼ��
	Pylon::CImageFormatConverter formatConverter;//�½�pylon ImageFormatConverter����
	formatConverter.OutputPixelFormat = Pylon::PixelType_BGR8packed;//ȷ��������ظ�ʽ
	Pylon::CPylonImage pylonImage;//����һ��Pylonlmage��������������OpenCV images

	while (!stopState)
	{
		try
		{
			while (mCamera->IsGrabbing())
			{
				// �ȴ����պͻָ�ͼ�񣬳�ʱʱ������Ϊ5000 ms
				mCamera->RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
				// ���ͼ��ץȡ�ɹ�
				if (ptrGrabResult->GrabSucceeded())
				{
					clock_t start = clock(); // ��¼����ʼʱ��
					// ���ͼ�������
					formatConverter.Convert(pylonImage, ptrGrabResult);//��ץȡ�Ļ�������ת����pylon image.
					openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());//�� pylon imageת��OpenCV image.

					// ͼ����
					Mat img = imread("E:/1113.png");

					////���PLC����Ͽ���������
					//if (!plc.PLCWrite(0xA8, &plcnum[0], 1, 4035))
					//{
					//	plcCLnum++;
					//	Log("���ڽ��е�", plcCLnum,"��PLC�Ͽ�������");
					//	plcresult = false;
					//	plcdisconnected = true;
					//}

					 plcresult = true;
					if (BiaoDingresult)
					{

						resultimg = Dingwei(openCvImage);
					}
					else
					{
						//if (plcresult)
						//{
							//clock_t detectstart = clock(); // ��¼����ʼʱ��

							resultimg = Detect(img);

							//clock_t detectfinish = clock();  // ��¼�������ʱ��
							//double detecttime_length = (double)(detectfinish - detectstart) / CLOCKS_PER_SEC; //��������ʱ�̵Ĳ����������ʱ�� 
							//Log("ͼ���⴦���ʱ:", detecttime_length, "ms");
							sendData();
							// ͼ�񱣴�
							if (plcresult)
							{
								if (results)
								{
									int OK = 1;
									//����ͼ��
									SaveImage(resultimg, OK);
								}
								else
								{
									int OK = 0;
									//����ͼ��
									SaveImage(resultimg, OK);
								}
							}
							clock_t finish = clock();  // ��¼�������ʱ��
							double time_length = (double)(finish - start) / CLOCKS_PER_SEC; //��������ʱ�̵Ĳ����������ʱ�� 
							Log("�����ʱ:", time_length, "ms");
							Log("-----------------------------------------------------------------------------------");
							IfWritePLC = false;
						//}
						//else
						//{
						//	resultimg = openCvImage;
						//}
					}
					// ����QImage����
					image = MatToQImage(resultimg);
					// ����ͼ����ʾ�ź�
					emit showImageSignal(image);
				}
			}
		}
		catch (const Pylon::GenericException& e)
		{
			qDebug() << e.GetDescription();
		}
	}
}


