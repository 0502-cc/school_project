#pragma once
/*********************************************
*   说明：Basler相机sdk对接库。
**********************************************/
#ifndef _BASLER_CAMERA_H_
#define _BASLER_CAMERA_H_

#include <pylon/PylonIncludes.h>
#include <QThread>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <QString>
#include "yolov8_seg_onnx.h"
#include <QMessageBox>
#include <windows.h>
#include "ytplc_mitsubishi_mc.h"
#include "log.h"
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

using namespace std;
using namespace cv;

class BaslerCamera : public QThread
{
	Q_OBJECT

public:         //可以从任何地方访问
	BaslerCamera(void);
	~BaslerCamera(void);

	bool OpenCamera();//打开相机
	bool StartCamera();//开始采集
	void StopCamera();//关闭相机
	void CloseCamera();//停止采集	

	bool GetFloatPara(const char* nameNode, double& para);
	bool GetIntPara(const char* nameNode, int64_t& para);
	bool GetStringPara(const char* nameNode, std::string& para);
	bool SetFloatPara(const char* nameNode, const double& para);
	bool SetIntPara(const char* nameNode, INT64 para);
	bool SetStringPara(const char* nameNode, Pylon::String_t para);
	bool SetCmd(const char* name);
	bool stopState;
	QImage MatToQImage(const cv::Mat& mat);//将Opencv类型图片格式转为Qt类型
	void sendData();
	cv::Mat BaslerCamera::Detect(cv::Mat& img);
	void SaveImage(cv::Mat& img, int OK);
	void Connect();
	void Write();

	cv::Mat BaslerCamera::Dingwei(cv::Mat& img);//标定程序
protected:    //只能被该类的成员函数和派生类的成员函数访问
	void run();

private:     //只能被定义它们的类的成员函数访问
	std::mutex m_mutex;
	Pylon::CInstantCamera* mCamera;  //定义指针用于通信
	GenApi::INodeMap* nodemap;   //访问相机参数
	bool CameraRuning;   //判断相机是否开始采集图像
	bool CameraOpening; //判断相机是否打开
	Pylon::CGrabResultPtr ptrGrabResult;   //定义图像采集的结果

	cv::Mat openCvImage;//新建一个OpenCV image对象
	QImage image;
	cv::Mat resultimg;

	bool m_isExitConnectThread;
	HANDLE m_threadHandle;

public:
	YtPlc_Mitsubishi_Mc plc;
	int Choose;
	std::string model_path;
	Yolov8SegOnnx test;

signals:
	void showImageSignal(QImage);
	void baslerConditionMet();  // 定义一个信号
	//void ContinueWritePLC();
};

#endif