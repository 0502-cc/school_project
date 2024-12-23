#include "Basler.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{

    // 文件路径
    std::string filePath = "D:/Save/camera.txt";

    // 尝试打开文件，检查文件是否存在
    std::ifstream existingFile(filePath);
    if (existingFile.is_open())
    {
        // 文件已存在，关闭文件
        existingFile.close();
    }
    else 
    {
        // 文件不存在，尝试创建文件
        std::ofstream newFile(filePath);
        if (newFile.is_open())
        {
            // 关闭文件
            newFile.close();
        }
        else {
            // 文件无法创建，输出错误信息
            std::cerr << "无法创建文件：" << filePath << std::endl;
            return 1;  // 退出程序
        }
    }

    // 文件路径
    std::string NewfilePath = "D:/Save/Othercamera.txt";

    // 尝试打开文件，检查文件是否存在
    std::ifstream newexistingFile(NewfilePath);
    if (newexistingFile.is_open())
    {
        // 文件已存在，关闭文件
        newexistingFile.close();
    }
    else
    {
        // 文件不存在，尝试创建文件
        std::ofstream twonewFile(NewfilePath);
        if (twonewFile.is_open())
        {
            // 关闭文件
            twonewFile.close();
        }
        else {
            // 文件无法创建，输出错误信息
            std::cerr << "无法创建文件：" << NewfilePath << std::endl;
            return 1;  // 退出程序
        }
    }


    SYSTEMTIME st;
    GetLocalTime(&st);
    QApplication a(argc, argv);

    // 获取D盘根路径
    QString dPath = "D:/Save";
    // 创建Log文件夹
    QString LogPath = dPath + "/Log";
    QDir LogDir(LogPath);
    if (!LogDir.exists())
    {
        LogDir.mkpath(".");
    }
    // 创建BarScanIMG文件夹
    QString barScanImgPath = dPath + "/ClothesIMG";
    QDir barScanImgDir(barScanImgPath);
    if (!barScanImgDir.exists())
    {
        barScanImgDir.mkpath(".");
    }
    // 在BarScanIMG文件夹下创建OK文件夹
    QString okPath = barScanImgPath + "/OK";
    QDir okDir(okPath);
    if (!okDir.exists())
    {
        okDir.mkpath(".");
    }
    // 在OK文件夹下创建日期文件
    QString OKdayPath = okPath + QString("/%1-%2").arg(st.wMonth).arg(st.wDay);;
    QDir OKdayDir(OKdayPath);
    if (!OKdayDir.exists())
    {
        OKdayDir.mkpath(".");
    }
    // 在BarScanIMG文件夹下创建NG文件夹
    QString ngPath = barScanImgPath + "/NG";
    QDir ngDir(ngPath);
    if (!ngDir.exists())
    {
        ngDir.mkpath(".");
    }
    // 在NG文件夹下创建日期文件
    QString NGdayPath = ngPath + QString("/%1-%2").arg(st.wMonth).arg(st.wDay);;
    QDir NGdayDir(NGdayPath);
    if (!NGdayDir.exists())
    {
        NGdayDir.mkpath(".");
    }
    Basler w;
    w.show();
    return a.exec();

}
