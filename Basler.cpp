#pragma execution_character_set("utf-8")
#include "Basler.h"
#include <QDebug>
#include "ui_Basler.h"
#include <QMetaType>
#include <QTimer>
#include <QRegularExpression>
#include <bitset>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Camera.h"
#include <vector>
#include <fstream>

using namespace std;
using namespace cv;
using namespace dnn;
QString srcDirPathOK;
QString srcDirPathLog;
QString srcDirPathNG;

extern std::vector<cv::Point2f>ENDpoint;//存储最终中心点  (extern表明是在外部定义此类型  可全局公用）
extern std::vector<double>ENDAngle;//存储最终角度
extern std::vector<int>direnum;//存储方向
extern int FinalpointX[1];//存储最终中心点的X坐标
extern int FinalpointY[1];//存储最终中心点的Y坐标
extern int Finalangle[1];//存储最终中心点的Y坐标
extern int Maxdis[1];//存储矩形长边
extern int Mindis[1];//存储矩形短边
extern int EndMaxdis[1];//存储矩形长边
extern int EndMindis[1];//存储矩形短边
extern float showpointX[1];
extern float showpointY[1];
extern float showangle[1];
extern std::vector<cv::Point>imagecenter;
extern int ClothCount;//初始化衣服数量  
bool plcresult;
bool BiaoDingresult;
bool Shizhiresult;
bool RLUDresult;
extern bool results;

extern int Data[12];//存储一次性传入参数

std::vector<float>XYscale;//存储xy的缩放因子  0-2
std::vector<float>XXDvalue;//存储线消抖参数 
std::vector<float>EXPvalue;//存储曝光参数 
std::vector<float>RLDistance;//存储图像左右两侧间距
std::vector<float>UDDistance;//存储图像左右两侧间距
std::vector<float>Clothes_Distance;//存储两件衣服间隔
std::vector<float>Clothes_Area;//存储衣服面积进行判断
std::vector<int>Modeindex;//存储触发模式索引
std::vector<int>Sourceindex;//存储触发源索引

void Basler::CheckAndCleanFolders(QString path)
{
	// 创建一个QDir对象dir，用于操作指定路径path的文件和目录
	QDir dir(path);
	// 获取当前的日期和时间
	QDateTime currentDate = QDateTime::currentDateTime();
	// 获取指定路径下的所有子目录列表，不包括"."和".."这两个特殊目录
	QStringList folderList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	// 遍历获取到的文件夹列表
	for (const QString& folderName : folderList)
	{
		// 将文件夹名称以"-"为分隔符进行拆分
		QStringList parts = folderName.split('-');
		// 检查拆分后的部分是否正好为4个（预期格式：月-日-其他部分）

		if (parts.size() == 2)
		{
			// 提取文件夹名称中的月份和日期部分转换为整数
			int folderMonth = parts[0].toInt();
			int folderDay = parts[1].toInt();
			// 使用当前日期的年份，与提取出的月份和日期构造一个QDate对象表示文件夹的日期
			QDate folderDate(currentDate.date().year(), folderMonth, folderDay);
			// 计算文件夹日期与当前日期之间的天数差
			int daysDiff = folderDate.daysTo(currentDate.date());

			// 如果日期差距大于等于保存时间（SaveTime）设定的天数

			if (daysDiff >= SaveTime)
			{
				// 构造完整的要删除的文件夹路径
				QString folderPath = QString("%1/%2").arg(path).arg(folderName);
				// 使用QDir对象删除该文件夹及其包含的所有子文件和子目录
				QDir(folderPath).removeRecursively();
			}
		}
	}
}
void Basler::CheckAndCleanFiles(QString path)
{
	QDir dir(path);
	// 获取当前日期
	QDateTime currentDate = QDateTime::currentDateTime();
	// 获取文件列表
	QStringList fileList = dir.entryList(QStringList() << "*.txt", QDir::Files | QDir::NoDotAndDotDot);

	// 遍历文件列表
	for (const QString& fileName : fileList)
	{
		// 解析文件名
		QStringList parts = fileName.split('-');
		if (parts.size() == 3)
		{
			int fileYear = parts[0].toInt();
			int fileMonth = parts[1].toInt();
			int fileDay = parts[2].split('.').first().toInt();
			// 构造文件日期
			QDate fileDate(fileYear, fileMonth, fileDay);
			// 计算日期差距
			int daysDiff = fileDate.daysTo(currentDate.date());
			// 如果日期差距超过指定天数，则删除文件
			if (daysDiff >= SaveTime)
			{
				QString filePath = QString("%1/%2").arg(path).arg(fileName);
				QFile(filePath).remove();
			}
		}
	}
}
//保存相机曝光时间
void Basler::saveSettings(const std::string& filename)
{

	std::ofstream file(filename);
	if (file.is_open())
	{
		file << EXPvalue[0];
		file.close();
	}
	else {
		std::cerr << "Unable to save settings to file." << std::endl;
	}
}
//保存程序其它设置
void Basler::saveOtherSettings(const std::string& filename)
{
	std::ofstream file(filename);

	if (file.is_open())
	{
		// 假设RLDistance和UDDistance都是与EXPvalue相同类型的数组

			file << XXDvalue[0] << std::endl; // 将XXDvalue[0]写入第1行
			file << XYscale[0] << std::endl; // 将XYscale[0]写入第2行
			file << RLDistance[0] << std::endl; // 将RLDistance[0]写入第3行
			file << UDDistance[0] << std::endl; // 将UDDistance[0]写入第4行
			file << Clothes_Distance[0] << std::endl; // 将Clothes_Distance[0]写入第5行
			file << Clothes_Area[0] << std::endl; // 将Clothes_Area[0]写入第6行


		file.close();
	}
	else {
		std::cerr << "Unable to save settings to file." << std::endl;
	}
}

// 从文件加载曝光时间
void Basler::loadSettings(const std::string& filename)
{
	std::ifstream file(filename);
	if (file.is_open())
	{
		// 检查文件是否为空
		if (file.peek() == std::ifstream::traits_type::eof())
		{

			// 文件为空，写入默认值9000
			file.close();
			std::ofstream outFile(filename);
			if (outFile.is_open())
			{
				outFile << 9000;
				outFile.close();

				// 重新打开文件以读取默认值并赋值给exposureTime
				std::ifstream inFile(filename);
				if (inFile.is_open())
				{
					inFile >> exposureTime;
					inFile.close();
				}
				else
				{
					std::cerr << "无法从文件中读取默认值。" << std::endl;
				}
			}
			else
			{
				std::cerr << "无法将默认值写入文件。" << std::endl;
			}
		}
		else
		{

			// 文件不为空，读取数据并赋值给exposureTime
			file >> exposureTime;
			file.close();
		}
	}
	else {
		std::cerr << "无法从文件加载设置。使用默认值。" << std::endl;
	}
}

// 从文件加载其它程序设置
void Basler::loadOtherSettings(const std::string& filename)
{
	std::ifstream file(filename);

	if (file.is_open())
	{
		if (file.peek() == std::ifstream::traits_type::eof())
		{
			// 处理文件为空的情况，写入默认值
			file.close();

			std::ofstream outFile(filename);
			if (outFile.is_open())
			{
				// 写入默认值
				outFile << 2000 << std::endl;
				outFile << 1.0 << std::endl;
				outFile << 100.0 << std::endl;
				outFile << 100.0 << std::endl;
				outFile << 100.0 << std::endl;
				outFile << 300000.0 << std::endl;
				outFile.close();

				// 重新打开文件以读取刚写入的默认值
				std::ifstream inFile(filename);
				if (inFile.is_open())
				{
					// 分别读取六行数据
					inFile >> XXD;
					inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 跳过分隔行
					inFile >> Scale;
					inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					inFile >> RL;
					inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					inFile >> UD;
					inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					inFile >> Dis;
					inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					inFile >> Area;
					inFile.close();

					// 因为您只需要写入/读取前三行，所以后续的UD, Dis, Area 不需要处理默认值
				}
				else
				{
					std::cerr << "无法从文件中读取默认值。" << std::endl;
				}
			}
			else
			{
				std::cerr << "无法将默认值写入文件。" << std::endl;
			}
		}
		else
		{
			// 分别读取六行数据
			file >> XXD;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 跳过分隔行
			file >> Scale;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> RL;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> UD;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> Dis;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> Area;

			// 关闭文件
			file.close();
		}
	}
	else {
		std::cerr << "无法从文件加载设置。" << std::endl;
	}
}

Basler::Basler(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_control = new BaslerCamera();

	ui.comboBox_mode->addItem("Off");//关闭触发模式
	ui.comboBox_mode->addItem("On");//打开触发模式

	ui.comboBox_trigsrc->addItem("Software");//添加软件触发
	ui.comboBox_trigsrc->addItem("Line1");//添加线触发

	loadSettings("D:/Save/camera.txt");
	loadOtherSettings("D:/Save/Othercamera.txt");

	// 通讯定时
	QTimer* timer = new QTimer(this);
	connect(ui.pushButton_ConnectPLC, &QPushButton::clicked, [=]()
		{
			timer->start(500);
			connect(timer, &QTimer::timeout, this, [=]()
				{
					if (plcresult)
					{
						int OK[1] = { 1 };
						m_control->plc.PLCWrite(0xA8, &OK[0], 1, 4030);//扫码软件开启状态
					}
				});
		});

	connect(ui.pushButton_ConnectPLC, &QPushButton::clicked, [=]()
		{
			timer->start(500);
			connect(timer, &QTimer::timeout, this, [=]()
				{
					if (plcresult)
					{
						int recv_data[1];
						int recv_size;
						m_control->plc.PLCRead(0xA8, 4000, 1, recv_data, recv_size);

						if (recv_data[0] == 1)
						{
							int OK[1] = { 0 };
							m_control->plc.PLCWrite(0xA8, &OK[0], 1, 4000);//置零
							m_control->SetCmd("TriggerSoftware");
						}
					}
				});
		});

	connect(ui.comboBox_trigsrc, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
		QSettings settings("Mysetting", QSettings::IniFormat);
		settings.setValue("comboBoxSourceIndex", index);
		});

	connect(ui.comboBox_mode, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
		QSettings settings("Mysetting", QSettings::IniFormat);
		settings.setValue("comboBoxModeIndex", index);
		});

	// 读取上一次保存的值
	QSettings settings("Mysetting", QSettings::IniFormat);

	// 在程序启动时恢复QSlider的值
	int EXPValue = settings.value("sliderValue").toInt();
	if (EXPValue != -1)
	{
		ui.horizontalSliderExp->setValue(EXPValue);
		m_control->SetIntPara("ExposureTimeRaw", EXPValue);
		EXPvalue.clear();
		EXPvalue.push_back(EXPValue);
	}

	// 在程序启动时恢复QSpinBox的值
	int savedSpinBoxEXP = settings.value("spinBoxValue").toInt();
	if (savedSpinBoxEXP != -1)
	{
		ui.spinBoxExp->setValue(savedSpinBoxEXP);
		m_control->SetIntPara("ExposureTimeRaw", savedSpinBoxEXP);
		EXPvalue.clear();
		EXPvalue.push_back(EXPValue);
	}

	QString IPValue = settings.value("IPValue").toString();
	if (!IPValue.isEmpty())
	{
		ui.IP->setText(IPValue);
	}
	QString portValue = settings.value("portValue").toString();
	if (!portValue.isEmpty())
	{
		ui.port->setText(portValue);
	}
	QString OKpath = settings.value("OKpath").toString();

	if (!OKpath.isEmpty())
	{
		srcDirPathOK = OKpath;
		ui.OK_lineEdit->setText(OKpath);
	}
	QString NGpath = settings.value("NGpath").toString();
	if (!NGpath.isEmpty())
	{
		srcDirPathNG = NGpath;
		ui.NG_lineEdit->setText(NGpath);
	}
	QString Logpath = settings.value("Logpath").toString();
	if (!Logpath.isEmpty())
	{
		srcDirPathLog = Logpath;
		ui.Log_lineEdit->setText(Logpath);
	}
	QString TimeValue = settings.value("TimeValue").toString();
	if (!TimeValue.isEmpty())
	{
		ui.SaveTime_lineEdit->setText(TimeValue);
		triggerCheckAndCleanFolders();
	}

	//double savedDoubleSpinBoxValue = settings.value("LastDoubleSpinBoxValue").toDouble();
	//if (savedDoubleSpinBoxValue != 0.0)
	//{
	//	ui.ScaleBox->setValue(savedDoubleSpinBoxValue);
	//	XYscale.push_back(savedDoubleSpinBoxValue);
	//}

	//double savedXXDValue = settings.value("LastXXDValue").toDouble();
	//if (savedXXDValue != 0.0)
	//{
	//	ui.XXD->setValue(savedXXDValue);
	//	XXDvalue.push_back(savedXXDValue);
	//}

	//double savedRLValue = settings.value("LastRLValue").toDouble();
	//if (savedRLValue != -1)
	//{
	//	ui.doubleSpinBox_RL->setValue(savedRLValue);
	//	RLDistance.push_back(savedRLValue);
	//}

	//double savedUDValue = settings.value("LastUDValue").toDouble();
	//if (savedUDValue != -1)
	//{
	//	ui.doubleSpinBox_UD->setValue(savedUDValue);
	//	UDDistance.push_back(savedUDValue);
	//}

	//double savedClthesDisValue = settings.value("LastClothesDisValue").toDouble();
	//if (savedClthesDisValue != -1)
	//{
	//	ui.Clothes_Dis->setValue(savedClthesDisValue);
	//	Clothes_Distance.push_back(savedClthesDisValue);
	//}

	//double savedClthesAreaValue = settings.value("LastClothesAreaValue").toDouble();
	//if (savedClthesAreaValue != -1)
	//{
	//	ui.doubleSpinBox_Area->setValue(savedClthesAreaValue);
	//	Clothes_Area.push_back(savedClthesAreaValue);
	//}

	int savedSourceIndex = settings.value("comboBoxSourceIndex").toInt();
	if (savedSourceIndex != 999)
	{
		ui.comboBox_trigsrc->setCurrentIndex(savedSourceIndex);
		if (savedSourceIndex == 0)
		{
			Sourceindex.push_back(0);
		}
		if (savedSourceIndex == 1)
		{
			Sourceindex.push_back(1);
		}
	}

	int savedModeIndex = settings.value("comboBoxModeIndex").toInt();
	if (savedModeIndex != 999)
	{
		ui.comboBox_mode->setCurrentIndex(savedModeIndex);
		if (savedModeIndex == 0)
		{
			Modeindex.push_back(0);
		}
		if (savedModeIndex == 1)
		{
			Modeindex.push_back(1);
		}
	}

	connect(m_control, SIGNAL(showImageSignal(QImage)), this, SLOT(showImage(QImage)));
	connect(m_control, &BaslerCamera::baslerConditionMet, this, &Basler::BaslerConditionMet);

	ui.pushButtonOpenCam->setEnabled(true);
	ui.pushButtonStartCam->setEnabled(false);
	ui.pushButtonSoftTrig->setEnabled(false);
	ui.pushButtonStopCam->setEnabled(false);
	ui.pushButtonCloseCam->setEnabled(false);
	ui.push_close->setEnabled(true);
	ui.pushButton_ConnectPLC->setEnabled(true);
	ui.pushButton_DisConnectPLC->setEnabled(true);

	EXPvalue.clear();
	EXPvalue.push_back(exposureTime);
	ui.spinBoxExp->setValue(exposureTime);
	ui.horizontalSliderExp->setValue(exposureTime);

	XXDvalue.clear();
	XXDvalue.push_back(XXD);
	ui.XXD->setValue(XXDvalue[0]);

	XYscale.clear();
	XYscale.push_back(Scale);
	ui.ScaleBox->setValue(XYscale[0]);

	RLDistance.clear();
	RLDistance.push_back(RL);
	ui.doubleSpinBox_RL->setValue(RLDistance[0]);

	UDDistance.clear();
	UDDistance.push_back(UD);
	ui.doubleSpinBox_UD->setValue(UDDistance[0]);

	Clothes_Distance.clear();
	Clothes_Distance.push_back(Dis);
	ui.Clothes_Dis->setValue(Clothes_Distance[0]);

	Clothes_Area.clear();
	Clothes_Area.push_back(Area);
	ui.doubleSpinBox_Area->setValue(Clothes_Area[0]);

	//triggerCheckAndCleanFolders();
}

Basler::~Basler()
{


	// 保存当前输入的值
	QSettings settings("Mysetting", QSettings::IniFormat);


	QString currentIPValue = ui.IP->text();
	settings.setValue("IPValue", currentIPValue);
	QString currentportValue = ui.port->text();
	settings.setValue("portValue", currentportValue);


	QString currentOKpath = ui.OK_lineEdit->text();
	settings.setValue("OKpath", currentOKpath);
	QString currentNGpath = ui.NG_lineEdit->text();
	settings.setValue("NGpath", currentNGpath);
	QString currentLogpath = ui.Log_lineEdit->text();
	settings.setValue("Logpath", currentLogpath);
	QString currentTimeValue = ui.SaveTime_lineEdit->text();
	settings.setValue("TimeValue", currentTimeValue);

	double currentDoubleSpinBoxValue = ui.ScaleBox->value(); // 获取当前的值
	settings.setValue("LastDoubleSpinBoxValue", currentDoubleSpinBoxValue); // 将值保存到设置中
	double currentXXDValue = ui.XXD->value(); // 获取当前的值
	settings.setValue("LastXXDValue", currentXXDValue); // 将值保存到设置中

	double currentRL = ui.doubleSpinBox_RL->value(); // 获取当前的值
	settings.setValue("LastRLValue", currentRL); // 将值保存到设置中
	double currentUD = ui.doubleSpinBox_UD->value(); // 获取当前的值
	settings.setValue("LastUDValue", currentUD); // 将值保存到设置中

	double currentClothesDis = ui.Clothes_Dis->value(); // 获取当前的值
	settings.setValue("LastClothesDisValue", currentClothesDis); // 将值保存到设置中

	double currentClothesArea = ui.doubleSpinBox_Area->value(); // 获取当前的值
	settings.setValue("LastClothesAreaValue", currentClothesArea); // 将值保存到设置中

	int currentSourceIndex = ui.comboBox_trigsrc->currentIndex();//触发源
	settings.setValue("comboBoxSourceIndex", currentSourceIndex);

	int currentModeIndex = ui.comboBox_mode->currentIndex();//触发模式
	settings.setValue("comboBoxModeIndex", currentModeIndex);

	// 保存QSlider的值
	int currentEXP = ui.horizontalSliderExp->value();
	settings.setValue("sliderValue", currentEXP);

	// 保存QSpinBox的值
	int spinBoxEXP = ui.spinBoxExp->value();
	settings.setValue("spinBoxValue", spinBoxEXP);

	saveSettings("D:/Save/camera.txt");
	saveOtherSettings("D:/Save/Othercamera.txt");

	Modeindex.clear();
	Sourceindex.clear();

	delete m_control, m_control = nullptr;
}

//修改曝光 滑动
void Basler::on_horizontalSliderExp_valueChanged(const int& value)
{
	m_control->SetIntPara("ExposureTimeRaw", value);
	ui.spinBoxExp->setValue(value);
	EXPvalue.clear();
	EXPvalue.push_back(value);
	on_spinBoxExp_editingFinished();
}

//修改曝光 微调
void Basler::on_spinBoxExp_editingFinished()
{
	int tmp = ui.spinBoxExp->value();
	m_control->SetIntPara("ExposureTimeRaw", tmp);
	ui.horizontalSliderExp->setValue(tmp);
	EXPvalue.clear();
	EXPvalue.push_back(tmp);
}

//修改比例缩放因子
void Basler::on_ScaleBox_editingFinished()
{
	float scale = ui.ScaleBox->value();
	XYscale.clear();
	XYscale.push_back(scale);
}
void Basler::on_RL_editingFinished()//图像左右两侧间距
{
	float RLdis = ui.doubleSpinBox_RL->value();
	RLDistance.clear();
	RLDistance.push_back(RLdis);
}

void Basler::on_UD_editingFinished()//图像上下两侧间距
{
	float UDdis = ui.doubleSpinBox_UD->value();
	UDDistance.clear();
	UDDistance.push_back(UDdis);
}

void Basler::on_ClothesDis_editingFinished()//获取两件衣服间隔
{
	float Clothes_distance = ui.Clothes_Dis->value();
	Clothes_Distance.clear();
	Clothes_Distance.push_back(Clothes_distance);
	cout << Clothes_Distance[0] << endl;
}

void Basler::on_ClothesArea_editingFinished()//获取衣服面积大小
{
	float Clothes_area = ui.doubleSpinBox_Area->value();
	Clothes_Area.clear();
	Clothes_Area.push_back(Clothes_area);
	cout << Clothes_Area[0] << endl;
}

//相机线消抖时间设置
void Basler::Xianxiaodou_editingFinished()
{
	XXDvalue.clear();
	int value = ui.XXD->value();
	XXDvalue.push_back(value);
}
//获取触发模式
void Basler::on_comboBox_mode_activated(const QString& arg1)//将qstring类型转换成c++的string。在获取c的字符指针
{
	m_control->SetStringPara("TriggerMode", arg1.toStdString().c_str());
	std::string modeStr = arg1.toStdString();
	if (modeStr == "On")
	{
		Modeindex.clear();
		Modeindex.push_back(1);
	}
	if (modeStr == "Off")
	{
		Modeindex.clear();
		Modeindex.push_back(0);
	}
}

//获取触发源
void Basler::on_comboBox_trigsrc_activated(const QString& arg1)//将qstring类型转换成c++的string。在获取c的字符指针
{
	m_control->SetStringPara("TriggerSource", arg1.toStdString().c_str());
	std::string sourceStr = arg1.toStdString();
	if (sourceStr == "Software")
	{
		Sourceindex.clear();
		Sourceindex.push_back(0);
	}
	if (sourceStr == "Line1")
	{
		Sourceindex.clear();
		Sourceindex.push_back(1);
	}
}


//判断IP合法性
bool Basler::isIPValid(const QString& ip)
{
	QRegularExpression ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
	QRegularExpressionMatch match = ipRegex.match(ip);
	return match.hasMatch();
}

//获取IP地址
QString Basler::IP_change()
{
	//获取lineedit数据
	QString IP = ui.IP->text();
	if (isIPValid(IP))
	{
		//qDebug() << "IP地址有效";
		// IP地址合法
		ui.IP->setStyleSheet("QLineEdit { color: black; }");
		ui.IP->setStyleSheet("QLineEdit{border:1px solid black }");
	}
	else {
		/*qDebug() << "IP地址无效";*/
		// IP地址不合法
		ui.IP->setStyleSheet("QLineEdit { color: red; }");
	}
	return IP;
}

//获取端口号
int Basler::port_change()
{
	//获取lineedit数据
	QString port_s = ui.port->text();
	int port_i = port_s.toInt();
	if (port_i <= 0)
	{
		ui.port->setStyleSheet("QLineEdit { color: red; }");
	}
	else
		ui.port->setStyleSheet("QLineEdit{border:1px solid black }");
	return port_i;
}



//连接PLC
void Basler::pushButton_ConnectPLC_clicked()
{
	QString IP_Q = IP_change();//获取IP
	//QString转换为 const char*
	QByteArray byteArray1 = IP_Q.toUtf8();
	const char* IP_c = byteArray1.constData();

	int port = port_change();//获取端口号

	if (IP_Q.isEmpty() || port <= 0)
	{
		if (IP_Q.isEmpty())
		{
			// 设置LineEdit变为红色
			ui.IP->setStyleSheet("QLineEdit{border:1px solid red }");
		}
		if (port <= 0)
		{
			// 设置LineEdit变为红色
			ui.port->setStyleSheet("QLineEdit{border:1px solid red }");
		}
		QMessageBox::warning(this, "警告", "IP或端口号非法!");
		return;
	}
	m_control->plc.Startup();


	plcresult = m_control->plc.Open(IP_c, port, 500);

	if (!plcresult)
	{
		QMessageBox::critical(0, "错误", "无法连接到 PLC!");
		return;
	}

	if (plcresult)
	{
		QMessageBox::information(0, "正确", "连接 PLC成功!");
		m_control->plc.InitParam();
		int OK[1] = { 0 };
		m_control->plc.PLCWrite(0xA8, &OK[0], 1, 4020);//初始为0
		int OK1[15] = { 0 };
		m_control->plc.PLCWrite(0xA8, &OK1[0], 6, 4022);//初始为0
	}
}

void Basler::BaslerConditionMet()
{

	QString IP_Q = IP_change();//获取IP
	//QString转换为 const char*
	QByteArray byteArray1 = IP_Q.toUtf8();
	const char* IP_c = byteArray1.constData();

	int port = port_change();//获取端口号

	if (IP_Q.isEmpty() || port <= 0)
	{
		if (IP_Q.isEmpty())
		{
			// 设置LineEdit变为红色
			ui.IP->setStyleSheet("QLineEdit{border:1px solid red }");
		}
		if (port <= 0)
		{
			// 设置LineEdit变为红色
			ui.port->setStyleSheet("QLineEdit{border:1px solid red }");
		}
		return;
	}
	m_control->plc.Close();
	m_control->plc.Startup();
	plcresult = m_control->plc.Open(IP_c, port, 500);

	//if (plcresult)
	//{
	//	m_control->plc.InitParam();
	//}

}
//断开连接
void Basler::pushButton_DisConnectPLC_clicked()
{
	plcresult = false;
	m_control->plc.Close();
	if (m_control->plc.IsOpen()) {
		QMessageBox::warning(0, "警告", "断开PLC 失败!");
		// 未成功关闭连接，处理错误
		qDebug() << "Failed to disconnect PLC.";
	}
	else
	{
		QMessageBox::information(0, "成功", "断开PLC 成功!");
		// 成功关闭连接，释放资源
		// 释放其他资源或进行其他必要的清理工作
		qDebug() << "PLC disconnected successfully.";
	}
}
//打开相机
void Basler::pushButton_Opencamera_clicked()
{
	if (m_control->OpenCamera())
	{
		ui.pushButtonCloseCam->setEnabled(true);
		ui.pushButtonSoftTrig->setEnabled(true);
		ui.pushButtonStartCam->setEnabled(true);
		ui.pushButtonStopCam->setEnabled(false);
		on_horizontalSliderExp_valueChanged(EXPvalue[0]);
		INT64 exp;
		m_control->GetIntPara("ExposureTimeRaw", exp);
		//ui.spinBoxExp->setValue(exp);
		//ui.horizontalSliderExp->setValue(exp);
		ui.spinBoxExp->setValue(EXPvalue[0]);
		ui.horizontalSliderExp->setValue(EXPvalue[0]);
		std::string tmp;
		m_control->GetStringPara("TriggerSource", tmp);
		//qDebug() << tmp.c_str();
		int index = ui.comboBox_trigsrc->findText(tmp.c_str());

		if (Sourceindex[0] == 0)
		{
			on_comboBox_trigsrc_activated("Software");
		}
		if (Sourceindex[0] == 1)
		{
			on_comboBox_trigsrc_activated("Line1");
		}
		//ui.comboBox_trigsrc->setCurrentIndex(index);
		m_control->GetStringPara("TriggerMode", tmp);

		index = ui.comboBox_mode->findText(tmp.c_str());
		if (Modeindex[0] == 0)
		{
			on_comboBox_mode_activated("Off");
		}
		if (Modeindex[0] == 1)
		{
			on_comboBox_mode_activated("On");
		}
		//ui.comboBox_mode->setCurrentIndex(index);


	}

}

//显示图像中心点（标定点）
void Basler::Show_Biaoding()
{
	BiaoDingresult = true;
}
void Basler::Com_Biaoding()
{
	BiaoDingresult = false;
}
//显示图像十字开关
void Basler::Open_xiangxian()
{
	Shizhiresult = true;
}
void Basler::Close_xiangxian()
{
	Shizhiresult = false;
}
//打开图像四周边界框按钮
void Basler::Open_RLUD()
{
	RLUDresult = true;
}
void Basler::Close_RLUD()
{
	RLUDresult = false;
}


//开始采集图像
void Basler::pushButton_Startgrab_clicked()
{
	m_control->start();
	ui.pushButtonStartCam->setEnabled(false);
	ui.pushButtonStopCam->setEnabled(true);
	ui.pushButtonCloseCam->setEnabled(false);
}

//停止采集图像
void Basler::pushButton_Stopgrab_clicked()
{
	m_control->StopCamera();
	// 等待一段时间
	Sleep(500);
	ui.pushButtonStartCam->setEnabled(true);
	ui.pushButtonStopCam->setEnabled(false);
	ui.pushButtonCloseCam->setEnabled(true);
}

//软件触发
void Basler::pushButton_Soft_clicked()
{
	m_control->SetCmd("TriggerSoftware");
}

//关闭相机
void Basler::pushButton_Closecamera_clicked()
{

	m_control->CloseCamera();
	ui.pushButtonCloseCam->setDisabled(true);
	ui.pushButtonSoftTrig->setDisabled(true);
	ui.pushButtonStartCam->setDisabled(true);
	ui.pushButtonStopCam->setDisabled(true);
	//XYscale.clear();
	//XXDvalue.clear();
	RLDistance.clear();
	UDDistance.clear();
}


//显示图像
void Basler::showImage(const QImage& dst)
{

	ui.label_KUZI->setPixmap(QPixmap::fromImage(dst).scaled(ui.label_KUZI->width(),
		ui.label_KUZI->height(),
		Qt::KeepAspectRatioByExpanding));
	ui.label_KUZI->setScaledContents(true);
	//// 设置 QLabel 的大小策略为 Ignored
	//ui.label_KUZI->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	//// 调整 QLabel 大小以适应图像大小
	//ui.label_KUZI->setFixedSize(dst.size());
	int clothesSN[1] = { ClothCount };
	QString clothes = QString::number(clothesSN[0]);
	ui.lineEdit_Number->setText(clothes);
	if (results)
	{
		if (!direnum.empty())
		{
			QString direction = QString::number(direnum[0]);
			ui.lineEdit_Direction->setText(direction);
			direnum.clear();
		}
		if (XYscale.empty())
		{
			QString QAngle = QString::number(Finalangle[0]);
			QString QPointX = QString("X : %1").arg(FinalpointX[0]);
			QString QPointY = QString("Y : %1").arg(FinalpointY[0]);
			ui.lineEdit_PointX->setText(QPointX);
			ui.lineEdit_PointY->setText(QPointY);
			ui.lineEdit_Angle->setText(QAngle);
		}
		if (!XYscale.empty())
		{
			QString QAngle = QString::number(Finalangle[0]);
			QString QPointX = QString("X : %1").arg(FinalpointX[0]);
			QString QPointY = QString("Y : %1").arg(FinalpointY[0]);
			ui.lineEdit_PointX->setText(QPointX);
			ui.lineEdit_PointY->setText(QPointY);
			ui.lineEdit_Angle->setText(QAngle);
		}
		if (Maxdis[0] != 0 && !XYscale.empty())
		{

			QString longdis = QString::number(Maxdis[0] * XYscale[0]);
			QString shortdis = QString::number(Mindis[0] * XYscale[0]);
			ui.lineEdit_long->setText(longdis);
			ui.lineEdit_short->setText(shortdis);
		}
		if (Maxdis[0] != 0 && XYscale.empty())
		{

			QString longdis = QString::number(Maxdis[0]);
			QString shortdis = QString::number(Mindis[0]);
			ui.lineEdit_long->setText(longdis);
			ui.lineEdit_short->setText(shortdis);
		}

		ENDAngle.clear();
		ENDpoint.clear();
		for (int i = 0; i < 1; ++i)
		{
			showangle[i] = 0;
			showpointX[i] = 0;
			showpointY[i] = 0;
			FinalpointX[i] = 0;
			FinalpointY[i] = 0;
			Finalangle[i] = 0;
			Maxdis[i] = 0;
			Mindis[i] = 0;
			EndMaxdis[i] = 0;
			EndMindis[i] = 0;
		}
		for (int i = 0; i < 11; ++i)
		{
			Data[i] = 0;
		}
	}
	else
	{
		ui.lineEdit_PointX->clear();
		ui.lineEdit_PointY->clear();
		ui.lineEdit_Angle->clear();
		ui.lineEdit_Direction->clear();
		ui.lineEdit_long->clear();
		ui.lineEdit_short->clear();
	}

	//if (!plcresult)
	//{
	//	onBaslerConditionMet();
	//}
}

void Basler::OKPath_pushButton_clicked()
{
	//文件夹路径
	srcDirPathOK = QFileDialog::getExistingDirectory(
		this, "choose src Directory", "/");
	if (srcDirPathOK.isEmpty())
	{
		return;
	}
	ui.OK_lineEdit->setText(srcDirPathOK);
}
void Basler::LogPath_pushButton_clicked()
{
	//文件夹路径
	srcDirPathLog = QFileDialog::getExistingDirectory(
		this, "choose src Directory", "/");
	if (srcDirPathLog.isEmpty())
	{
		return;
	}
	else
	{
		srcDirPathLog += "/";
		ui.Log_lineEdit->setText(srcDirPathLog);
	}
}
void Basler::NGPath_pushButton_clicked()
{
	//文件夹路径
	srcDirPathNG = QFileDialog::getExistingDirectory(
		this, "choose src Directory", "/");
	if (srcDirPathNG.isEmpty())
	{
		return;
	}
	ui.NG_lineEdit->setText(srcDirPathNG);
}
int Basler::SaveTime_lineEdit_Change()
{

	bool ok;
	//获取lineedit数据
	QString SaveTime_s = ui.SaveTime_lineEdit->text();
	int SaveTime_i = SaveTime_s.toInt(&ok);
	if (SaveTime_i <= 0)
	{
		ui.SaveTime_lineEdit->setStyleSheet("QLineEdit { color: red; }");
	}
	else
		ui.SaveTime_lineEdit->setStyleSheet("QLineEdit{border:1px solid black }");

	return SaveTime_i;
	
}
void Basler::triggerCheckAndCleanFolders()
{
	SaveTime = SaveTime_lineEdit_Change();
	// 检查并清理保存路径内的文件夹
	CheckAndCleanFolders(srcDirPathOK);
	CheckAndCleanFolders(srcDirPathNG);
	CheckAndCleanFiles(srcDirPathLog);
}

//退出程序
void Basler::pushButton_Exit_clicked()
{
	QMessageBox::StandardButton result = QMessageBox::question(this, "提示", "确定退出?", QMessageBox::Yes | QMessageBox::No);
	if (result == QMessageBox::Yes)
	{
		this->close();
	}
	else {}
}


