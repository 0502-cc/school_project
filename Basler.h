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

    void pushButton_Opencamera_clicked();   //打开相机
    void pushButton_Closecamera_clicked();   //关闭相机
    void pushButton_Startgrab_clicked();   //开始采集图像
    void pushButton_Stopgrab_clicked();  //停止采集图像
    void pushButton_Exit_clicked();//退出程序
    void pushButton_Soft_clicked(); //软件触发
    void showImage(const QImage& dst);//显示图像
    void on_comboBox_mode_activated(const QString& arg1);//获取触发模式
    void on_comboBox_trigsrc_activated(const QString& arg1);//获取触发源（线触发、软件触发）
    void on_horizontalSliderExp_valueChanged(const int& value);//获取曝光时间
    void on_spinBoxExp_editingFinished();//获取曝光时间
    void on_ScaleBox_editingFinished();//获取缩放因子
    void Xianxiaodou_editingFinished();//获取缩放因子
    void on_ClothesDis_editingFinished();//获取两件衣服相隔间距
    void on_ClothesArea_editingFinished();//获取衣服面积进行判断
    void CheckAndCleanFiles(QString path);
    void pushButton_ConnectPLC_clicked();//连接PLC
    void pushButton_DisConnectPLC_clicked();//断开连接PLC
    void on_RL_editingFinished();//图像左右两侧间距
    void on_UD_editingFinished();//图像上下两侧间距

    bool isIPValid(const QString& ip);
    QString IP_change();
    int port_change();
    void OKPath_pushButton_clicked();
    void LogPath_pushButton_clicked();
    void NGPath_pushButton_clicked();
    int SaveTime_lineEdit_Change();
    void Show_Biaoding();//显示图像中心点  标定点
    void Com_Biaoding();//标定完成
    void Open_xiangxian();//打开十字开关按钮
    void Close_xiangxian();//关闭十字开关按钮
    void Open_RLUD();//打开图像四周边界框按钮
    void Close_RLUD();//关闭图像四周边界框按钮

private:
    Ui::BaslerClass ui;
    BaslerCamera* m_control;   //定义Basler相机
};
#endif // BASLER_H