#ifndef BASLER_H
#define BASLER_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QtWidgets/QMainWindow>
#include "ui_Basler.h"
#include "Camera.h"
#include "QTimer"
#include <QTranslator.h>
#include<QMessageBox>
#include <QSettings>
#include <qdatetime.h>
#include <QFileDialog>
#include "log.h"
namespace Ui
{
    class Basler;
}

class Basler : public QMainWindow
{
    Q_OBJECT

public:
    Basler(QWidget* parent = nullptr);
    ~Basler();
    void CheckAndCleanFolders(QString path);
    void triggerCheckAndCleanFolders();
    int SaveTime = 0;
    void saveSettings(const std::string& filename);
    void loadSettings(const std::string& filename);
    void saveOtherSettings(const std::string& filename);
    void loadOtherSettings(const std::string& filename);
    float exposureTime;
    double XXD;
    double Scale;
    double RL;
    double UD;
    double Dis;
    double Area;

public slots:
    void BaslerConditionMet();
private slots:

    void pushButton_Opencamera_clicked();   //�����
    void pushButton_Closecamera_clicked();   //�ر����
    void pushButton_Startgrab_clicked();   //��ʼ�ɼ�ͼ��
    void pushButton_Stopgrab_clicked();  //ֹͣ�ɼ�ͼ��
    void pushButton_Exit_clicked();//�˳�����
    void pushButton_Soft_clicked(); //�������
    void showImage(const QImage& dst);//��ʾͼ��
    void on_comboBox_mode_activated(const QString& arg1);//��ȡ����ģʽ
    void on_comboBox_trigsrc_activated(const QString& arg1);//��ȡ����Դ���ߴ��������������
    void on_horizontalSliderExp_valueChanged(const int& value);//��ȡ�ع�ʱ��
    void on_spinBoxExp_editingFinished();//��ȡ�ع�ʱ��
    void on_ScaleBox_editingFinished();//��ȡ��������
    void Xianxiaodou_editingFinished();//��ȡ��������
    void on_ClothesDis_editingFinished();//��ȡ�����·�������
    void on_ClothesArea_editingFinished();//��ȡ�·���������ж�
    void CheckAndCleanFiles(QString path);
    void pushButton_ConnectPLC_clicked();//����PLC
    void pushButton_DisConnectPLC_clicked();//�Ͽ�����PLC
    void on_RL_editingFinished();//ͼ������������
    void on_UD_editingFinished();//ͼ������������

    bool isIPValid(const QString& ip);
    QString IP_change();
    int port_change();
    void OKPath_pushButton_clicked();
    void LogPath_pushButton_clicked();
    void NGPath_pushButton_clicked();
    int SaveTime_lineEdit_Change();
    void Show_Biaoding();//��ʾͼ�����ĵ�  �궨��
    void Com_Biaoding();//�궨���
    void Open_xiangxian();//��ʮ�ֿ��ذ�ť
    void Close_xiangxian();//�ر�ʮ�ֿ��ذ�ť
    void Open_RLUD();//��ͼ�����ܱ߽��ť
    void Close_RLUD();//�ر�ͼ�����ܱ߽��ť

private:
    Ui::BaslerClass ui;
    BaslerCamera* m_control;   //����Basler���
};
#endif // BASLER_H