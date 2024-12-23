#include "Basler.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{

    // �ļ�·��
    std::string filePath = "D:/Save/camera.txt";

    // ���Դ��ļ�������ļ��Ƿ����
    std::ifstream existingFile(filePath);
    if (existingFile.is_open())
    {
        // �ļ��Ѵ��ڣ��ر��ļ�
        existingFile.close();
    }
    else 
    {
        // �ļ������ڣ����Դ����ļ�
        std::ofstream newFile(filePath);
        if (newFile.is_open())
        {
            // �ر��ļ�
            newFile.close();
        }
        else {
            // �ļ��޷����������������Ϣ
            std::cerr << "�޷������ļ���" << filePath << std::endl;
            return 1;  // �˳�����
        }
    }

    // �ļ�·��
    std::string NewfilePath = "D:/Save/Othercamera.txt";

    // ���Դ��ļ�������ļ��Ƿ����
    std::ifstream newexistingFile(NewfilePath);
    if (newexistingFile.is_open())
    {
        // �ļ��Ѵ��ڣ��ر��ļ�
        newexistingFile.close();
    }
    else
    {
        // �ļ������ڣ����Դ����ļ�
        std::ofstream twonewFile(NewfilePath);
        if (twonewFile.is_open())
        {
            // �ر��ļ�
            twonewFile.close();
        }
        else {
            // �ļ��޷����������������Ϣ
            std::cerr << "�޷������ļ���" << NewfilePath << std::endl;
            return 1;  // �˳�����
        }
    }


    SYSTEMTIME st;
    GetLocalTime(&st);
    QApplication a(argc, argv);

    // ��ȡD�̸�·��
    QString dPath = "D:/Save";
    // ����Log�ļ���
    QString LogPath = dPath + "/Log";
    QDir LogDir(LogPath);
    if (!LogDir.exists())
    {
        LogDir.mkpath(".");
    }
    // ����BarScanIMG�ļ���
    QString barScanImgPath = dPath + "/ClothesIMG";
    QDir barScanImgDir(barScanImgPath);
    if (!barScanImgDir.exists())
    {
        barScanImgDir.mkpath(".");
    }
    // ��BarScanIMG�ļ����´���OK�ļ���
    QString okPath = barScanImgPath + "/OK";
    QDir okDir(okPath);
    if (!okDir.exists())
    {
        okDir.mkpath(".");
    }
    // ��OK�ļ����´��������ļ�
    QString OKdayPath = okPath + QString("/%1-%2").arg(st.wMonth).arg(st.wDay);;
    QDir OKdayDir(OKdayPath);
    if (!OKdayDir.exists())
    {
        OKdayDir.mkpath(".");
    }
    // ��BarScanIMG�ļ����´���NG�ļ���
    QString ngPath = barScanImgPath + "/NG";
    QDir ngDir(ngPath);
    if (!ngDir.exists())
    {
        ngDir.mkpath(".");
    }
    // ��NG�ļ����´��������ļ�
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
