
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
extern bool plcresult;//plc连接是否成功
extern bool BiaoDingresult;//是否进行标定
extern bool Shizhiresult;//是否进行显示图像十字
extern bool RLUDresult;//是否打开图像四周边界框

bool plcSECONd = false;
bool plcstate = false;
bool plcReconnected = true;
bool IfWritePLC = false;
bool plcdisconnected = false;
int ClothCount = 0;//初始化衣服数量
int plcCLnum = 0;
int plcnum[1] = { 1 };

extern QString srcDirPathOK;
extern QString srcDirPathNG;
extern std::vector<float>XYscale;//存储xy的缩放因子  0-2
extern std::vector<float>XXDvalue; //存储xy的缩放因子  0-2
extern std::vector<float>EXPvalue;//存储曝光参数 

extern std::vector<float>RLDistance;//存储图像左右两侧间距
extern std::vector<float>UDDistance;//存储图像上下两侧间距
extern std::vector<float>Clothes_Distance;//存储两件衣服间隔
extern std::vector<float>Clothes_Area;//存储衣服面积进行判断
double getdistance(Point2f pt1, Point2f pt2);//计算两点之间距离
void ConvexityDefects(vector<vector<Point>>contours);//凸包缺陷
double calangle_deg(Point2f p1, Point2f p2);//计算拟合后的直线与水平线的夹角
void SaveSide(vector<Point>contour);//筛选多边形拟合后较短的对边中的较长边(倾斜角基础边)
double calculateAngle(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);//计算多边形内夹角
void calminAreaRect(Mat contours);//计算旋转外接矩形框的中心点与角度
void saveangle(vector<Point>contour);//存储多边形拟合后的夹角
void DirectionConvexityDefects(vector<vector<Point>>contours);//判断衣服朝向
void convertpoint(Point2f pt);
void convertangle(double ang);
void depthsort(vector<cv::Point>Direpoints, vector<int> Diredepths);//根据深度进行排序
float calculateIOU(const OutputSeg& box1, const OutputSeg& box2);//判断两个检测框的IOU

std::vector<Point2f>ENDpoint;//存储中心点
std::vector<double>ENDAngle;//存储角度
std::vector<int>direnum;//存储朝向  1--右方  2--左方

std::vector<cv::Point>Direpoints;//存储衣服朝向缺陷最深点
std::vector<cv::Point>ENDDirepoints;//存储衣服朝向缺陷最深点
std::vector<int> Diredepths;//存储凸包缺陷深度
std::vector<cv::Point>Farpoints;//存储凸包缺陷最深点SecFarpoints
std::vector<cv::Point>SecFarpoints;
std::vector<int> depths;//存储凸包缺陷深度
std::vector<int> Secdepths;//存储凸包缺陷深度
std::vector<double>Objangle;//存储中心旋转角
std::vector<Point2f>ZXpoint;//存储旋转矩形框的四个角点
std::vector<double>ANGLE;//存储多边形拟合后内夹角
std::vector<double>Dist;//存储多边形拟合后的四条边
std::vector<int>sortidx;//存储索引

int Maxdis[1];//存储矩形长边
int Mindis[1];//存储矩形短边

int EndMaxdis[1];//存储矩形长边
int EndMindis[1];//存储矩形短边

int FinalpointX[1];//存储最终中心点的X坐标
int FinalpointY[1];//存储最终中心点的Y坐标
int Finalangle[1];//存储最终角度
int Data[12];//存储一次性传入参数
float showpointX[1];
float showpointY[1];
float showangle[1];

//形态学操作尺寸核
Mat kernelmm = getStructuringElement(MORPH_RECT, Size(9, 9), Point(-1, -1));
Mat kernelml = getStructuringElement(MORPH_RECT, Size(25, 25), Point(-1, -1));

std::vector<cv::Point>imagecenter;

//将32位类型数据转成2个16位的高低位数据
void SplitInt32toInt16(int32_t input, int16_t& lowpart, int16_t& highpart)
{
	lowpart = static_cast<int16_t>(input & 0xFFFF);   //低位
	highpart = static_cast<int16_t>((input >> 16) & 0xFFFF);   //高位
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

//衣服朝向判断根据
void DirectionConvexityDefects(vector<vector<Point>>contours)
{
	vector<std::vector<Point>> hull(contours.size());//存放point类型的凸包
	vector<std::vector<int>> inthull(contours.size());//存放int类型的凸包
	vector<vector<Vec4i> > hullDefect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
		convexHull(contours[i], inthull[i]);
		convexityDefects(cv::Mat(contours[i]), inthull[i], hullDefect[i]);
	}
	// 打印排序后的凸包缺陷点
	for (size_t i = 0; i < hullDefect.size(); i++)
	{
		// 凸包缺陷迭代器设置
		vector<Vec4i>::iterator iterDefects = hullDefect[i].begin();
		// 遍历得到凸包缺陷的4个特征量并进行绘制
		while (iterDefects != hullDefect[i].end())
		{
			Vec4i& v = (*iterDefects);
			// 起始位置
			int startidx = v[0];
			Point ptStart(contours[i][startidx]);
			// 终止位置
			int endidx = v[1];
			Point ptEnd(contours[i][endidx]);
			// 内凸壳的最远的点缺陷
			int faridx = v[2];
			Point ptFar(contours[i][faridx]);
			// 凸包之间的最远点
			int depth = v[3] / 256;
			if (depth > 20)//根据缺陷深度进行筛选
			{
				Direpoints.push_back(ptFar);
				Diredepths.push_back(depth);
			}
			iterDefects++;
		}
	}
}

//凸包缺陷
void ConvexityDefects(vector<vector<Point>>contours)
{
	vector<std::vector<Point>> hull(contours.size());//存放point类型的凸包
	vector<std::vector<int>> inthull(contours.size());//存放int类型的凸包
	vector<vector<Vec4i> > hullDefect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
		convexHull(contours[i], inthull[i]);
		convexityDefects(cv::Mat(contours[i]), inthull[i], hullDefect[i]);
	}
	// 打印排序后的凸包缺陷点
	for (size_t i = 0; i < hullDefect.size(); i++)
	{
		// 凸包缺陷迭代器设置
		vector<Vec4i>::iterator iterDefects = hullDefect[i].begin();
		// 遍历得到凸包缺陷的4个特征量并进行绘制
		while (iterDefects != hullDefect[i].end())
		{
			Vec4i& v = (*iterDefects);
			// 起始位置
			int startidx = v[0];
			Point ptStart(contours[i][startidx]);
			// 终止位置
			int endidx = v[1];
			Point ptEnd(contours[i][endidx]);
			// 内凸壳的最远的点缺陷
			int faridx = v[2];
			Point ptFar(contours[i][faridx]);
			// 凸包之间的最远点
			int depth = v[3] / 256;
			if (depth > 150)//根据缺陷深度进行筛选
			{
				//cout << "横坐标为：" << ptFar.x << "纵坐标为:" << ptFar.y
				//	<< "index：" << faridx << "深度:" << depth << endl;
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

//凸包缺陷
void ConvexityDefectsTwo(vector<vector<Point>>contours)
{
	vector<std::vector<Point>> hull(contours.size());//存放point类型的凸包
	vector<std::vector<int>> inthull(contours.size());//存放int类型的凸包
	vector<vector<Vec4i> > hullDefect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
		convexHull(contours[i], inthull[i]);
		convexityDefects(cv::Mat(contours[i]), inthull[i], hullDefect[i]);
	}
	// 打印排序后的凸包缺陷点
	for (size_t i = 0; i < hullDefect.size(); i++)
	{
		// 凸包缺陷迭代器设置
		vector<Vec4i>::iterator iterDefects = hullDefect[i].begin();
		// 遍历得到凸包缺陷的4个特征量并进行绘制
		while (iterDefects != hullDefect[i].end())
		{
			Vec4i& v = (*iterDefects);
			// 起始位置
			int startidx = v[0];
			Point ptStart(contours[i][startidx]);
			// 终止位置
			int endidx = v[1];
			Point ptEnd(contours[i][endidx]);
			// 内凸壳的最远的点缺陷
			int faridx = v[2];
			Point ptFar(contours[i][faridx]);
			// 凸包之间的最远点
			int depth = v[3] / 256;
			if (depth > 150)//根据缺陷深度进行筛选
			{
				//cout << "横坐标为：" << ptFar.x << "纵坐标为:" << ptFar.y
				//	<< "index：" << faridx << "深度:" << depth << endl;
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

//计算拟合后的直线与水平线的夹角
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

//筛选多边形拟合后较短的对边中的较长边(倾斜角基础边)
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

//计算两点之间距离
double getdistance(Point2f pt1, Point2f pt2)
{
	return (sqrt(powf(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2)));
}

//计算轮廓中y值之差用于判断
int calYdistance(vector<Point>contours)
{
	if (contours.empty()) {
		// 处理空容器的情况
		return 0; // 或者返回一个指定的值，表示容器为空
	}

	int minY = 5000; // 初始化最小 y 值为第一个点的 y 坐标值
	int maxY = 0; // 初始化最大 y 值为第一个点的 y 坐标值

	// 手动遍历容器内的所有点，更新最小和最大 y 值
	for (const Point& point : contours) {
		if (point.y < minY) {
			minY = point.y; // 更新最小 y 值
		}
		if (point.y > maxY) {
			maxY = point.y; // 更新最大 y 值
		}
	}

	// 计算最大 y 值和最小 y 值的差异
	int yDifference = maxY - minY;
	return yDifference;
}

//计算多边形内夹角
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

//计算旋转外接矩形框的中心点与角度
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

//存储多边形拟合后的夹角
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
	// 将 x 和 y 坐标分别保留两位小数并转换为int类型
	int xIntValue = static_cast<int>(std::round((pt.x - imagecenter[0].x) * 100.0f) / 100); // 四舍五入int类型
	int yIntValue = static_cast<int>(std::round((pt.y - imagecenter[0].y) * 100.0f) / 100);
	float x = round((pt.x - imagecenter[0].x) * 100.0) / 100.0;
	float y = round((pt.y - imagecenter[0].y) * 100.0) / 100.0;
	showpointX[0] = x;
	showpointY[0] = y;
	// 将转换后的int值放入数组中
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
	// 将整数转换为无符号短整型
	int intValue = static_cast<int>(std::round(ang)); // 四舍五入保留两位小数
	float angle = round(ang * 100.0) / 100.0;
	showangle[0] = angle;
	Finalangle[0] = intValue;
}

// 重连线程
static unsigned int __stdcall ConnectThread(void* pUser)   //线程的入口函数   静态函数
{
	// 将传递给线程的参数强制转换为 BaslerCamera 指针
	BaslerCamera* pBaslerCamera = (BaslerCamera*)pUser;
	// 检查转换后的指针是否为 NULL
	if (!pBaslerCamera)
	{
		printf("pBaslerCamera is NULL!\n");
		return -1;
	}

	pBaslerCamera->Connect();
	return 0;
}
static unsigned int __stdcall WritePLC(void* pUser)   //线程的入口函数   静态函数
{

	// 将传递给线程的参数强制转换为 BaslerCamera 指针
	BaslerCamera* pBaslerCamera2 = (BaslerCamera*)pUser;
	// 检查转换后的指针是否为 NULL
	if (!pBaslerCamera2)
	{
		printf("pBaslerCamera2 is NULL!\n");
		return -1;

	}

	pBaslerCamera2->Write();
	return 0;
}

BaslerCamera::BaslerCamera(void) :m_isExitConnectThread(false)   //m_isExitConnectThread，这是一个布尔类型的变量，被设置为 false通常这样的变量用于控制线程的执行或者表示某个状态
, m_threadHandle(NULL)                                        //m_threadHandle(NULL): 初始化了类的成员变量 m_threadHandle
{
	stopState = FALSE;//新增初始化
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

	// 启动重连线程
	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, ConnectThread, this, CREATE_SUSPENDED, NULL);
	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, WritePLC, this, CREATE_SUSPENDED, NULL);
	//_beginthreadex 是一个线程创建函数 创建线程  NULL：表示新线程默认使用父线程的安全属性
	// 0：表示线程栈的大小，0表示使用默认值    ConnectThread：新线程的入口函数 也就是新线程会从这个函数开始执行
	//this：是传递给线程的参数 BaslerCamera 对象的指针
	//CREATE_SUSPENDED：表示新线程创建后处于挂起状态 需要调用ResumeThread函数才会开始执行
	//NULL：是传递给新线程的 ID，如果不需要获取线程 ID，则可以传递 NULL

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

	Pylon::PylonTerminate();   //释放资源
}

void BaslerCamera::Write()
{
	try
	{
		while (!m_isExitConnectThread)
		{
			while (!IfWritePLC)
			{
				std::lock_guard<std::mutex> lock(m_mutex); // 自动加锁
				//emit baslerConditionMet();
				if (plc.PLCWrite(0xA8, &plcnum[0], 1, 4036))
				{
					Log("D4036写入成功！");
				}
				else
				{
					Log("D4036写入失败！");
				}
				Sleep(10000);
			}
		}
	}
	catch (const std::exception& ex)
	{
		// 捕获所有标准异常
		Log("发生异常: " + std::string(ex.what()));
	}
	catch (...)
	{
		// 捕获所有其他异常（非标准异常）
		Log("发生未知异常！");
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
				Log("PLC意外断开时,PLC断开重连成功");
				plcresult = true;
				plcdisconnected = false;
			}

		}

	}
}

//打开相机
bool BaslerCamera::OpenCamera()
{
	if (!CameraOpening)
	{
		// 在使用任何pylon方法之前，必须初始化pylon运行时
		Pylon::PylonInitialize();

		try
		{
			// 为首先找到的相机设备创建一个即时相机对象
			if (mCamera != nullptr)
			{
				// 如果相机对象已经存在，关闭相机
				mCamera->Close();
				delete mCamera;
				mCamera = nullptr;
			}

			mCamera = new Pylon::CInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
			Log("------相机创建对象成功-------");
			if (mCamera != NULL)
			{
				nodemap = &mCamera->GetNodeMap();//获得相机参数
				// 打开相机
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
						Log("设置相机线消抖成功");
					}

				}

				CameraOpening = true;
				QMessageBox::information(0, "Success", "Open Camera Success!");

			}
		}
		catch (const Pylon::GenericException& e)
		{
			// 错误处理
			QMessageBox::warning(0, "Warning", "Open Camera Error!");
			std::cerr << "Error creating camera: " << e.GetDescription() << std::endl;
			Log("错误原因：", e.GetDescription());
			// 从输入缓冲区中删除剩余字符
			std::cin.ignore(std::cin.rdbuf()->in_avail());
			// 关闭相机，避免下次打开时的冲突
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
		Log("------相机上次处于打开模式-------");
	}
	Log("------打开相机完成-------");

	//EXPvalue.clear();
	//EXPvalue.push_back(EXPtime);
	return true;

}

//关闭相机
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

//开始采集图像
bool BaslerCamera::StartCamera()
{
	if (CameraOpening && !CameraRuning && mCamera && mCamera->CanWaitForFrameTriggerReady())//相机等待触发器触发
	{
		//通过设置grabLoopType参数，使用抓取循环线程启动抓取
		//到GrabLoop_ProvidedByInstantCamera。抓取结果被传递给图像事件处理程序
		//使用GrabStrategy_OneByOne默认抓取策略
		mCamera->StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByUser);

		CameraRuning = true;  //开始采集
		return true;
	}
	else
	{
		return false;
	}

}
// 停止采集图像
void BaslerCamera::StopCamera()
{
	if (mCamera)
	{
		stopState = true;
		mCamera->StopGrabbing();
		CameraRuning = false;
	}
}

//标定程序
cv::Mat BaslerCamera::Dingwei(cv::Mat& img)
{
	if (XYscale.empty())
	{
		XYscale.push_back(1.000);
	}
	Log("比例缩放因子：", XYscale.size());
	Point center = Point(img.cols / 2, img.rows / 2);
	Mat gray, grayimg;
	cvtColor(img, gray, COLOR_BGR2GRAY);//灰度化
	GaussianBlur(gray, grayimg, Size(5, 5), 0);//高斯滤波
	Mat thresh;
	threshold(grayimg, thresh, 200, 255, cv::THRESH_BINARY_INV);//反二值化
	erode(thresh, thresh, kernelmm);
	morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//开操作消除小白点
	morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//闭操作消除小黑点

	//第一次查找所有轮廓
	vector<vector<Point>>contoursDingwei;
	vector<Vec4i>hierarchyDingwei;
	findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	Mat contour = Mat::zeros(img.size(), CV_8UC1);
	vector<double> g_dConArea(contoursDingwei.size());
	for (int i = 0; i < contoursDingwei.size(); i++)
	{
		g_dConArea[i] = contourArea(contoursDingwei[i]);
	}

	//寻找离图片中心最近的轮廓(目标轮廓)
	int MinIdx = 0;
	int Mindist = 2000;
	for (int i = 0; i < contoursDingwei.size(); i++)
	{
		//cout << g_dConArea[i] << endl;
		if (g_dConArea[i] > 5000 && g_dConArea[i] < 300000)//筛选一些干扰轮廓
		{

			Moments m = moments(contoursDingwei[i], true);
			Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
			double a = getdistance(pt_c, center);
			if (Mindist > a)
			{
				Mindist = a;
				MinIdx = i;
			}
		}
	}

	Mat Target = Mat::zeros(img.size(), CV_8UC1);
	drawContours(Target, contoursDingwei, MinIdx, Scalar(255, 255, 255), -1);//绘制目标轮廓 
	Mat roi = Mat::zeros(Target.size(), CV_8UC1);
	gray.copyTo(roi, Target);
	Mat dst;
	std::vector<Vec3f> circles;
	HoughCircles(roi, circles, HOUGH_GRADIENT, 1,
		5,  // minDist: 圆心间的最小距离
		100,  // Canny边缘检测的阈值高
		30,   // 圆心检测阈值（较小的值意味着更严格的检测）
		1,   // 最小半径
		150); // 最大半径  20

	// 在图像上绘制检测到的圆
	for (size_t i = 0; i < circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(img, center, radius, Scalar(0, 0, 255), 2, LINE_AA);
		cout << "第" << i << "个圆的圆心X坐标为：" << circles[i][0] * XYscale[0] << "," << "圆心Y坐标为：" << circles[i][1] * XYscale[0] << endl;
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

//执行软件触发
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

//将cv类型图片转为qt格式
QImage BaslerCamera::MatToQImage(const cv::Mat& mat)
{
	// 根据mat的数据创建QImage
	QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);
	// 如果图像是BGR格式，则进行颜色通道的交换
	if (mat.channels() == 3 && mat.type() == CV_8UC3)
	{
		return image.rgbSwapped();
	}
	return image;
}

//定位程序
cv::Mat BaslerCamera::Detect(cv::Mat& img)
{
	clock_t detectstart = clock(); // 记录程序开始时间
	results = false;
	IfWritePLC = true;
	if (plcresult)
	{
		int Choose = 1;
		if (Choose == 1)
		{
			clock_t TuiListart = clock(); // 记录程序开始时间
			int EndOutput;
			std::vector<OutputSeg> output;
			bool find = test.OnnxDetect(img, output);
			clock_t TuiLifinish = clock();  // 记录程序结束时间
			double TuiLitime_length = (double)(TuiLifinish - TuiListart) / CLOCKS_PER_SEC; //根据两个时刻的差，计算出处理的时间 
			Log("图像推理处理耗时:", TuiLitime_length, "ms");
			Log("目标个数:", output.size());
			cout << "目标个数:" << output.size() << endl;

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
			cout << "最终目标个数:" << EndOutput << endl;
			Log("最终目标个数:", EndOutput);
			Point center = Point(img.cols / 2, img.rows / 2);
			imagecenter.push_back(center);

			if (Shizhiresult)
			{
				line(img, Point(imagecenter[0].x - img.cols / 2, imagecenter[0].y), Point(imagecenter[0].x + img.cols / 2, imagecenter[0].y), Scalar(0, 0, 255), 2);
				line(img, Point(imagecenter[0].x, imagecenter[0].y - img.rows / 2), Point(imagecenter[0].x, imagecenter[0].y + img.rows / 2), Scalar(0, 0, 255), 2);
			}
			if (RLUDresult)
			{
				line(img, Point(RLDistance[0], 0), Point(RLDistance[0], img.rows), Scalar(0, 0, 255), 2);//左边竖线
				line(img, Point(img.cols - RLDistance[0], 0), Point(img.cols - RLDistance[0], img.rows), Scalar(0, 0, 255), 2);//右边竖线
				line(img, Point(0, UDDistance[0]), Point(img.cols, UDDistance[0]), Scalar(0, 0, 255), 2);//上边横线
				line(img, Point(0, img.rows - UDDistance[0]), Point(img.cols, img.rows - UDDistance[0]), Scalar(0, 0, 255), 2);//上边横线
			}
			if (EndOutput == 1 || EndOutput == 2)
			{
				if (EndOutput == 1)
				{
			/*		cout << "22332" << endl;*/
					//cv::rectangle(img, output[0].box, cv::Scalar(255, 250, 0), 3);
					Mat gray, grayimg;
					cvtColor(img, gray, COLOR_BGR2GRAY);//灰度化
					GaussianBlur(gray, grayimg, Size(5, 5), 0);//高斯滤波
					Mat thresh;
					threshold(grayimg, thresh, 50, 255, cv::THRESH_BINARY_INV + THRESH_OTSU);//反二值化
					erode(thresh, thresh, kernelml);
					morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//开操作消除小白点
					morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//闭操作消除小黑点

					//第一次查找所有轮廓
					vector<vector<Point>>contoursDingwei;
					vector<Vec4i>hierarchyDingwei;
					findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

					Mat contour = Mat::zeros(img.size(), CV_8UC1);
					vector<double> g_dConArea(contoursDingwei.size());
					for (int i = 0; i < contoursDingwei.size(); i++)
					{
						g_dConArea[i] = contourArea(contoursDingwei[i]);
					}

					//寻找离图片中心最近的轮廓(目标轮廓)
					int MinIdx = 0;
					int Mindist = 2000;
					for (int i = 0; i < contoursDingwei.size(); i++)
					{
						int Ydis = calYdistance(contoursDingwei[i]);
						//cout << g_dConArea[i] << endl;
	/*					cout << "Ydis" << Ydis << endl;*/
						if (g_dConArea[i] > 60000 && Ydis < 1100)//筛选一些干扰轮廓 100000 900
						{
	
							Moments m = moments(contoursDingwei[i], true);
							Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
							double a = getdistance(pt_c, center);
							if (RLDistance.empty())
							{
								RLDistance.push_back(100);
							}
							if (UDDistance.empty())
							{
								UDDistance.push_back(100);
							}

							if (pt_c.x > RLDistance[0] && pt_c.x < img.cols - RLDistance[0] && pt_c.y>UDDistance[0] && pt_c.y < img.rows - UDDistance[0])//根据轮廓中心点进行判断NG情况
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
						drawContours(Target, contoursDingwei, MinIdx, Scalar(255), -1);//绘制目标轮廓
						//imshow("Target", Target);
						//waitKey(0);
						vector<vector<Point>>contoursTubao;
						vector<Vec4i>hierarchyTubao;
						findContours(Target, contoursTubao, hierarchyTubao, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
						ConvexityDefects(contoursTubao);//对目标轮廓进行凸包缺陷检测   针对衣服与传送带有粘连情况
						Log("缺陷点个数:", Farpoints.size());
						//cout << "缺陷点个数:" << Farpoints.size() << endl;
						//circle(img, Farpoints[0], 4, Scalar(0, 0, 255), 4);
						//针对衣服与传送带有粘连情况
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

							//对目标轮廓进行操作 （处理粘连情况）
							vector<vector<Point>>Endcontours;//寻找处理后的目标轮廓
							vector<Vec4i>Endhierarchy;
							findContours(Target, Endcontours, Endhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

							int Idx = 0;
							double dist = 2000;
							for (int i = 0; i < Endcontours.size(); i++)
							{
								Moments m = moments(Endcontours[i], true);
								Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
								ConvexityDefectsTwo(Secondcontours);//对目标轮廓进行凸包缺陷检测   针对衣服与传送带有粘连情况
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

								//对目标轮廓进行操作 （处理粘连情况）
								vector<vector<Point>>SecEndcontours;//寻找处理后的目标轮廓
								vector<Vec4i>SecEndhierarchy;
								findContours(ENDTarget, SecEndcontours, SecEndhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

								int Idx = 0;
								double dist = 2000;
								for (int i = 0; i < SecEndcontours.size(); i++)
								{
									Moments m = moments(SecEndcontours[i], true);
									Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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

									//对目标轮廓进行凸包操作
									vector<vector<Point>>hull(contoursENDMoban.size());
									Mat dst = Mat::zeros(img.size(), CV_8UC1);
									for (int i = 0; i < contoursENDMoban.size(); i++)
									{
										convexHull(contoursENDMoban[i], hull[i], false, true);
										//绘制凸包
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}


									//多边形拟合进行模板匹配操作
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 200, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);
									saveangle(outputcurve);//存储内夹角
									//SaveSide(outputcurve);//找到基准边
	
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
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
											EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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

									//对目标轮廓进行凸包操作
									vector<vector<Point>>hull(contoursENDMoban.size());
									Mat dst = Mat::zeros(img.size(), CV_8UC1);
									for (int i = 0; i < contoursENDMoban.size(); i++)
									{
										convexHull(contoursENDMoban[i], hull[i], false, true);
										//绘制凸包
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}


									//多边形拟合进行模板匹配操作
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 100, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);


									saveangle(outputcurve);//存储内夹角
									//SaveSide(outputcurve);//找到基准边

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
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
											EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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

						//针对衣服刚好处在传送带中心
						else if (Farpoints.size() == 0)
						{
	
							if (contourArea(contoursDingwei[MinIdx]) < Clothes_Area[0])   //用来判别是否存在两件衣服  220000  250000
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
									//绘制凸包
									drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
								}

								//多边形拟合进行模板匹配操作
								vector<Point> inputcurve = hull[0];
								vector<Point>outputcurve;
								approxPolyDP(inputcurve, outputcurve, 100, true);

								Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
								drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);

								saveangle(outputcurve);//存储内夹角
								//SaveSide(outputcurve);//找到基准边

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
										Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
										EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
										EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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
					/*			cout << "进入两件衣服" << endl;*/
					int area1 = output[0].box.area();
					int area2 = output[1].box.area();

					if (((area1 > 60000 && area1 < 600000) || (area2 > 60000 && area2 < 600000)) && Cloresults)  //400000 100000
					{

						Mat gray, grayimg;
						cvtColor(img, gray, COLOR_BGR2GRAY);//灰度化
						GaussianBlur(gray, grayimg, Size(5, 5), 0);//高斯滤波
						Mat thresh;
						threshold(grayimg, thresh, 50, 255, cv::THRESH_BINARY_INV + THRESH_OTSU);//反二值化
						erode(thresh, thresh, kernelml);
						morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//开操作消除小白点
						morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//闭操作消除小黑点
						//第一次查找所有轮廓
						vector<vector<Point>>contoursDingwei;
						vector<Vec4i>hierarchyDingwei;
						findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
						Mat contour = Mat::zeros(img.size(), CV_8UC1);
						vector<double> g_dConArea(contoursDingwei.size());
						for (int i = 0; i < contoursDingwei.size(); i++)
						{
							g_dConArea[i] = contourArea(contoursDingwei[i]);
						}
						//寻找离图片中心最近的轮廓(目标轮廓)
						int MinIdx = 0;
						int Mindist = 2000;
						for (int i = 0; i < contoursDingwei.size(); i++)
						{
							int Ydis = calYdistance(contoursDingwei[i]);
							if (g_dConArea[i] > 60000 && Ydis < 900)//筛选一些干扰轮廓 100000
							{

								Moments m = moments(contoursDingwei[i], true);
								Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
								double a = getdistance(pt_c, center);
								if (RLDistance.empty())
								{
									RLDistance.push_back(100);
								}
								if (UDDistance.empty())
								{
									UDDistance.push_back(100);
								}

								if (pt_c.x > RLDistance[0] && pt_c.x < img.cols - RLDistance[0] && pt_c.y>UDDistance[0] && pt_c.y < img.rows - UDDistance[0])//根据轮廓中心点进行判断NG情况
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
							drawContours(Target, contoursDingwei, MinIdx, Scalar(255), -1);//绘制目标轮廓
							vector<vector<Point>>contoursTubao;
							vector<Vec4i>hierarchyTubao;
							findContours(Target, contoursTubao, hierarchyTubao, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
							ConvexityDefects(contoursTubao);//对目标轮廓进行凸包缺陷检测   针对衣服与传送带有粘连情况

							/*cout <<"缺陷点个数：" << Farpoints.size() << endl;*/
							//针对衣服与传送带有粘连情况
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
								//对目标轮廓进行操作 （处理粘连情况）
								vector<vector<Point>>Endcontours;//寻找处理后的目标轮廓
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
									Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
								//cout <<"最终目标轮廓面积大小：" << contourArea(Endcontours[Idx]) << endl;
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
									//对目标轮廓进行凸包操作
									vector<vector<Point>>hull(contoursENDMoban.size());
									Mat dst = Mat::zeros(img.size(), CV_8UC1);
									for (int i = 0; i < contoursENDMoban.size(); i++)
									{
										convexHull(contoursENDMoban[i], hull[i], false, true);
										//绘制凸包
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}
									//多边形拟合进行模板匹配操作
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 100, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);
									saveangle(outputcurve);//存储内夹角
									//SaveSide(outputcurve);//找到基准边
						/*			cout << "多变形拟合后边数：" << outputcurve.size() << endl;*/
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
										/*cout << "角度num：" << num << endl;*/
										num = 0;
										if (num == 0)
										{

											calminAreaRect(Mat(contoursENDMoban[0]));
											Moments m = moments(contoursENDMoban[0], true);
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
											EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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
							//针对衣服刚好处在传送带中心
							else if (Farpoints.size() == 0)
							{
								//cout << "衣服处于中间位置时，最终轮廓面积：" << contourArea(contoursDingwei[MinIdx]) << endl;
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
										//绘制凸包
										drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
									}
									//多边形拟合进行模板匹配操作
									vector<Point> inputcurve = hull[0];
									vector<Point>outputcurve;
									approxPolyDP(inputcurve, outputcurve, 100, true);
									Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
									drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);
									saveangle(outputcurve);//存储内夹角
									//SaveSide(outputcurve);//找到基准边
									/*cout << "衣服处于中间位置时,多边形拟合边数：" << outputcurve.size() << endl;*/
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
										/*cout << "衣服处于中间位置时,角度数目：" << num << endl;*/
										num = 0;
										if (num == 0)
										{

											calminAreaRect(Mat(contoursENDMoban[0]));
											Moments m = moments(contoursENDMoban[0], true);
											Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
											EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
											EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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
				cvtColor(img, gray, COLOR_BGR2GRAY);//灰度化
				GaussianBlur(gray, grayimg, Size(5, 5), 0);//高斯滤波
				Mat thresh;
				threshold(grayimg, thresh, 50, 255, cv::THRESH_BINARY_INV + THRESH_OTSU);//反二值化
				erode(thresh, thresh, kernelml);
				morphologyEx(thresh, thresh, MORPH_OPEN, kernelml);//开操作消除小白点
				morphologyEx(thresh, thresh, MORPH_CLOSE, kernelmm);//闭操作消除小黑点

				//第一次查找所有轮廓
				vector<vector<Point>>contoursDingwei;
				vector<Vec4i>hierarchyDingwei;
				findContours(thresh, contoursDingwei, hierarchyDingwei, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

				Mat contour = Mat::zeros(img.size(), CV_8UC1);
				vector<double> g_dConArea(contoursDingwei.size());
				for (int i = 0; i < contoursDingwei.size(); i++)
				{
					g_dConArea[i] = contourArea(contoursDingwei[i]);
				}

				//寻找离图片中心最近的轮廓(目标轮廓)
				int MinIdx = 0;
				int Mindist = 2000;
				for (int i = 0; i < contoursDingwei.size(); i++)
				{
					int Ydis = calYdistance(contoursDingwei[i]);
					if (g_dConArea[i] > 60000 && Ydis < 900)//筛选一些干扰轮廓 100000
					{

						Moments m = moments(contoursDingwei[i], true);
						Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
						double a = getdistance(pt_c, center);
						if (RLDistance.empty())
						{
							RLDistance.push_back(100);
						}
						if (UDDistance.empty())
						{
							UDDistance.push_back(100);
						}

						if (pt_c.x > RLDistance[0] && pt_c.x < img.cols - RLDistance[0] && pt_c.y>UDDistance[0] && pt_c.y < img.rows - UDDistance[0])//根据轮廓中心点进行判断NG情况
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
					//cout << "进入处理流程" << endl;
					Mat Target = Mat::zeros(img.size(), CV_8UC1);
					drawContours(Target, contoursDingwei, MinIdx, Scalar(255), -1);//绘制目标轮廓

					vector<vector<Point>>contoursTubao;
					vector<Vec4i>hierarchyTubao;
					findContours(Target, contoursTubao, hierarchyTubao, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
					ConvexityDefects(contoursTubao);//对目标轮廓进行凸包缺陷检测   针对衣服与传送带有粘连情况
					Log("缺陷点个数:", Farpoints.size());
					//circle(img, Farpoints[0], 4, Scalar(0, 0, 255), 4);
					//针对衣服与传送带有粘连情况
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

						//对目标轮廓进行操作 （处理粘连情况）
						vector<vector<Point>>Endcontours;//寻找处理后的目标轮廓
						vector<Vec4i>Endhierarchy;
						findContours(Target, Endcontours, Endhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

						int Idx = 0;
						double dist = 2000;
						for (int i = 0; i < Endcontours.size(); i++)
						{
							
							Moments m = moments(Endcontours[i], true);
							Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
							ConvexityDefectsTwo(Secondcontours);//对目标轮廓进行凸包缺陷检测   针对衣服与传送带有粘连情况
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

							//对目标轮廓进行操作 （处理粘连情况）
							vector<vector<Point>>SecEndcontours;//寻找处理后的目标轮廓
							vector<Vec4i>SecEndhierarchy;
							findContours(ENDTarget, SecEndcontours, SecEndhierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

							int Idx = 0;
							double dist = 2000;
							for (int i = 0; i < SecEndcontours.size(); i++)
							{
								Moments m = moments(SecEndcontours[i], true);
								Point2f pt_c(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
								Log("output错误");

								//if (output[0].id == 1)
								//{
								//	DirectionConvexityDefects(contoursENDMoban);
								//}

								//对目标轮廓进行凸包操作
								vector<vector<Point>>hull(contoursENDMoban.size());
								Mat dst = Mat::zeros(img.size(), CV_8UC1);
								for (int i = 0; i < contoursENDMoban.size(); i++)
								{
									convexHull(contoursENDMoban[i], hull[i], false, true);
									//绘制凸包
									drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
								}


								//多边形拟合进行模板匹配操作
								vector<Point> inputcurve = hull[0];
								vector<Point>outputcurve;
								approxPolyDP(inputcurve, outputcurve, 100, true);
								Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
								drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);


								saveangle(outputcurve);//存储内夹角
								//SaveSide(outputcurve);//找到基准边
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
										Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
										EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
										EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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
								Log("output错误");
								//if (output[0].id == 1)
								//{
								//	DirectionConvexityDefects(contoursENDMoban);
								//}

								//对目标轮廓进行凸包操作
								vector<vector<Point>>hull(contoursENDMoban.size());
								Mat dst = Mat::zeros(img.size(), CV_8UC1);
								for (int i = 0; i < contoursENDMoban.size(); i++)
								{
									convexHull(contoursENDMoban[i], hull[i], false, true);
									//绘制凸包
									drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
								}


								//多边形拟合进行模板匹配操作
								vector<Point> inputcurve = hull[0];
								vector<Point>outputcurve;
								approxPolyDP(inputcurve, outputcurve, 100, true);
								Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
								drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);


								saveangle(outputcurve);//存储内夹角
								//SaveSide(outputcurve);//找到基准边

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
										Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
										EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
										EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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

					//针对衣服刚好处在传送带中心
					else if (Farpoints.size() == 0)
					{
						//cout << contourArea(contoursDingwei[MinIdx]) << endl;
						if (contourArea(contoursDingwei[MinIdx]) < Clothes_Area[0])   //用来判别是否存在两件衣服  220000  250000
						{

							vector<vector<Point>>contoursENDMoban;
							vector<Vec4i>hierarchyENDMoban;
							findContours(Target, contoursENDMoban, hierarchyENDMoban, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

							vector<vector<Point>>hull(contoursENDMoban.size());
							Mat dst = Mat::zeros(img.size(), CV_8UC1);
							for (int i = 0; i < contoursENDMoban.size(); i++)
							{
								convexHull(contoursENDMoban[i], hull[i], false, true);
								//绘制凸包
								drawContours(dst, hull, i, Scalar(255), 2, 8, Mat());
							}

							//多边形拟合进行模板匹配操作
							vector<Point> inputcurve = hull[0];
							vector<Point>outputcurve;
							approxPolyDP(inputcurve, outputcurve, 100, true);

							Mat OUTCON = Mat::zeros(img.size(), CV_8UC1);
							drawContours(OUTCON, vector<vector<Point>>{outputcurve}, 0, Scalar(255), 2);

							saveangle(outputcurve);//存储内夹角
							//SaveSide(outputcurve);//找到基准边

							if (outputcurve.size() >= 4)
							{

								int num = 0;

								if (num == 0)
								{

									calminAreaRect(Mat(contoursENDMoban[0]));
									Moments m = moments(contoursENDMoban[0], true);
									Point2f center(m.m10 / m.m00, m.m01 / m.m00);//此为重心
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
									EndMaxdis[0] = Maxdis[0] * XYscale[0];//存储矩形长边
									EndMindis[0] = Mindis[0] * XYscale[0];//存储矩形短边
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
	Log("检测结果:", results);
	clock_t detectfinish = clock();  // 记录程序结束时间
	double detecttime_length = (double)(detectfinish - detectstart) / CLOCKS_PER_SEC; //根据两个时刻的差，计算出处理的时间 
	Log("图像检测处理耗时:", detecttime_length, "ms");
	return img;
}

// 数据发送
void BaslerCamera::sendData()
{
	clock_t plcstart = clock(); // 记录程序开始时间
	ClothCount++;
	Log("---------------------第", ClothCount, "件--------------------");
	int clothesSN[1] = { ClothCount };

	Log("发送数据时,PLC连接状态", plcresult);
	if (plcresult)
	{
		plc.PLCWrite(0xA8, &clothesSN[0], 1, 4031);//记录第多少件衣服
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
				Log("成功发送第", ClothCount, "件定位数据");
			}
			else
			{
				Log("发送第", ClothCount, "件定位数据失败");
			}
			//plc.PLCWrite(0xA8, &FinalpointX[0], 1, 4022);//发送定位标志
			//plc.PLCWrite(0xA8, &FinalpointY[0], 1, 4024);//发送定位标志
			//plc.PLCWrite(0xA8, &Finalangle[0], 1, 4026);//发送定位标志
			//plc.PLCWrite(0xA8, &EndMaxdis[0], 1, 4032);//发送矩形长度
			//plc.PLCWrite(0xA8, &EndMindis[0], 1, 4034);//发送矩形宽度
			//int OK[1] = { 1 };
			//plc.PLCWrite(0xA8, &OK[0], 1, 4020);//发送扫码标志
		}
		Log("成功处理第", ClothCount, "件", " ", "中心点X:", FinalpointX[0], " ", "中心点Y:", FinalpointY[0], " ", "倾斜角 : ", Finalangle[0]);
	}
	else
	{
		if (plcresult)
		{
			int OK[1] = { 2 };
			int w = plc.PLCWrite(0xA8, &OK[0], 1, 4040);//发送扫码标志
		}
		Log("未成功处理第", ClothCount, "件");
	}
	clock_t plcfinish = clock();  // 记录程序结束时间
	double plctime_length = (double)(plcfinish - plcstart) / CLOCKS_PER_SEC; //根据两个时刻的差，计算出处理的时间 
	Log("PLC发送数据处理耗时:", plctime_length, "ms");
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

	// pylonimg转opencvimg初始化
	Pylon::CImageFormatConverter formatConverter;//新建pylon ImageFormatConverter对象
	formatConverter.OutputPixelFormat = Pylon::PixelType_BGR8packed;//确定输出像素格式
	Pylon::CPylonImage pylonImage;//创建一个Pylonlmage后续将用来创建OpenCV images

	while (!stopState)
	{
		try
		{
			while (mCamera->IsGrabbing())
			{
				// 等待接收和恢复图像，超时时间设置为5000 ms
				mCamera->RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
				// 如果图像抓取成功
				if (ptrGrabResult->GrabSucceeded())
				{
					clock_t start = clock(); // 记录程序开始时间
					// 添加图像处理代码
					formatConverter.Convert(pylonImage, ptrGrabResult);//将抓取的缓冲数据转化成pylon image.
					openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());//将 pylon image转成OpenCV image.

					// 图像处理
					Mat img = imread("E:/1113.png");

					////添加PLC意外断开重连机制
					//if (!plc.PLCWrite(0xA8, &plcnum[0], 1, 4035))
					//{
					//	plcCLnum++;
					//	Log("正在进行第", plcCLnum,"次PLC断开重连！");
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
							//clock_t detectstart = clock(); // 记录程序开始时间

							resultimg = Detect(img);

							//clock_t detectfinish = clock();  // 记录程序结束时间
							//double detecttime_length = (double)(detectfinish - detectstart) / CLOCKS_PER_SEC; //根据两个时刻的差，计算出处理的时间 
							//Log("图像检测处理耗时:", detecttime_length, "ms");
							sendData();
							// 图像保存
							if (plcresult)
							{
								if (results)
								{
									int OK = 1;
									//保存图像
									SaveImage(resultimg, OK);
								}
								else
								{
									int OK = 0;
									//保存图像
									SaveImage(resultimg, OK);
								}
							}
							clock_t finish = clock();  // 记录程序结束时间
							double time_length = (double)(finish - start) / CLOCKS_PER_SEC; //根据两个时刻的差，计算出处理的时间 
							Log("处理耗时:", time_length, "ms");
							Log("-----------------------------------------------------------------------------------");
							IfWritePLC = false;
						//}
						//else
						//{
						//	resultimg = openCvImage;
						//}
					}
					// 创建QImage对象
					image = MatToQImage(resultimg);
					// 发出图像显示信号
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


