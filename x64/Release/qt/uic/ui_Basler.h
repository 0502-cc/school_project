/********************************************************************************
** Form generated from reading UI file 'Basler.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BASLER_H
#define UI_BASLER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BaslerClass
{
public:
    QWidget *centralWidget;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_7;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QSlider *horizontalSliderExp;
    QSpinBox *spinBoxExp;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_14;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_15;
    QLineEdit *OK_lineEdit;
    QPushButton *OKPath_pushButton;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_16;
    QLineEdit *NG_lineEdit;
    QPushButton *NGPath_pushButton;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_17;
    QLineEdit *Log_lineEdit;
    QPushButton *pushButton_3;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_18;
    QLineEdit *SaveTime_lineEdit;
    QSpacerItem *verticalSpacer_4;
    QLabel *label;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_4;
    QLineEdit *IP;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_5;
    QLineEdit *port;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *pushButton_ConnectPLC;
    QPushButton *pushButton_DisConnectPLC;
    QSpacerItem *verticalSpacer;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_10;
    QComboBox *comboBox_trigsrc;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_11;
    QComboBox *comboBox_mode;
    QSpacerItem *verticalSpacer_3;
    QLabel *label_12;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_9;
    QLineEdit *lineEdit_Number;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *lineEdit_PointX;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_13;
    QLineEdit *lineEdit_PointY;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_22;
    QLineEdit *lineEdit_long;
    QHBoxLayout *horizontalLayout_21;
    QLabel *label_23;
    QLineEdit *lineEdit_short;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_20;
    QLineEdit *lineEdit_Direction;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QLineEdit *lineEdit_Angle;
    QScrollArea *scrollArea_3;
    QWidget *scrollAreaWidgetContents_3;
    QHBoxLayout *horizontalLayout_12;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_19;
    QPushButton *pushButtonOpenCam;
    QPushButton *pushButtonStartCam;
    QPushButton *pushButtonSoftTrig;
    QPushButton *pushButtonStopCam;
    QPushButton *pushButtonCloseCam;
    QPushButton *push_close;
    QHBoxLayout *horizontalLayout_13;
    QPushButton *pushButton_2;
    QPushButton *pushButton_4;
    QPushButton *push_show;
    QPushButton *pushButton;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QHBoxLayout *horizontalLayout_22;
    QLabel *label_21;
    QDoubleSpinBox *XXD;
    QLabel *label_19;
    QDoubleSpinBox *ScaleBox;
    QLabel *label_24;
    QDoubleSpinBox *doubleSpinBox_RL;
    QLabel *label_25;
    QDoubleSpinBox *doubleSpinBox_UD;
    QLabel *label_26;
    QDoubleSpinBox *Clothes_Dis;
    QLabel *label_27;
    QDoubleSpinBox *doubleSpinBox_Area;
    QFrame *frame;
    QLabel *label_KUZI;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BaslerClass)
    {
        if (BaslerClass->objectName().isEmpty())
            BaslerClass->setObjectName(QString::fromUtf8("BaslerClass"));
        BaslerClass->resize(1675, 1069);
        centralWidget = new QWidget(BaslerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        scrollArea_2 = new QScrollArea(centralWidget);
        scrollArea_2->setObjectName(QString::fromUtf8("scrollArea_2"));
        scrollArea_2->setGeometry(QRect(1030, 0, 641, 821));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 639, 819));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents_2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_7 = new QLabel(scrollAreaWidgetContents_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_3->addWidget(label_7);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_8 = new QLabel(scrollAreaWidgetContents_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_7->addWidget(label_8);

        horizontalSliderExp = new QSlider(scrollAreaWidgetContents_2);
        horizontalSliderExp->setObjectName(QString::fromUtf8("horizontalSliderExp"));
        horizontalSliderExp->setMinimum(0);
        horizontalSliderExp->setMaximum(30000);
        horizontalSliderExp->setSingleStep(100);
        horizontalSliderExp->setPageStep(100);
        horizontalSliderExp->setValue(500);
        horizontalSliderExp->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(horizontalSliderExp);

        spinBoxExp = new QSpinBox(scrollAreaWidgetContents_2);
        spinBoxExp->setObjectName(QString::fromUtf8("spinBoxExp"));
        spinBoxExp->setMinimum(0);
        spinBoxExp->setMaximum(30000);
        spinBoxExp->setSingleStep(100);
        spinBoxExp->setValue(3000);

        horizontalLayout_7->addWidget(spinBoxExp);


        verticalLayout_3->addLayout(horizontalLayout_7);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        label_14 = new QLabel(scrollAreaWidgetContents_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        verticalLayout_3->addWidget(label_14);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_15 = new QLabel(scrollAreaWidgetContents_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_14->addWidget(label_15);

        OK_lineEdit = new QLineEdit(scrollAreaWidgetContents_2);
        OK_lineEdit->setObjectName(QString::fromUtf8("OK_lineEdit"));

        horizontalLayout_14->addWidget(OK_lineEdit);

        OKPath_pushButton = new QPushButton(scrollAreaWidgetContents_2);
        OKPath_pushButton->setObjectName(QString::fromUtf8("OKPath_pushButton"));

        horizontalLayout_14->addWidget(OKPath_pushButton);


        verticalLayout_4->addLayout(horizontalLayout_14);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        label_16 = new QLabel(scrollAreaWidgetContents_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_15->addWidget(label_16);

        NG_lineEdit = new QLineEdit(scrollAreaWidgetContents_2);
        NG_lineEdit->setObjectName(QString::fromUtf8("NG_lineEdit"));

        horizontalLayout_15->addWidget(NG_lineEdit);

        NGPath_pushButton = new QPushButton(scrollAreaWidgetContents_2);
        NGPath_pushButton->setObjectName(QString::fromUtf8("NGPath_pushButton"));

        horizontalLayout_15->addWidget(NGPath_pushButton);


        verticalLayout_4->addLayout(horizontalLayout_15);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        label_17 = new QLabel(scrollAreaWidgetContents_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        horizontalLayout_16->addWidget(label_17);

        Log_lineEdit = new QLineEdit(scrollAreaWidgetContents_2);
        Log_lineEdit->setObjectName(QString::fromUtf8("Log_lineEdit"));

        horizontalLayout_16->addWidget(Log_lineEdit);

        pushButton_3 = new QPushButton(scrollAreaWidgetContents_2);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout_16->addWidget(pushButton_3);


        verticalLayout_4->addLayout(horizontalLayout_16);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setSpacing(6);
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_18 = new QLabel(scrollAreaWidgetContents_2);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        horizontalLayout_17->addWidget(label_18);

        SaveTime_lineEdit = new QLineEdit(scrollAreaWidgetContents_2);
        SaveTime_lineEdit->setObjectName(QString::fromUtf8("SaveTime_lineEdit"));

        horizontalLayout_17->addWidget(SaveTime_lineEdit);


        verticalLayout_4->addLayout(horizontalLayout_17);


        verticalLayout_3->addLayout(verticalLayout_4);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        label = new QLabel(scrollAreaWidgetContents_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_3->addWidget(label);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_4 = new QLabel(scrollAreaWidgetContents_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_5->addWidget(label_4);

        IP = new QLineEdit(scrollAreaWidgetContents_2);
        IP->setObjectName(QString::fromUtf8("IP"));

        horizontalLayout_5->addWidget(IP);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_5 = new QLabel(scrollAreaWidgetContents_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_6->addWidget(label_5);

        port = new QLineEdit(scrollAreaWidgetContents_2);
        port->setObjectName(QString::fromUtf8("port"));

        horizontalLayout_6->addWidget(port);


        verticalLayout->addLayout(horizontalLayout_6);


        verticalLayout_3->addLayout(verticalLayout);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        pushButton_ConnectPLC = new QPushButton(scrollAreaWidgetContents_2);
        pushButton_ConnectPLC->setObjectName(QString::fromUtf8("pushButton_ConnectPLC"));

        horizontalLayout_9->addWidget(pushButton_ConnectPLC);

        pushButton_DisConnectPLC = new QPushButton(scrollAreaWidgetContents_2);
        pushButton_DisConnectPLC->setObjectName(QString::fromUtf8("pushButton_DisConnectPLC"));

        horizontalLayout_9->addWidget(pushButton_DisConnectPLC);


        verticalLayout_3->addLayout(horizontalLayout_9);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        label_6 = new QLabel(scrollAreaWidgetContents_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_3->addWidget(label_6);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_10 = new QLabel(scrollAreaWidgetContents_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_10->addWidget(label_10);

        comboBox_trigsrc = new QComboBox(scrollAreaWidgetContents_2);
        comboBox_trigsrc->setObjectName(QString::fromUtf8("comboBox_trigsrc"));

        horizontalLayout_10->addWidget(comboBox_trigsrc);


        verticalLayout_3->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_11 = new QLabel(scrollAreaWidgetContents_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_11->addWidget(label_11);

        comboBox_mode = new QComboBox(scrollAreaWidgetContents_2);
        comboBox_mode->setObjectName(QString::fromUtf8("comboBox_mode"));

        horizontalLayout_11->addWidget(comboBox_mode);


        verticalLayout_3->addLayout(horizontalLayout_11);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);

        label_12 = new QLabel(scrollAreaWidgetContents_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        verticalLayout_3->addWidget(label_12);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_9 = new QLabel(scrollAreaWidgetContents_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_8->addWidget(label_9);

        lineEdit_Number = new QLineEdit(scrollAreaWidgetContents_2);
        lineEdit_Number->setObjectName(QString::fromUtf8("lineEdit_Number"));

        horizontalLayout_8->addWidget(lineEdit_Number);


        verticalLayout_2->addLayout(horizontalLayout_8);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(scrollAreaWidgetContents_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_3->addWidget(label_2);

        lineEdit_PointX = new QLineEdit(scrollAreaWidgetContents_2);
        lineEdit_PointX->setObjectName(QString::fromUtf8("lineEdit_PointX"));

        horizontalLayout_3->addWidget(lineEdit_PointX);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_13 = new QLabel(scrollAreaWidgetContents_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_2->addWidget(label_13);

        lineEdit_PointY = new QLineEdit(scrollAreaWidgetContents_2);
        lineEdit_PointY->setObjectName(QString::fromUtf8("lineEdit_PointY"));

        horizontalLayout_2->addWidget(lineEdit_PointY);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setSpacing(6);
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        label_22 = new QLabel(scrollAreaWidgetContents_2);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        horizontalLayout_20->addWidget(label_22);

        lineEdit_long = new QLineEdit(scrollAreaWidgetContents_2);
        lineEdit_long->setObjectName(QString::fromUtf8("lineEdit_long"));

        horizontalLayout_20->addWidget(lineEdit_long);


        verticalLayout_2->addLayout(horizontalLayout_20);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setSpacing(6);
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        label_23 = new QLabel(scrollAreaWidgetContents_2);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        horizontalLayout_21->addWidget(label_23);

        lineEdit_short = new QLineEdit(scrollAreaWidgetContents_2);
        lineEdit_short->setObjectName(QString::fromUtf8("lineEdit_short"));

        horizontalLayout_21->addWidget(lineEdit_short);


        verticalLayout_2->addLayout(horizontalLayout_21);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setSpacing(6);
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        label_20 = new QLabel(scrollAreaWidgetContents_2);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        horizontalLayout_18->addWidget(label_20);

        lineEdit_Direction = new QLineEdit(scrollAreaWidgetContents_2);
        lineEdit_Direction->setObjectName(QString::fromUtf8("lineEdit_Direction"));

        horizontalLayout_18->addWidget(lineEdit_Direction);


        verticalLayout_2->addLayout(horizontalLayout_18);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_3 = new QLabel(scrollAreaWidgetContents_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_4->addWidget(label_3);

        lineEdit_Angle = new QLineEdit(scrollAreaWidgetContents_2);
        lineEdit_Angle->setObjectName(QString::fromUtf8("lineEdit_Angle"));

        horizontalLayout_4->addWidget(lineEdit_Angle);


        verticalLayout_2->addLayout(horizontalLayout_4);


        verticalLayout_3->addLayout(verticalLayout_2);

        scrollArea_2->setWidget(scrollAreaWidgetContents_2);
        scrollArea_3 = new QScrollArea(centralWidget);
        scrollArea_3->setObjectName(QString::fromUtf8("scrollArea_3"));
        scrollArea_3->setGeometry(QRect(-10, 820, 1681, 191));
        scrollArea_3->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_3"));
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 1679, 189));
        horizontalLayout_12 = new QHBoxLayout(scrollAreaWidgetContents_3);
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setSpacing(6);
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        pushButtonOpenCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonOpenCam->setObjectName(QString::fromUtf8("pushButtonOpenCam"));
        pushButtonOpenCam->setAutoRepeatDelay(0);
        pushButtonOpenCam->setAutoRepeatInterval(0);

        horizontalLayout_19->addWidget(pushButtonOpenCam);

        pushButtonStartCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonStartCam->setObjectName(QString::fromUtf8("pushButtonStartCam"));

        horizontalLayout_19->addWidget(pushButtonStartCam);

        pushButtonSoftTrig = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonSoftTrig->setObjectName(QString::fromUtf8("pushButtonSoftTrig"));

        horizontalLayout_19->addWidget(pushButtonSoftTrig);

        pushButtonStopCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonStopCam->setObjectName(QString::fromUtf8("pushButtonStopCam"));

        horizontalLayout_19->addWidget(pushButtonStopCam);

        pushButtonCloseCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonCloseCam->setObjectName(QString::fromUtf8("pushButtonCloseCam"));

        horizontalLayout_19->addWidget(pushButtonCloseCam);

        push_close = new QPushButton(scrollAreaWidgetContents_3);
        push_close->setObjectName(QString::fromUtf8("push_close"));

        horizontalLayout_19->addWidget(push_close);


        verticalLayout_6->addLayout(horizontalLayout_19);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        pushButton_2 = new QPushButton(scrollAreaWidgetContents_3);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout_13->addWidget(pushButton_2);

        pushButton_4 = new QPushButton(scrollAreaWidgetContents_3);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));

        horizontalLayout_13->addWidget(pushButton_4);

        push_show = new QPushButton(scrollAreaWidgetContents_3);
        push_show->setObjectName(QString::fromUtf8("push_show"));

        horizontalLayout_13->addWidget(push_show);

        pushButton = new QPushButton(scrollAreaWidgetContents_3);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_13->addWidget(pushButton);

        pushButton_5 = new QPushButton(scrollAreaWidgetContents_3);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));

        horizontalLayout_13->addWidget(pushButton_5);

        pushButton_6 = new QPushButton(scrollAreaWidgetContents_3);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));

        horizontalLayout_13->addWidget(pushButton_6);


        verticalLayout_6->addLayout(horizontalLayout_13);

        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setSpacing(6);
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        label_21 = new QLabel(scrollAreaWidgetContents_3);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        horizontalLayout_22->addWidget(label_21);

        XXD = new QDoubleSpinBox(scrollAreaWidgetContents_3);
        XXD->setObjectName(QString::fromUtf8("XXD"));
        XXD->setDecimals(1);
        XXD->setMinimum(0.000000000000000);
        XXD->setMaximum(20000.000000000000000);
        XXD->setSingleStep(1000.000000000000000);

        horizontalLayout_22->addWidget(XXD);

        label_19 = new QLabel(scrollAreaWidgetContents_3);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        horizontalLayout_22->addWidget(label_19);

        ScaleBox = new QDoubleSpinBox(scrollAreaWidgetContents_3);
        ScaleBox->setObjectName(QString::fromUtf8("ScaleBox"));
        ScaleBox->setDecimals(3);
        ScaleBox->setMinimum(-999.999000000000024);
        ScaleBox->setMaximum(999.999000000000024);
        ScaleBox->setSingleStep(1.000000000000000);
        ScaleBox->setValue(1.000000000000000);

        horizontalLayout_22->addWidget(ScaleBox);

        label_24 = new QLabel(scrollAreaWidgetContents_3);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        horizontalLayout_22->addWidget(label_24);

        doubleSpinBox_RL = new QDoubleSpinBox(scrollAreaWidgetContents_3);
        doubleSpinBox_RL->setObjectName(QString::fromUtf8("doubleSpinBox_RL"));
        doubleSpinBox_RL->setMaximum(640.000000000000000);
        doubleSpinBox_RL->setSingleStep(5.000000000000000);
        doubleSpinBox_RL->setValue(100.000000000000000);

        horizontalLayout_22->addWidget(doubleSpinBox_RL);

        label_25 = new QLabel(scrollAreaWidgetContents_3);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        horizontalLayout_22->addWidget(label_25);

        doubleSpinBox_UD = new QDoubleSpinBox(scrollAreaWidgetContents_3);
        doubleSpinBox_UD->setObjectName(QString::fromUtf8("doubleSpinBox_UD"));
        doubleSpinBox_UD->setMaximum(512.000000000000000);
        doubleSpinBox_UD->setSingleStep(5.000000000000000);
        doubleSpinBox_UD->setValue(100.000000000000000);

        horizontalLayout_22->addWidget(doubleSpinBox_UD);

        label_26 = new QLabel(scrollAreaWidgetContents_3);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        horizontalLayout_22->addWidget(label_26);

        Clothes_Dis = new QDoubleSpinBox(scrollAreaWidgetContents_3);
        Clothes_Dis->setObjectName(QString::fromUtf8("Clothes_Dis"));
        Clothes_Dis->setMaximum(1000.000000000000000);
        Clothes_Dis->setSingleStep(10.000000000000000);
        Clothes_Dis->setValue(100.000000000000000);

        horizontalLayout_22->addWidget(Clothes_Dis);

        label_27 = new QLabel(scrollAreaWidgetContents_3);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        horizontalLayout_22->addWidget(label_27);

        doubleSpinBox_Area = new QDoubleSpinBox(scrollAreaWidgetContents_3);
        doubleSpinBox_Area->setObjectName(QString::fromUtf8("doubleSpinBox_Area"));
        doubleSpinBox_Area->setMinimum(0.000000000000000);
        doubleSpinBox_Area->setMaximum(1000000.000000000000000);
        doubleSpinBox_Area->setSingleStep(10000.000000000000000);
        doubleSpinBox_Area->setValue(300000.000000000000000);

        horizontalLayout_22->addWidget(doubleSpinBox_Area);


        verticalLayout_6->addLayout(horizontalLayout_22);


        verticalLayout_5->addLayout(verticalLayout_6);


        horizontalLayout_12->addLayout(verticalLayout_5);

        scrollArea_3->setWidget(scrollAreaWidgetContents_3);
        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 1024, 819));
        frame->setMinimumSize(QSize(1024, 819));
        frame->setMaximumSize(QSize(1024, 819));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_KUZI = new QLabel(frame);
        label_KUZI->setObjectName(QString::fromUtf8("label_KUZI"));
        label_KUZI->setEnabled(true);
        label_KUZI->setGeometry(QRect(0, 0, 1024, 819));
        label_KUZI->setMinimumSize(QSize(1024, 819));
        label_KUZI->setMaximumSize(QSize(1024, 819));
        BaslerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BaslerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1675, 22));
        BaslerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BaslerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        BaslerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(BaslerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        BaslerClass->setStatusBar(statusBar);

        retranslateUi(BaslerClass);
        QObject::connect(pushButtonOpenCam, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_Opencamera_clicked()));
        QObject::connect(pushButtonStartCam, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_Startgrab_clicked()));
        QObject::connect(pushButtonSoftTrig, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_Soft_clicked()));
        QObject::connect(pushButtonStopCam, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_Stopgrab_clicked()));
        QObject::connect(pushButtonCloseCam, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_Closecamera_clicked()));
        QObject::connect(push_close, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_Exit_clicked()));
        QObject::connect(horizontalSliderExp, SIGNAL(valueChanged(int)), BaslerClass, SLOT(on_horizontalSliderExp_valueChanged()));
        QObject::connect(spinBoxExp, SIGNAL(editingFinished()), BaslerClass, SLOT(on_spinBoxExp_editingFinished()));
        QObject::connect(comboBox_mode, SIGNAL(activated(int)), BaslerClass, SLOT(on_comboBox_mode_activated()));
        QObject::connect(comboBox_trigsrc, SIGNAL(activated(int)), BaslerClass, SLOT(on_comboBox_trigsrc_activated()));
        QObject::connect(pushButton_ConnectPLC, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_ConnectPLC_clicked()));
        QObject::connect(pushButton_DisConnectPLC, SIGNAL(clicked()), BaslerClass, SLOT(pushButton_DisConnectPLC_clicked()));
        QObject::connect(OKPath_pushButton, SIGNAL(clicked()), BaslerClass, SLOT(OKPath_pushButton_clicked()));
        QObject::connect(NGPath_pushButton, SIGNAL(clicked()), BaslerClass, SLOT(NGPath_pushButton_clicked()));
        QObject::connect(pushButton_3, SIGNAL(clicked()), BaslerClass, SLOT(LogPath_pushButton_clicked()));
        QObject::connect(SaveTime_lineEdit, SIGNAL(editingFinished()), BaslerClass, SLOT(SaveTime_lineEdit_Change()));
        QObject::connect(ScaleBox, SIGNAL(editingFinished()), BaslerClass, SLOT(on_ScaleBox_editingFinished()));
        QObject::connect(push_show, SIGNAL(clicked()), BaslerClass, SLOT(Show_Biaoding()));
        QObject::connect(pushButton, SIGNAL(clicked()), BaslerClass, SLOT(Com_Biaoding()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), BaslerClass, SLOT(Open_xiangxian()));
        QObject::connect(pushButton_4, SIGNAL(clicked()), BaslerClass, SLOT(Close_xiangxian()));
        QObject::connect(doubleSpinBox_RL, SIGNAL(editingFinished()), BaslerClass, SLOT(on_RL_editingFinished()));
        QObject::connect(doubleSpinBox_UD, SIGNAL(editingFinished()), BaslerClass, SLOT(on_UD_editingFinished()));
        QObject::connect(pushButton_5, SIGNAL(clicked()), BaslerClass, SLOT(Open_RLUD()));
        QObject::connect(pushButton_6, SIGNAL(clicked()), BaslerClass, SLOT(Close_RLUD()));
        QObject::connect(Clothes_Dis, SIGNAL(editingFinished()), BaslerClass, SLOT(on_ClothesDis_editingFinished()));
        QObject::connect(doubleSpinBox_Area, SIGNAL(editingFinished()), BaslerClass, SLOT(on_ClothesArea_editingFinished()));
        QObject::connect(XXD, SIGNAL(editingFinished()), BaslerClass, SLOT(Xianxiaodou_editingFinished()));

        QMetaObject::connectSlotsByName(BaslerClass);
    } // setupUi

    void retranslateUi(QMainWindow *BaslerClass)
    {
        BaslerClass->setWindowTitle(QCoreApplication::translate("BaslerClass", "Basler", nullptr));
        label_7->setText(QCoreApplication::translate("BaslerClass", "\347\233\270\346\234\272\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label_8->setText(QCoreApplication::translate("BaslerClass", "\346\233\235\345\205\211\346\227\266\351\227\264\357\274\232", nullptr));
        label_14->setText(QCoreApplication::translate("BaslerClass", "\345\255\230\345\202\250\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label_15->setText(QCoreApplication::translate("BaslerClass", " O K \345\233\276 \357\274\232", nullptr));
        OKPath_pushButton->setText(QCoreApplication::translate("BaslerClass", "\346\265\217\350\247\210", nullptr));
        label_16->setText(QCoreApplication::translate("BaslerClass", " N G \345\233\276 \357\274\232", nullptr));
        NGPath_pushButton->setText(QCoreApplication::translate("BaslerClass", "\346\265\217\350\247\210", nullptr));
        label_17->setText(QCoreApplication::translate("BaslerClass", " \346\227\245  \345\277\227 \357\274\232", nullptr));
        pushButton_3->setText(QCoreApplication::translate("BaslerClass", "\346\265\217\350\247\210", nullptr));
        label_18->setText(QCoreApplication::translate("BaslerClass", "\345\255\230\345\202\250\345\244\251\346\225\260\357\274\232", nullptr));
        label->setText(QCoreApplication::translate("BaslerClass", "\351\200\232\350\256\257\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label_4->setText(QCoreApplication::translate("BaslerClass", "PLC IP\357\274\232", nullptr));
        label_5->setText(QCoreApplication::translate("BaslerClass", "\347\253\257\345\217\243\345\217\267\357\274\232", nullptr));
        pushButton_ConnectPLC->setText(QCoreApplication::translate("BaslerClass", "PLC\350\277\236\346\216\245", nullptr));
        pushButton_DisConnectPLC->setText(QCoreApplication::translate("BaslerClass", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label_6->setText(QCoreApplication::translate("BaslerClass", "\350\247\246\345\217\221\346\250\241\345\274\217\350\256\276\347\275\256", nullptr));
        label_10->setText(QCoreApplication::translate("BaslerClass", "\350\247\246 \345\217\221 \346\272\220\357\274\232", nullptr));
        label_11->setText(QCoreApplication::translate("BaslerClass", "\350\247\246\345\217\221\346\250\241\345\274\217\357\274\232", nullptr));
        label_12->setText(QCoreApplication::translate("BaslerClass", "\346\225\260\346\215\256\346\230\276\347\244\272", nullptr));
        label_9->setText(QCoreApplication::translate("BaslerClass", "     \346\225\260  \351\207\217   \357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("BaslerClass", " \344\270\255\345\277\203\347\202\271X\345\200\274\345\201\217\345\267\256\357\274\232", nullptr));
        label_13->setText(QCoreApplication::translate("BaslerClass", " \344\270\255\345\277\203\347\202\271Y\345\200\274\345\201\217\345\267\256\357\274\232", nullptr));
        label_22->setText(QCoreApplication::translate("BaslerClass", " \347\237\251 \345\275\242 \351\225\277 \350\276\271  \357\274\232", nullptr));
        label_23->setText(QCoreApplication::translate("BaslerClass", " \347\237\251 \345\275\242 \347\237\255 \350\276\271  \357\274\232", nullptr));
        label_20->setText(QCoreApplication::translate("BaslerClass", "     \346\226\271  \345\220\221   \357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("BaslerClass", "     \350\247\222  \345\272\246   \357\274\232", nullptr));
        pushButtonOpenCam->setText(QCoreApplication::translate("BaslerClass", "\346\211\223\345\274\200\347\233\270\346\234\272", nullptr));
        pushButtonStartCam->setText(QCoreApplication::translate("BaslerClass", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
        pushButtonSoftTrig->setText(QCoreApplication::translate("BaslerClass", "\350\275\257\344\273\266\350\247\246\345\217\221", nullptr));
        pushButtonStopCam->setText(QCoreApplication::translate("BaslerClass", "\345\201\234\346\255\242\351\207\207\351\233\206", nullptr));
        pushButtonCloseCam->setText(QCoreApplication::translate("BaslerClass", "\345\205\263\351\227\255\347\233\270\346\234\272", nullptr));
        push_close->setText(QCoreApplication::translate("BaslerClass", "\351\200\200\345\207\272\347\250\213\345\272\217", nullptr));
        pushButton_2->setText(QCoreApplication::translate("BaslerClass", "\346\211\223\345\274\200\345\215\201\345\255\227\350\261\241\351\231\220", nullptr));
        pushButton_4->setText(QCoreApplication::translate("BaslerClass", "\345\217\226\346\266\210\345\215\201\345\255\227\350\261\241\351\231\220", nullptr));
        push_show->setText(QCoreApplication::translate("BaslerClass", "\345\274\200\345\247\213\346\240\207\345\256\232", nullptr));
        pushButton->setText(QCoreApplication::translate("BaslerClass", "\346\240\207\345\256\232\345\256\214\346\210\220", nullptr));
        pushButton_5->setText(QCoreApplication::translate("BaslerClass", "\346\230\276\347\244\272\345\233\276\345\203\217\345\233\233\345\221\250\350\276\271\347\225\214\346\241\206", nullptr));
        pushButton_6->setText(QCoreApplication::translate("BaslerClass", "\345\217\226\346\266\210\346\230\276\347\244\272\345\233\276\345\203\217\345\233\233\345\221\250\350\276\271\347\225\214\346\241\206", nullptr));
        label_21->setText(QCoreApplication::translate("BaslerClass", "  \347\233\270\346\234\272\347\272\277\346\266\210\346\212\226\346\227\266\351\227\264\357\274\232", nullptr));
        label_19->setText(QCoreApplication::translate("BaslerClass", "  \346\225\260\346\215\256\346\257\224\344\276\213\347\274\251\346\224\276\345\233\240\345\255\220\357\274\232", nullptr));
        label_24->setText(QCoreApplication::translate("BaslerClass", "  \345\233\276\345\203\217\345\267\246\345\217\263\344\270\244\344\276\247\351\227\264\350\267\235\357\274\232", nullptr));
        label_25->setText(QCoreApplication::translate("BaslerClass", "  \345\233\276\345\203\217\344\270\212\344\270\213\344\270\244\344\276\247\351\227\264\350\267\235\357\274\232", nullptr));
        label_26->setText(QCoreApplication::translate("BaslerClass", "\344\270\244\344\273\266\350\241\243\346\234\215\347\233\270\351\232\224\351\227\264\350\267\235\357\274\232", nullptr));
        label_27->setText(QCoreApplication::translate("BaslerClass", "\350\241\243\346\234\215\351\235\242\347\247\257\345\244\247\345\260\217\357\274\232", nullptr));
        label_KUZI->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BaslerClass: public Ui_BaslerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BASLER_H
