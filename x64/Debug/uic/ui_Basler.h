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
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout;
    QLabel *label_KUZI;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_7;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QSlider *horizontalSliderExp;
    QSpinBox *spinBoxExp;
    QSpacerItem *verticalSpacer_2;
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
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QLineEdit *lineEdit_Angle;
    QScrollArea *scrollArea_3;
    QWidget *scrollAreaWidgetContents_3;
    QHBoxLayout *horizontalLayout_12;
    QPushButton *pushButtonOpenCam;
    QPushButton *pushButtonStartCam;
    QPushButton *pushButtonSoftTrig;
    QPushButton *pushButtonStopCam;
    QPushButton *pushButtonCloseCam;
    QPushButton *push_close;
    QScrollArea *scrollArea_4;
    QWidget *scrollAreaWidgetContents_4;
    QHBoxLayout *horizontalLayout_13;
    QPushButton *LED;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BaslerClass)
    {
        if (BaslerClass->objectName().isEmpty())
            BaslerClass->setObjectName(QString::fromUtf8("BaslerClass"));
        BaslerClass->resize(1022, 667);
        centralWidget = new QWidget(BaslerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(0, 0, 741, 551));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 739, 549));
        horizontalLayout = new QHBoxLayout(scrollAreaWidgetContents);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_KUZI = new QLabel(scrollAreaWidgetContents);
        label_KUZI->setObjectName(QString::fromUtf8("label_KUZI"));

        horizontalLayout->addWidget(label_KUZI);

        scrollArea->setWidget(scrollAreaWidgetContents);
        scrollArea_2 = new QScrollArea(centralWidget);
        scrollArea_2->setObjectName(QString::fromUtf8("scrollArea_2"));
        scrollArea_2->setGeometry(QRect(740, 0, 271, 551));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 269, 549));
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
        horizontalSliderExp->setMinimum(500);
        horizontalSliderExp->setMaximum(30000);
        horizontalSliderExp->setSingleStep(100);
        horizontalSliderExp->setPageStep(100);
        horizontalSliderExp->setValue(3000);
        horizontalSliderExp->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(horizontalSliderExp);

        spinBoxExp = new QSpinBox(scrollAreaWidgetContents_2);
        spinBoxExp->setObjectName(QString::fromUtf8("spinBoxExp"));
        spinBoxExp->setMinimum(500);
        spinBoxExp->setMaximum(30000);
        spinBoxExp->setSingleStep(100);
        spinBoxExp->setValue(3000);

        horizontalLayout_7->addWidget(spinBoxExp);


        verticalLayout_3->addLayout(horizontalLayout_7);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

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
        scrollArea_3->setGeometry(QRect(0, 550, 741, 61));
        scrollArea_3->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_3"));
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 739, 59));
        horizontalLayout_12 = new QHBoxLayout(scrollAreaWidgetContents_3);
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        pushButtonOpenCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonOpenCam->setObjectName(QString::fromUtf8("pushButtonOpenCam"));

        horizontalLayout_12->addWidget(pushButtonOpenCam);

        pushButtonStartCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonStartCam->setObjectName(QString::fromUtf8("pushButtonStartCam"));

        horizontalLayout_12->addWidget(pushButtonStartCam);

        pushButtonSoftTrig = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonSoftTrig->setObjectName(QString::fromUtf8("pushButtonSoftTrig"));

        horizontalLayout_12->addWidget(pushButtonSoftTrig);

        pushButtonStopCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonStopCam->setObjectName(QString::fromUtf8("pushButtonStopCam"));

        horizontalLayout_12->addWidget(pushButtonStopCam);

        pushButtonCloseCam = new QPushButton(scrollAreaWidgetContents_3);
        pushButtonCloseCam->setObjectName(QString::fromUtf8("pushButtonCloseCam"));

        horizontalLayout_12->addWidget(pushButtonCloseCam);

        push_close = new QPushButton(scrollAreaWidgetContents_3);
        push_close->setObjectName(QString::fromUtf8("push_close"));

        horizontalLayout_12->addWidget(push_close);

        scrollArea_3->setWidget(scrollAreaWidgetContents_3);
        scrollArea_4 = new QScrollArea(centralWidget);
        scrollArea_4->setObjectName(QString::fromUtf8("scrollArea_4"));
        scrollArea_4->setGeometry(QRect(740, 550, 271, 61));
        scrollArea_4->setWidgetResizable(true);
        scrollAreaWidgetContents_4 = new QWidget();
        scrollAreaWidgetContents_4->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_4"));
        scrollAreaWidgetContents_4->setGeometry(QRect(0, 0, 269, 59));
        horizontalLayout_13 = new QHBoxLayout(scrollAreaWidgetContents_4);
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        LED = new QPushButton(scrollAreaWidgetContents_4);
        LED->setObjectName(QString::fromUtf8("LED"));

        horizontalLayout_13->addWidget(LED);

        scrollArea_4->setWidget(scrollAreaWidgetContents_4);
        BaslerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BaslerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1022, 22));
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

        QMetaObject::connectSlotsByName(BaslerClass);
    } // setupUi

    void retranslateUi(QMainWindow *BaslerClass)
    {
        BaslerClass->setWindowTitle(QCoreApplication::translate("BaslerClass", "Basler", nullptr));
        label_KUZI->setText(QString());
        label_7->setText(QCoreApplication::translate("BaslerClass", "\347\233\270\346\234\272\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label_8->setText(QCoreApplication::translate("BaslerClass", "\346\233\235\345\205\211\346\227\266\351\227\264\357\274\232", nullptr));
        label->setText(QCoreApplication::translate("BaslerClass", "\351\200\232\350\256\257\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label_4->setText(QCoreApplication::translate("BaslerClass", "PLC IP\357\274\232", nullptr));
        label_5->setText(QCoreApplication::translate("BaslerClass", "\347\253\257\345\217\243\345\217\267\357\274\232", nullptr));
        pushButton_ConnectPLC->setText(QCoreApplication::translate("BaslerClass", "PLC\350\277\236\346\216\245", nullptr));
        pushButton_DisConnectPLC->setText(QCoreApplication::translate("BaslerClass", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label_6->setText(QCoreApplication::translate("BaslerClass", "\350\247\246\345\217\221\346\250\241\345\274\217\350\256\276\347\275\256", nullptr));
        label_10->setText(QCoreApplication::translate("BaslerClass", "\350\247\246 \345\217\221 \346\272\220\357\274\232", nullptr));
        label_11->setText(QCoreApplication::translate("BaslerClass", "\350\247\246\345\217\221\346\250\241\345\274\217\357\274\232", nullptr));
        label_12->setText(QCoreApplication::translate("BaslerClass", "\346\225\260\346\215\256\346\230\276\347\244\272", nullptr));
        label_9->setText(QCoreApplication::translate("BaslerClass", " \346\225\260  \351\207\217 \357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("BaslerClass", " \344\270\255\345\277\203\347\202\271X\357\274\232", nullptr));
        label_13->setText(QCoreApplication::translate("BaslerClass", " \344\270\255\345\277\203\347\202\271Y\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("BaslerClass", " \350\247\222  \345\272\246 \357\274\232", nullptr));
        pushButtonOpenCam->setText(QCoreApplication::translate("BaslerClass", "\346\211\223\345\274\200\347\233\270\346\234\272", nullptr));
        pushButtonStartCam->setText(QCoreApplication::translate("BaslerClass", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
        pushButtonSoftTrig->setText(QCoreApplication::translate("BaslerClass", "\350\275\257\344\273\266\350\247\246\345\217\221", nullptr));
        pushButtonStopCam->setText(QCoreApplication::translate("BaslerClass", "\345\201\234\346\255\242\351\207\207\351\233\206", nullptr));
        pushButtonCloseCam->setText(QCoreApplication::translate("BaslerClass", "\345\205\263\351\227\255\347\233\270\346\234\272", nullptr));
        push_close->setText(QCoreApplication::translate("BaslerClass", "\351\200\200\345\207\272\347\250\213\345\272\217", nullptr));
        LED->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BaslerClass: public Ui_BaslerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BASLER_H
