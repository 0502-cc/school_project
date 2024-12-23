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

extern std::vector<cv::Point2f>ENDpoint;//�洢�������ĵ�  (extern���������ⲿ���������  ��ȫ�ֹ��ã�
extern std::vector<double>ENDAngle;//�洢���սǶ�
extern std::vector<int>direnum;//�洢����
extern int FinalpointX[1];//�洢�������ĵ��X����
extern int FinalpointY[1];//�洢�������ĵ��Y����
extern int Finalangle[1];//�洢�������ĵ��Y����
extern int Maxdis[1];//�洢���γ���
extern int Mindis[1];//�洢���ζ̱�
extern int EndMaxdis[1];//�洢���γ���
extern int EndMindis[1];//�洢���ζ̱�
extern float showpointX[1];
extern float showpointY[1];
extern float showangle[1];
extern std::vector<cv::Point>imagecenter;
extern int ClothCount;//��ʼ���·�����  
bool plcresult;
bool BiaoDingresult;
bool Shizhiresult;
bool RLUDresult;
extern bool results;

extern int Data[12];//�洢һ���Դ������

std::vector<float>XYscale;//�洢xy����������  0-2
std::vector<float>XXDvalue;//�洢���������� 
std::vector<float>EXPvalue;//�洢�ع���� 
std::vector<float>RLDistance;//�洢ͼ������������
std::vector<float>UDDistance;//�洢ͼ������������
std::vector<float>Clothes_Distance;//�洢�����·����
std::vector<float>Clothes_Area;//�洢�·���������ж�
std::vector<int>Modeindex;//�洢����ģʽ����
std::vector<int>Sourceindex;//�洢����Դ����

void Basler::CheckAndCleanFolders(QString path)
{
	// ����һ��QDir����dir�����ڲ���ָ��·��path���ļ���Ŀ¼
	QDir dir(path);
	// ��ȡ��ǰ�����ں�ʱ��
	QDateTime currentDate = QDateTime::currentDateTime();
	// ��ȡָ��·���µ�������Ŀ¼�б�������"."��".."����������Ŀ¼
	QStringList folderList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	// ������ȡ�����ļ����б�
	for (const QString& folderName : folderList)
	{
		// ���ļ���������"-"Ϊ�ָ������в��
		QStringList parts = folderName.split('-');
		// ����ֺ�Ĳ����Ƿ�����Ϊ4����Ԥ�ڸ�ʽ����-��-�������֣�

		if (parts.size() == 2)
		{
			// ��ȡ�ļ��������е��·ݺ����ڲ���ת��Ϊ����
			int folderMonth = parts[0].toInt();
			int folderDay = parts[1].toInt();
			// ʹ�õ�ǰ���ڵ���ݣ�����ȡ�����·ݺ����ڹ���һ��QDate�����ʾ�ļ��е�����
			QDate folderDate(currentDate.date().year(), folderMonth, folderDay);
			// �����ļ��������뵱ǰ����֮���������
			int daysDiff = folderDate.daysTo(currentDate.date());

			// ������ڲ����ڵ��ڱ���ʱ�䣨SaveTime���趨������

			if (daysDiff >= SaveTime)
			{
				// ����������Ҫɾ�����ļ���·��
				QString folderPath = QString("%1/%2").arg(path).arg(folderName);
				// ʹ��QDir����ɾ�����ļ��м���������������ļ�����Ŀ¼
				QDir(folderPath).removeRecursively();
			}
		}
	}
}
void Basler::CheckAndCleanFiles(QString path)
{
	QDir dir(path);
	// ��ȡ��ǰ����
	QDateTime currentDate = QDateTime::currentDateTime();
	// ��ȡ�ļ��б�
	QStringList fileList = dir.entryList(QStringList() << "*.txt", QDir::Files | QDir::NoDotAndDotDot);

	// �����ļ��б�
	for (const QString& fileName : fileList)
	{
		// �����ļ���
		QStringList parts = fileName.split('-');
		if (parts.size() == 3)
		{
			int fileYear = parts[0].toInt();
			int fileMonth = parts[1].toInt();
			int fileDay = parts[2].split('.').first().toInt();
			// �����ļ�����
			QDate fileDate(fileYear, fileMonth, fileDay);
			// �������ڲ��
			int daysDiff = fileDate.daysTo(currentDate.date());
			// ������ڲ�೬��ָ����������ɾ���ļ�
			if (daysDiff >= SaveTime)
			{
				QString filePath = QString("%1/%2").arg(path).arg(fileName);
				QFile(filePath).remove();
			}
		}
	}
}
//��������ع�ʱ��
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
//���������������
void Basler::saveOtherSettings(const std::string& filename)
{
	std::ofstream file(filename);

	if (file.is_open())
	{
		// ����RLDistance��UDDistance������EXPvalue��ͬ���͵�����

			file << XXDvalue[0] << std::endl; // ��XXDvalue[0]д���1��
			file << XYscale[0] << std::endl; // ��XYscale[0]д���2��
			file << RLDistance[0] << std::endl; // ��RLDistance[0]д���3��
			file << UDDistance[0] << std::endl; // ��UDDistance[0]д���4��
			file << Clothes_Distance[0] << std::endl; // ��Clothes_Distance[0]д���5��
			file << Clothes_Area[0] << std::endl; // ��Clothes_Area[0]д���6��


		file.close();
	}
	else {
		std::cerr << "Unable to save settings to file." << std::endl;
	}
}

// ���ļ������ع�ʱ��
void Basler::loadSettings(const std::string& filename)
{
	std::ifstream file(filename);
	if (file.is_open())
	{
		// ����ļ��Ƿ�Ϊ��
		if (file.peek() == std::ifstream::traits_type::eof())
		{

			// �ļ�Ϊ�գ�д��Ĭ��ֵ9000
			file.close();
			std::ofstream outFile(filename);
			if (outFile.is_open())
			{
				outFile << 9000;
				outFile.close();

				// ���´��ļ��Զ�ȡĬ��ֵ����ֵ��exposureTime
				std::ifstream inFile(filename);
				if (inFile.is_open())
				{
					inFile >> exposureTime;
					inFile.close();
				}
				else
				{
					std::cerr << "�޷����ļ��ж�ȡĬ��ֵ��" << std::endl;
				}
			}
			else
			{
				std::cerr << "�޷���Ĭ��ֵд���ļ���" << std::endl;
			}
		}
		else
		{

			// �ļ���Ϊ�գ���ȡ���ݲ���ֵ��exposureTime
			file >> exposureTime;
			file.close();
		}
	}
	else {
		std::cerr << "�޷����ļ��������á�ʹ��Ĭ��ֵ��" << std::endl;
	}
}

// ���ļ�����������������
void Basler::loadOtherSettings(const std::string& filename)
{
	std::ifstream file(filename);

	if (file.is_open())
	{
		if (file.peek() == std::ifstream::traits_type::eof())
		{
			// �����ļ�Ϊ�յ������д��Ĭ��ֵ
			file.close();

			std::ofstream outFile(filename);
			if (outFile.is_open())
			{
				// д��Ĭ��ֵ
				outFile << 2000 << std::endl;
				outFile << 1.0 << std::endl;
				outFile << 100.0 << std::endl;
				outFile << 100.0 << std::endl;
				outFile << 100.0 << std::endl;
				outFile << 300000.0 << std::endl;
				outFile.close();

				// ���´��ļ��Զ�ȡ��д���Ĭ��ֵ
				std::ifstream inFile(filename);
				if (inFile.is_open())
				{
					// �ֱ��ȡ��������
					inFile >> XXD;
					inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // �����ָ���
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

					// ��Ϊ��ֻ��Ҫд��/��ȡǰ���У����Ժ�����UD, Dis, Area ����Ҫ����Ĭ��ֵ
				}
				else
				{
					std::cerr << "�޷����ļ��ж�ȡĬ��ֵ��" << std::endl;
				}
			}
			else
			{
				std::cerr << "�޷���Ĭ��ֵд���ļ���" << std::endl;
			}
		}
		else
		{
			// �ֱ��ȡ��������
			file >> XXD;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // �����ָ���
			file >> Scale;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> RL;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> UD;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> Dis;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file >> Area;

			// �ر��ļ�
			file.close();
		}
	}
	else {
		std::cerr << "�޷����ļ��������á�" << std::endl;
	}
}

Basler::Basler(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_control = new BaslerCamera();

	ui.comboBox_mode->addItem("Off");//�رմ���ģʽ
	ui.comboBox_mode->addItem("On");//�򿪴���ģʽ

	ui.comboBox_trigsrc->addItem("Software");//����������
	ui.comboBox_trigsrc->addItem("Line1");//����ߴ���

	loadSettings("D:/Save/camera.txt");
	loadOtherSettings("D:/Save/Othercamera.txt");

	// ͨѶ��ʱ
	QTimer* timer = new QTimer(this);
	connect(ui.pushButton_ConnectPLC, &QPushButton::clicked, [=]()
		{
			timer->start(500);
			connect(timer, &QTimer::timeout, this, [=]()
				{
					if (plcresult)
					{
						int OK[1] = { 1 };
						m_control->plc.PLCWrite(0xA8, &OK[0], 1, 4030);//ɨ���������״̬
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
							m_control->plc.PLCWrite(0xA8, &OK[0], 1, 4000);//����
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

	// ��ȡ��һ�α����ֵ
	QSettings settings("Mysetting", QSettings::IniFormat);

	// �ڳ�������ʱ�ָ�QSlider��ֵ
	int EXPValue = settings.value("sliderValue").toInt();
	if (EXPValue != -1)
	{
		ui.horizontalSliderExp->setValue(EXPValue);
		m_control->SetIntPara("ExposureTimeRaw", EXPValue);
		EXPvalue.clear();
		EXPvalue.push_back(EXPValue);
	}

	// �ڳ�������ʱ�ָ�QSpinBox��ֵ
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


	// ���浱ǰ�����ֵ
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

	double currentDoubleSpinBoxValue = ui.ScaleBox->value(); // ��ȡ��ǰ��ֵ
	settings.setValue("LastDoubleSpinBoxValue", currentDoubleSpinBoxValue); // ��ֵ���浽������
	double currentXXDValue = ui.XXD->value(); // ��ȡ��ǰ��ֵ
	settings.setValue("LastXXDValue", currentXXDValue); // ��ֵ���浽������

	double currentRL = ui.doubleSpinBox_RL->value(); // ��ȡ��ǰ��ֵ
	settings.setValue("LastRLValue", currentRL); // ��ֵ���浽������
	double currentUD = ui.doubleSpinBox_UD->value(); // ��ȡ��ǰ��ֵ
	settings.setValue("LastUDValue", currentUD); // ��ֵ���浽������

	double currentClothesDis = ui.Clothes_Dis->value(); // ��ȡ��ǰ��ֵ
	settings.setValue("LastClothesDisValue", currentClothesDis); // ��ֵ���浽������

	double currentClothesArea = ui.doubleSpinBox_Area->value(); // ��ȡ��ǰ��ֵ
	settings.setValue("LastClothesAreaValue", currentClothesArea); // ��ֵ���浽������

	int currentSourceIndex = ui.comboBox_trigsrc->currentIndex();//����Դ
	settings.setValue("comboBoxSourceIndex", currentSourceIndex);

	int currentModeIndex = ui.comboBox_mode->currentIndex();//����ģʽ
	settings.setValue("comboBoxModeIndex", currentModeIndex);

	// ����QSlider��ֵ
	int currentEXP = ui.horizontalSliderExp->value();
	settings.setValue("sliderValue", currentEXP);

	// ����QSpinBox��ֵ
	int spinBoxEXP = ui.spinBoxExp->value();
	settings.setValue("spinBoxValue", spinBoxEXP);

	saveSettings("D:/Save/camera.txt");
	saveOtherSettings("D:/Save/Othercamera.txt");

	Modeindex.clear();
	Sourceindex.clear();

	delete m_control, m_control = nullptr;
}

//�޸��ع� ����
void Basler::on_horizontalSliderExp_valueChanged(const int& value)
{
	m_control->SetIntPara("ExposureTimeRaw", value);
	ui.spinBoxExp->setValue(value);
	EXPvalue.clear();
	EXPvalue.push_back(value);
	on_spinBoxExp_editingFinished();
}

//�޸��ع� ΢��
void Basler::on_spinBoxExp_editingFinished()
{
	int tmp = ui.spinBoxExp->value();
	m_control->SetIntPara("ExposureTimeRaw", tmp);
	ui.horizontalSliderExp->setValue(tmp);
	EXPvalue.clear();
	EXPvalue.push_back(tmp);
}

//�޸ı�����������
void Basler::on_ScaleBox_editingFinished()
{
	float scale = ui.ScaleBox->value();
	XYscale.clear();
	XYscale.push_back(scale);
}
void Basler::on_RL_editingFinished()//ͼ������������
{
	float RLdis = ui.doubleSpinBox_RL->value();
	RLDistance.clear();
	RLDistance.push_back(RLdis);
}

void Basler::on_UD_editingFinished()//ͼ������������
{
	float UDdis = ui.doubleSpinBox_UD->value();
	UDDistance.clear();
	UDDistance.push_back(UDdis);
}

void Basler::on_ClothesDis_editingFinished()//��ȡ�����·����
{
	float Clothes_distance = ui.Clothes_Dis->value();
	Clothes_Distance.clear();
	Clothes_Distance.push_back(Clothes_distance);
	cout << Clothes_Distance[0] << endl;
}

void Basler::on_ClothesArea_editingFinished()//��ȡ�·������С
{
	float Clothes_area = ui.doubleSpinBox_Area->value();
	Clothes_Area.clear();
	Clothes_Area.push_back(Clothes_area);
	cout << Clothes_Area[0] << endl;
}

//���������ʱ������
void Basler::Xianxiaodou_editingFinished()
{
	XXDvalue.clear();
	int value = ui.XXD->value();
	XXDvalue.push_back(value);
}
//��ȡ����ģʽ
void Basler::on_comboBox_mode_activated(const QString& arg1)//��qstring����ת����c++��string���ڻ�ȡc���ַ�ָ��
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

//��ȡ����Դ
void Basler::on_comboBox_trigsrc_activated(const QString& arg1)//��qstring����ת����c++��string���ڻ�ȡc���ַ�ָ��
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


//�ж�IP�Ϸ���
bool Basler::isIPValid(const QString& ip)
{
	QRegularExpression ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
	QRegularExpressionMatch match = ipRegex.match(ip);
	return match.hasMatch();
}

//��ȡIP��ַ
QString Basler::IP_change()
{
	//��ȡlineedit����
	QString IP = ui.IP->text();
	if (isIPValid(IP))
	{
		//qDebug() << "IP��ַ��Ч";
		// IP��ַ�Ϸ�
		ui.IP->setStyleSheet("QLineEdit { color: black; }");
		ui.IP->setStyleSheet("QLineEdit{border:1px solid black }");
	}
	else {
		/*qDebug() << "IP��ַ��Ч";*/
		// IP��ַ���Ϸ�
		ui.IP->setStyleSheet("QLineEdit { color: red; }");
	}
	return IP;
}

//��ȡ�˿ں�
int Basler::port_change()
{
	//��ȡlineedit����
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



//����PLC
void Basler::pushButton_ConnectPLC_clicked()
{
	QString IP_Q = IP_change();//��ȡIP
	//QStringת��Ϊ const char*
	QByteArray byteArray1 = IP_Q.toUtf8();
	const char* IP_c = byteArray1.constData();

	int port = port_change();//��ȡ�˿ں�

	if (IP_Q.isEmpty() || port <= 0)
	{
		if (IP_Q.isEmpty())
		{
			// ����LineEdit��Ϊ��ɫ
			ui.IP->setStyleSheet("QLineEdit{border:1px solid red }");
		}
		if (port <= 0)
		{
			// ����LineEdit��Ϊ��ɫ
			ui.port->setStyleSheet("QLineEdit{border:1px solid red }");
		}
		QMessageBox::warning(this, "����", "IP��˿ںŷǷ�!");
		return;
	}
	m_control->plc.Startup();


	plcresult = m_control->plc.Open(IP_c, port, 500);

	if (!plcresult)
	{
		QMessageBox::critical(0, "����", "�޷����ӵ� PLC!");
		return;
	}

	if (plcresult)
	{
		QMessageBox::information(0, "��ȷ", "���� PLC�ɹ�!");
		m_control->plc.InitParam();
		int OK[1] = { 0 };
		m_control->plc.PLCWrite(0xA8, &OK[0], 1, 4020);//��ʼΪ0
		int OK1[15] = { 0 };
		m_control->plc.PLCWrite(0xA8, &OK1[0], 6, 4022);//��ʼΪ0
	}
}

void Basler::BaslerConditionMet()
{

	QString IP_Q = IP_change();//��ȡIP
	//QStringת��Ϊ const char*
	QByteArray byteArray1 = IP_Q.toUtf8();
	const char* IP_c = byteArray1.constData();

	int port = port_change();//��ȡ�˿ں�

	if (IP_Q.isEmpty() || port <= 0)
	{
		if (IP_Q.isEmpty())
		{
			// ����LineEdit��Ϊ��ɫ
			ui.IP->setStyleSheet("QLineEdit{border:1px solid red }");
		}
		if (port <= 0)
		{
			// ����LineEdit��Ϊ��ɫ
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
//�Ͽ�����
void Basler::pushButton_DisConnectPLC_clicked()
{
	plcresult = false;
	m_control->plc.Close();
	if (m_control->plc.IsOpen()) {
		QMessageBox::warning(0, "����", "�Ͽ�PLC ʧ��!");
		// δ�ɹ��ر����ӣ��������
		qDebug() << "Failed to disconnect PLC.";
	}
	else
	{
		QMessageBox::information(0, "�ɹ�", "�Ͽ�PLC �ɹ�!");
		// �ɹ��ر����ӣ��ͷ���Դ
		// �ͷ�������Դ�����������Ҫ��������
		qDebug() << "PLC disconnected successfully.";
	}
}
//�����
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

//��ʾͼ�����ĵ㣨�궨�㣩
void Basler::Show_Biaoding()
{
	BiaoDingresult = true;
}
void Basler::Com_Biaoding()
{
	BiaoDingresult = false;
}
//��ʾͼ��ʮ�ֿ���
void Basler::Open_xiangxian()
{
	Shizhiresult = true;
}
void Basler::Close_xiangxian()
{
	Shizhiresult = false;
}
//��ͼ�����ܱ߽��ť
void Basler::Open_RLUD()
{
	RLUDresult = true;
}
void Basler::Close_RLUD()
{
	RLUDresult = false;
}


//��ʼ�ɼ�ͼ��
void Basler::pushButton_Startgrab_clicked()
{
	m_control->start();
	ui.pushButtonStartCam->setEnabled(false);
	ui.pushButtonStopCam->setEnabled(true);
	ui.pushButtonCloseCam->setEnabled(false);
}

//ֹͣ�ɼ�ͼ��
void Basler::pushButton_Stopgrab_clicked()
{
	m_control->StopCamera();
	// �ȴ�һ��ʱ��
	Sleep(500);
	ui.pushButtonStartCam->setEnabled(true);
	ui.pushButtonStopCam->setEnabled(false);
	ui.pushButtonCloseCam->setEnabled(true);
}

//�������
void Basler::pushButton_Soft_clicked()
{
	m_control->SetCmd("TriggerSoftware");
}

//�ر����
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


//��ʾͼ��
void Basler::showImage(const QImage& dst)
{

	ui.label_KUZI->setPixmap(QPixmap::fromImage(dst).scaled(ui.label_KUZI->width(),
		ui.label_KUZI->height(),
		Qt::KeepAspectRatioByExpanding));
	ui.label_KUZI->setScaledContents(true);
	//// ���� QLabel �Ĵ�С����Ϊ Ignored
	//ui.label_KUZI->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	//// ���� QLabel ��С����Ӧͼ���С
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
	//�ļ���·��
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
	//�ļ���·��
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
	//�ļ���·��
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
	//��ȡlineedit����
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
	// ��鲢������·���ڵ��ļ���
	CheckAndCleanFolders(srcDirPathOK);
	CheckAndCleanFolders(srcDirPathNG);
	CheckAndCleanFiles(srcDirPathLog);
}

//�˳�����
void Basler::pushButton_Exit_clicked()
{
	QMessageBox::StandardButton result = QMessageBox::question(this, "��ʾ", "ȷ���˳�?", QMessageBox::Yes | QMessageBox::No);
	if (result == QMessageBox::Yes)
	{
		this->close();
	}
	else {}
}


