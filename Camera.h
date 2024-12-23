#pragma once
/*********************************************
*   ˵����Basler���sdk�Խӿ⡣
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

public:         //���Դ��κεط�����
	BaslerCamera(void);
	~BaslerCamera(void);

	bool OpenCamera();//�����
	bool StartCamera();//��ʼ�ɼ�
	void StopCamera();//�ر����
	void CloseCamera();//ֹͣ�ɼ�	

	bool GetFloatPara(const char* nameNode, double& para);
	bool GetIntPara(const char* nameNode, int64_t& para);
	bool GetStringPara(const char* nameNode, std::string& para);
	bool SetFloatPara(const char* nameNode, const double& para);
	bool SetIntPara(const char* nameNode, INT64 para);
	bool SetStringPara(const char* nameNode, Pylon::String_t para);
	bool SetCmd(const char* name);
	bool stopState;
	QImage MatToQImage(const cv::Mat& mat);//��Opencv����ͼƬ��ʽתΪQt����
	void sendData();
	cv::Mat BaslerCamera::Detect(cv::Mat& img);
	void SaveImage(cv::Mat& img, int OK);
	void Connect();
	void Write();

	cv::Mat BaslerCamera::Dingwei(cv::Mat& img);//�궨����
protected:    //ֻ�ܱ�����ĳ�Ա������������ĳ�Ա��������
	void run();

private:     //ֻ�ܱ��������ǵ���ĳ�Ա��������
	std::mutex m_mutex;
	Pylon::CInstantCamera* mCamera;  //����ָ������ͨ��
	GenApi::INodeMap* nodemap;   //�����������
	bool CameraRuning;   //�ж�����Ƿ�ʼ�ɼ�ͼ��
	bool CameraOpening; //�ж�����Ƿ��
	Pylon::CGrabResultPtr ptrGrabResult;   //����ͼ��ɼ��Ľ��

	cv::Mat openCvImage;//�½�һ��OpenCV image����
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
	void baslerConditionMet();  // ����һ���ź�
	//void ContinueWritePLC();
};

#endif