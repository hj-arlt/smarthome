/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tabWidgetPageInfo;
    QProgressBar *pBarTemp;
    QProgressBar *pBarHum;
    QLabel *labelTimeDate;
    QLabel *labelStatus;
    QLabel *labelHum;
    QCheckBox *cBoxIBasic;
    QCheckBox *cBoxITH10;
    QCheckBox *cBoxIGong;
    QLabel *labelTemp;
    QWidget *tabWidgetPageHome;
    QPushButton *pButtonBasic;
    QPushButton *pButtonTH10;
    QPushButton *pButtonKlingel;
    QCheckBox *cBoxBasic;
    QCheckBox *cBoxTH10;
    QCheckBox *cBoxGong;
    QWidget *tabWidgetPageCarport;
    QWidget *tabWidgetPageTimer;
    QPushButton *pButtonTmr;
    QPushButton *pButtonDay;
    QPushButton *pButtonTime;
    QPushButton *pButtonAction;
    QSpinBox *spinBoxTmr;
    QTimeEdit *timeEdit;
    QCheckBox *checkBoxMo;
    QCheckBox *checkBoxDi;
    QCheckBox *checkBoxMi;
    QCheckBox *checkBoxDo;
    QCheckBox *checkBoxFr;
    QCheckBox *checkBoxSa;
    QCheckBox *checkBoxSo;
    QLabel *labelDays;
    QPushButton *pButtonSwitch;
    QCheckBox *checkBoxTmr;
    QCheckBox *checkBoxAction;
    QPushButton *pButtonSwOnoff;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(320, 240);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(10);
        MainWindow->setFont(font);
        MainWindow->setCursor(QCursor(Qt::CrossCursor));
        MainWindow->setTabletTracking(false);
        MainWindow->setFocusPolicy(Qt::NoFocus);
        MainWindow->setAnimated(false);
        MainWindow->setDockOptions(QMainWindow::AllowTabbedDocks);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setEnabled(true);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setMinimumSize(QSize(320, 240));
        centralWidget->setMaximumSize(QSize(320, 240));
        centralWidget->setFont(font);
        centralWidget->setCursor(QCursor(Qt::CrossCursor));
        centralWidget->setTabletTracking(false);
        centralWidget->setFocusPolicy(Qt::NoFocus);
        centralWidget->setAcceptDrops(false);
        centralWidget->setAutoFillBackground(false);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setEnabled(true);
        tabWidget->setGeometry(QRect(0, 0, 320, 240));
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setMaximumSize(QSize(320, 240));
        tabWidget->setFont(font);
        tabWidget->setCursor(QCursor(Qt::CrossCursor));
        tabWidget->setTabletTracking(true);
        tabWidget->setFocusPolicy(Qt::ClickFocus);
        tabWidget->setAutoFillBackground(false);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setUsesScrollButtons(false);
        tabWidgetPageInfo = new QWidget();
        tabWidgetPageInfo->setObjectName(QStringLiteral("tabWidgetPageInfo"));
        tabWidgetPageInfo->setAutoFillBackground(false);
        pBarTemp = new QProgressBar(tabWidgetPageInfo);
        pBarTemp->setObjectName(QStringLiteral("pBarTemp"));
        pBarTemp->setGeometry(QRect(270, 20, 21, 81));
        pBarTemp->setCursor(QCursor(Qt::CrossCursor));
        pBarTemp->setAutoFillBackground(true);
        pBarTemp->setMinimum(-20);
        pBarTemp->setMaximum(40);
        pBarTemp->setValue(22);
        pBarTemp->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);
        pBarTemp->setTextVisible(false);
        pBarTemp->setOrientation(Qt::Vertical);
        pBarTemp->setInvertedAppearance(false);
        pBarTemp->setTextDirection(QProgressBar::TopToBottom);
        pBarHum = new QProgressBar(tabWidgetPageInfo);
        pBarHum->setObjectName(QStringLiteral("pBarHum"));
        pBarHum->setGeometry(QRect(210, 20, 21, 81));
        pBarHum->setCursor(QCursor(Qt::CrossCursor));
        pBarHum->setAutoFillBackground(true);
        pBarHum->setStyleSheet(QStringLiteral(""));
        pBarHum->setMinimum(0);
        pBarHum->setMaximum(100);
        pBarHum->setValue(48);
        pBarHum->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);
        pBarHum->setTextVisible(false);
        pBarHum->setOrientation(Qt::Vertical);
        pBarHum->setInvertedAppearance(false);
        pBarHum->setTextDirection(QProgressBar::TopToBottom);
        labelTimeDate = new QLabel(tabWidgetPageInfo);
        labelTimeDate->setObjectName(QStringLiteral("labelTimeDate"));
        labelTimeDate->setGeometry(QRect(10, 150, 291, 21));
        labelTimeDate->setFrameShape(QFrame::Panel);
        labelTimeDate->setFrameShadow(QFrame::Raised);
        labelTimeDate->setLineWidth(2);
        labelTimeDate->setAlignment(Qt::AlignCenter);
        labelStatus = new QLabel(tabWidgetPageInfo);
        labelStatus->setObjectName(QStringLiteral("labelStatus"));
        labelStatus->setGeometry(QRect(10, 180, 291, 21));
        labelStatus->setFrameShape(QFrame::Box);
        labelStatus->setFrameShadow(QFrame::Sunken);
        labelStatus->setLineWidth(1);
        labelHum = new QLabel(tabWidgetPageInfo);
        labelHum->setObjectName(QStringLiteral("labelHum"));
        labelHum->setGeometry(QRect(180, 110, 61, 20));
        labelHum->setFrameShape(QFrame::Box);
        labelHum->setFrameShadow(QFrame::Raised);
        labelHum->setAlignment(Qt::AlignCenter);
        cBoxIBasic = new QCheckBox(tabWidgetPageInfo);
        cBoxIBasic->setObjectName(QStringLiteral("cBoxIBasic"));
        cBoxIBasic->setGeometry(QRect(20, 30, 81, 23));
        cBoxIBasic->setCheckable(true);
        cBoxIBasic->setTristate(false);
        cBoxITH10 = new QCheckBox(tabWidgetPageInfo);
        cBoxITH10->setObjectName(QStringLiteral("cBoxITH10"));
        cBoxITH10->setGeometry(QRect(20, 60, 81, 23));
        cBoxITH10->setCheckable(true);
        cBoxITH10->setTristate(false);
        cBoxIGong = new QCheckBox(tabWidgetPageInfo);
        cBoxIGong->setObjectName(QStringLiteral("cBoxIGong"));
        cBoxIGong->setGeometry(QRect(20, 90, 81, 23));
        cBoxIGong->setCheckable(true);
        cBoxIGong->setTristate(false);
        labelTemp = new QLabel(tabWidgetPageInfo);
        labelTemp->setObjectName(QStringLiteral("labelTemp"));
        labelTemp->setGeometry(QRect(240, 110, 67, 21));
        labelTemp->setFrameShape(QFrame::Box);
        labelTemp->setFrameShadow(QFrame::Raised);
        labelTemp->setAlignment(Qt::AlignCenter);
        tabWidget->addTab(tabWidgetPageInfo, QString());
        tabWidgetPageHome = new QWidget();
        tabWidgetPageHome->setObjectName(QStringLiteral("tabWidgetPageHome"));
        tabWidgetPageHome->setAutoFillBackground(false);
        pButtonBasic = new QPushButton(tabWidgetPageHome);
        pButtonBasic->setObjectName(QStringLiteral("pButtonBasic"));
        pButtonBasic->setGeometry(QRect(30, 40, 101, 25));
        pButtonTH10 = new QPushButton(tabWidgetPageHome);
        pButtonTH10->setObjectName(QStringLiteral("pButtonTH10"));
        pButtonTH10->setGeometry(QRect(30, 80, 101, 25));
        pButtonKlingel = new QPushButton(tabWidgetPageHome);
        pButtonKlingel->setObjectName(QStringLiteral("pButtonKlingel"));
        pButtonKlingel->setGeometry(QRect(30, 120, 101, 25));
        cBoxBasic = new QCheckBox(tabWidgetPageHome);
        cBoxBasic->setObjectName(QStringLiteral("cBoxBasic"));
        cBoxBasic->setGeometry(QRect(150, 40, 16, 23));
        cBoxBasic->setCheckable(true);
        cBoxBasic->setTristate(true);
        cBoxTH10 = new QCheckBox(tabWidgetPageHome);
        cBoxTH10->setObjectName(QStringLiteral("cBoxTH10"));
        cBoxTH10->setGeometry(QRect(150, 80, 16, 23));
        cBoxTH10->setCheckable(true);
        cBoxTH10->setAutoRepeat(false);
        cBoxTH10->setAutoExclusive(false);
        cBoxTH10->setTristate(false);
        cBoxGong = new QCheckBox(tabWidgetPageHome);
        cBoxGong->setObjectName(QStringLiteral("cBoxGong"));
        cBoxGong->setGeometry(QRect(150, 120, 16, 23));
        cBoxGong->setCheckable(true);
        cBoxGong->setTristate(false);
        tabWidget->addTab(tabWidgetPageHome, QString());
        tabWidgetPageCarport = new QWidget();
        tabWidgetPageCarport->setObjectName(QStringLiteral("tabWidgetPageCarport"));
        tabWidgetPageCarport->setAutoFillBackground(false);
        tabWidget->addTab(tabWidgetPageCarport, QString());
        tabWidgetPageTimer = new QWidget();
        tabWidgetPageTimer->setObjectName(QStringLiteral("tabWidgetPageTimer"));
        tabWidgetPageTimer->setAutoFillBackground(false);
        pButtonTmr = new QPushButton(tabWidgetPageTimer);
        pButtonTmr->setObjectName(QStringLiteral("pButtonTmr"));
        pButtonTmr->setGeometry(QRect(20, 50, 101, 25));
        pButtonDay = new QPushButton(tabWidgetPageTimer);
        pButtonDay->setObjectName(QStringLiteral("pButtonDay"));
        pButtonDay->setGeometry(QRect(20, 90, 101, 25));
        pButtonTime = new QPushButton(tabWidgetPageTimer);
        pButtonTime->setObjectName(QStringLiteral("pButtonTime"));
        pButtonTime->setGeometry(QRect(20, 130, 101, 25));
        pButtonTime->setFocusPolicy(Qt::StrongFocus);
        pButtonAction = new QPushButton(tabWidgetPageTimer);
        pButtonAction->setObjectName(QStringLiteral("pButtonAction"));
        pButtonAction->setEnabled(true);
        pButtonAction->setGeometry(QRect(20, 170, 101, 25));
        pButtonAction->setAutoFillBackground(true);
        spinBoxTmr = new QSpinBox(tabWidgetPageTimer);
        spinBoxTmr->setObjectName(QStringLiteral("spinBoxTmr"));
        spinBoxTmr->setGeometry(QRect(150, 50, 48, 26));
        spinBoxTmr->setMinimum(1);
        spinBoxTmr->setMaximum(16);
        timeEdit = new QTimeEdit(tabWidgetPageTimer);
        timeEdit->setObjectName(QStringLiteral("timeEdit"));
        timeEdit->setGeometry(QRect(150, 130, 61, 26));
        checkBoxMo = new QCheckBox(tabWidgetPageTimer);
        checkBoxMo->setObjectName(QStringLiteral("checkBoxMo"));
        checkBoxMo->setGeometry(QRect(150, 90, 16, 21));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(checkBoxMo->sizePolicy().hasHeightForWidth());
        checkBoxMo->setSizePolicy(sizePolicy1);
        checkBoxMo->setCursor(QCursor(Qt::CrossCursor));
        checkBoxMo->setTabletTracking(true);
        checkBoxMo->setFocusPolicy(Qt::NoFocus);
        checkBoxMo->setContextMenuPolicy(Qt::NoContextMenu);
        checkBoxMo->setAcceptDrops(false);
        checkBoxMo->setAutoFillBackground(true);
        checkBoxMo->setCheckable(true);
        checkBoxMo->setChecked(false);
        checkBoxDi = new QCheckBox(tabWidgetPageTimer);
        checkBoxDi->setObjectName(QStringLiteral("checkBoxDi"));
        checkBoxDi->setGeometry(QRect(170, 90, 16, 21));
        sizePolicy1.setHeightForWidth(checkBoxDi->sizePolicy().hasHeightForWidth());
        checkBoxDi->setSizePolicy(sizePolicy1);
        checkBoxDi->setCursor(QCursor(Qt::CrossCursor));
        checkBoxDi->setTabletTracking(true);
        checkBoxDi->setContextMenuPolicy(Qt::NoContextMenu);
        checkBoxDi->setAcceptDrops(false);
        checkBoxDi->setAutoFillBackground(true);
        checkBoxDi->setCheckable(true);
        checkBoxDi->setChecked(false);
        checkBoxMi = new QCheckBox(tabWidgetPageTimer);
        checkBoxMi->setObjectName(QStringLiteral("checkBoxMi"));
        checkBoxMi->setGeometry(QRect(190, 90, 16, 21));
        sizePolicy1.setHeightForWidth(checkBoxMi->sizePolicy().hasHeightForWidth());
        checkBoxMi->setSizePolicy(sizePolicy1);
        checkBoxMi->setCursor(QCursor(Qt::CrossCursor));
        checkBoxMi->setTabletTracking(true);
        checkBoxMi->setContextMenuPolicy(Qt::NoContextMenu);
        checkBoxMi->setAcceptDrops(false);
        checkBoxMi->setAutoFillBackground(true);
        checkBoxMi->setCheckable(true);
        checkBoxMi->setChecked(false);
        checkBoxDo = new QCheckBox(tabWidgetPageTimer);
        checkBoxDo->setObjectName(QStringLiteral("checkBoxDo"));
        checkBoxDo->setGeometry(QRect(210, 90, 16, 21));
        sizePolicy1.setHeightForWidth(checkBoxDo->sizePolicy().hasHeightForWidth());
        checkBoxDo->setSizePolicy(sizePolicy1);
        checkBoxDo->setCursor(QCursor(Qt::CrossCursor));
        checkBoxDo->setTabletTracking(true);
        checkBoxDo->setContextMenuPolicy(Qt::NoContextMenu);
        checkBoxDo->setAcceptDrops(false);
        checkBoxDo->setAutoFillBackground(true);
        checkBoxDo->setCheckable(true);
        checkBoxDo->setChecked(false);
        checkBoxFr = new QCheckBox(tabWidgetPageTimer);
        checkBoxFr->setObjectName(QStringLiteral("checkBoxFr"));
        checkBoxFr->setGeometry(QRect(230, 90, 16, 21));
        sizePolicy1.setHeightForWidth(checkBoxFr->sizePolicy().hasHeightForWidth());
        checkBoxFr->setSizePolicy(sizePolicy1);
        checkBoxFr->setCursor(QCursor(Qt::CrossCursor));
        checkBoxFr->setTabletTracking(true);
        checkBoxFr->setContextMenuPolicy(Qt::NoContextMenu);
        checkBoxFr->setAcceptDrops(false);
        checkBoxFr->setAutoFillBackground(true);
        checkBoxFr->setCheckable(true);
        checkBoxFr->setChecked(false);
        checkBoxSa = new QCheckBox(tabWidgetPageTimer);
        checkBoxSa->setObjectName(QStringLiteral("checkBoxSa"));
        checkBoxSa->setGeometry(QRect(250, 90, 16, 21));
        sizePolicy1.setHeightForWidth(checkBoxSa->sizePolicy().hasHeightForWidth());
        checkBoxSa->setSizePolicy(sizePolicy1);
        checkBoxSa->setCursor(QCursor(Qt::CrossCursor));
        checkBoxSa->setTabletTracking(true);
        checkBoxSa->setContextMenuPolicy(Qt::NoContextMenu);
        checkBoxSa->setAcceptDrops(false);
        checkBoxSa->setAutoFillBackground(true);
        checkBoxSa->setCheckable(true);
        checkBoxSa->setChecked(false);
        checkBoxSo = new QCheckBox(tabWidgetPageTimer);
        checkBoxSo->setObjectName(QStringLiteral("checkBoxSo"));
        checkBoxSo->setGeometry(QRect(270, 90, 16, 21));
        sizePolicy1.setHeightForWidth(checkBoxSo->sizePolicy().hasHeightForWidth());
        checkBoxSo->setSizePolicy(sizePolicy1);
        checkBoxSo->setCursor(QCursor(Qt::CrossCursor));
        checkBoxSo->setTabletTracking(true);
        checkBoxSo->setContextMenuPolicy(Qt::NoContextMenu);
        checkBoxSo->setAcceptDrops(false);
        checkBoxSo->setAutoFillBackground(true);
        checkBoxSo->setText(QStringLiteral(""));
        checkBoxSo->setCheckable(true);
        checkBoxSo->setChecked(false);
        labelDays = new QLabel(tabWidgetPageTimer);
        labelDays->setObjectName(QStringLiteral("labelDays"));
        labelDays->setGeometry(QRect(150, 110, 141, 17));
        pButtonSwitch = new QPushButton(tabWidgetPageTimer);
        pButtonSwitch->setObjectName(QStringLiteral("pButtonSwitch"));
        pButtonSwitch->setGeometry(QRect(20, 10, 101, 25));
        checkBoxTmr = new QCheckBox(tabWidgetPageTimer);
        checkBoxTmr->setObjectName(QStringLiteral("checkBoxTmr"));
        checkBoxTmr->setGeometry(QRect(230, 50, 51, 23));
        checkBoxTmr->setMouseTracking(false);
        checkBoxTmr->setTabletTracking(false);
        checkBoxTmr->setCheckable(true);
        checkBoxTmr->setTristate(true);
        checkBoxAction = new QCheckBox(tabWidgetPageTimer);
        checkBoxAction->setObjectName(QStringLiteral("checkBoxAction"));
        checkBoxAction->setGeometry(QRect(230, 170, 51, 23));
        checkBoxAction->setCursor(QCursor(Qt::CrossCursor));
        checkBoxAction->setMouseTracking(false);
        checkBoxAction->setCheckable(true);
        checkBoxAction->setTristate(true);
        pButtonSwOnoff = new QPushButton(tabWidgetPageTimer);
        pButtonSwOnoff->setObjectName(QStringLiteral("pButtonSwOnoff"));
        pButtonSwOnoff->setGeometry(QRect(150, 10, 131, 25));
        sizePolicy.setHeightForWidth(pButtonSwOnoff->sizePolicy().hasHeightForWidth());
        pButtonSwOnoff->setSizePolicy(sizePolicy);
        pButtonSwOnoff->setCursor(QCursor(Qt::CrossCursor));
        pButtonSwOnoff->setAutoFillBackground(true);
        pButtonSwOnoff->setFlat(true);
        tabWidget->addTab(tabWidgetPageTimer, QString());
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);
        pButtonSwOnoff->setDefault(true);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        pBarTemp->setFormat(QApplication::translate("MainWindow", "%p \302\260C", Q_NULLPTR));
        pBarHum->setFormat(QApplication::translate("MainWindow", "%p \302\260C", Q_NULLPTR));
        labelTimeDate->setText(QApplication::translate("MainWindow", "Mo, 21.12.2018 / 12:45", Q_NULLPTR));
        labelStatus->setText(QApplication::translate("MainWindow", "Status", Q_NULLPTR));
        labelHum->setText(QApplication::translate("MainWindow", "48.1 %", Q_NULLPTR));
        cBoxIBasic->setText(QApplication::translate("MainWindow", "Basic", Q_NULLPTR));
        cBoxITH10->setText(QApplication::translate("MainWindow", "TH10", Q_NULLPTR));
        cBoxIGong->setText(QApplication::translate("MainWindow", "Klingel", Q_NULLPTR));
        labelTemp->setText(QApplication::translate("MainWindow", "22,1 \302\260C", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabWidgetPageInfo), QApplication::translate("MainWindow", "Info", Q_NULLPTR));
        pButtonBasic->setText(QApplication::translate("MainWindow", "Basic Schalter", Q_NULLPTR));
        pButtonTH10->setText(QApplication::translate("MainWindow", "TH10 Schalter", Q_NULLPTR));
        pButtonKlingel->setText(QApplication::translate("MainWindow", "Klingel", Q_NULLPTR));
        cBoxBasic->setText(QString());
        cBoxTH10->setText(QString());
        cBoxGong->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabWidgetPageHome), QApplication::translate("MainWindow", "Haus", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabWidgetPageCarport), QApplication::translate("MainWindow", "Carport", Q_NULLPTR));
        pButtonTmr->setText(QApplication::translate("MainWindow", "Timer", Q_NULLPTR));
        pButtonDay->setText(QApplication::translate("MainWindow", "Tage", Q_NULLPTR));
        pButtonTime->setText(QApplication::translate("MainWindow", "Zeit", Q_NULLPTR));
        pButtonAction->setText(QApplication::translate("MainWindow", "Action", Q_NULLPTR));
        checkBoxMo->setText(QString());
        checkBoxDi->setText(QString());
        checkBoxMi->setText(QString());
        checkBoxDo->setText(QString());
        checkBoxFr->setText(QString());
        checkBoxSa->setText(QString());
        labelDays->setText(QApplication::translate("MainWindow", "Mo  Di  Mi  Do  Fr  Sa  So", Q_NULLPTR));
        pButtonSwitch->setText(QApplication::translate("MainWindow", "Schalter", Q_NULLPTR));
        checkBoxTmr->setText(QApplication::translate("MainWindow", "aus", Q_NULLPTR));
        checkBoxAction->setText(QApplication::translate("MainWindow", "aus", Q_NULLPTR));
        pButtonSwOnoff->setText(QApplication::translate("MainWindow", "Basic aus", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabWidgetPageTimer), QApplication::translate("MainWindow", "Timer", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
