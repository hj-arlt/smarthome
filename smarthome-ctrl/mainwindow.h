#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDateTime>
#include <QMqttClient>
#include <QThread>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QtCharts>
#include <QDateTimeAxis>
#include <QSplineSeries>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /* mqtt */
    void updateLogStateChange();
    void brokerDisconnected();
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void pingResponse();

    /* gui */
    void on_tabWidget_currentChanged(int index);
    void on_pButtonTmr_clicked();
    void on_pButtonTmrMode_clicked();
    void on_pButtonDay_clicked();
    void on_pButtonTime_clicked();
    void on_pButtonAction_clicked();
    void on_pButtonSwitch_clicked();
    void on_pButtonSwOnoff_clicked();
    void timerUpdate();
    void on_pushButton1_pressed();
    void on_pushButton2_pressed();
    void on_pushButton3_pressed();
    void on_pushButton4_pressed();
    void on_pushButton5_pressed();
    void on_pushButton6_pressed();
    void on_pushButton7_pressed();
    void on_pButtonTmrRepeat_clicked();
    void on_pButtonExit_clicked();
    void on_pButtonSave_clicked();
    void on_checkBoxMo_stateChanged(int arg1);
    void on_checkBoxDi_stateChanged(int arg1);
    void on_checkBoxMi_stateChanged(int arg1);
    void on_checkBoxDo_stateChanged(int arg1);
    void on_checkBoxFr_stateChanged(int arg1);
    void on_checkBoxSa_stateChanged(int arg1);
    void on_checkBoxSo_stateChanged(int arg1);
    void on_timeEdit_userTimeChanged(const QTime &time);
    void on_pButtonZone_clicked();

private:

    enum ir_typ {
        IR_UNKNOWN,
        IR_NEC,
        IR_RC5,
        IR_RC6,
    };

    const QString hostname = "192.168.178.10";
    const quint16 port = 1883;
    const QString MY_LONGITUDE = "12.5757218";
    const QString MY_LATITUDE  = "51.3127998";

    const QString sonoffTopic = "sonoff/#";
    const QString smarthomeTopic = "smarthome/#";
    const QString gongTopic = "smarthome/klingel/taste";
    const QString tempTopic = "smarthome/klingel/temp";
    const QString ledTopic = "smarthome/klingel/led";
    const QString irRecTopic = "smarthome/ir/receiver";
    const QString irSendTopic = "smarthome/ir/sender";

    #define NB_DEVICES 6
    #define NB_ESP8266 1

    const QString device[NB_DEVICES] = {"7236","3432","3424","6816","2531","0616"};
    const QString deviceName[NB_DEVICES] = {"Carport","Multi","Keller","Stube","Büro","Option" };

    const QString powerTopic = "POWER";
    const QString sensorTopic = "SENSOR";
    const QString sensor8Topic = "STATUS8";
    const QString stateTopic = "STATE";
    const QString statusTopic = "STATUS";
    const QString resultTopic = "RESULT";

    const QPixmap pixmapOn = QPixmap(":/icons/kugelbirne_on.png");
    const QPixmap pixmapOff = QPixmap(":/icons/kugelbirne_off.png");
    const QPixmap pixmapIrOn = QPixmap(":/icons/infrared_on.png");
    const QPixmap pixmapIrOff = QPixmap(":/icons/infrared_off.png");
    const QPixmap pixmapTmrOn = QPixmap(":/icons/clock_on.png");
    const QPixmap pixmapTmrOff = QPixmap(":/icons/clock_off.png");

    #define NB_TIMERS 16

    struct timers {
        int mode;
        int hour;
        int min;
        int window;
        int out;
        int action;
        quint8 days;
        quint8 dummy;
        bool repeat;
        bool onoff;
    };

    Ui::MainWindow *ui;

    /* Chart */
    QChart *chart;
    QChartView *chartView;
    QDateTimeAxis *axisX;
    QValueAxis *axisY;
    //QLineSeries *series[NB_DEVICES + NB_ESP8266];
    QSplineSeries *series[NB_DEVICES + NB_ESP8266];
    QPen pen[NB_DEVICES + NB_ESP8266];
    double m_minChart;
    double m_maxChart;
    QDateTime startTime;
    QDateTime nowTime;

    /* Mqtt */
    QMqttClient *m_client;
    bool m_subscribed;

    /* sonoff switchs */
    int m_switchNr;
    bool m_timerGlobal[NB_DEVICES];
    QString m_timerLon[NB_DEVICES];
    QString m_timerLat[NB_DEVICES];
    int m_timerZone[NB_DEVICES];
    int m_timerNr;
    struct timers m_timer[NB_DEVICES][NB_TIMERS];
    QFile m_logFile;
    bool isLog;
    QTextStream m_out;
    QString m_date;

    /* log timer */
    #define TIMER_CYCLE  (60000*5)
    //#define TIMER_CYCLE  (10000)

    QString m_time;
    QString m_t[NB_DEVICES + NB_ESP8266];
    QTimer *m_logTimer;

    /* IR control */
    ir_typ m_irTyp;
    int64_t m_irValue;
    int32_t m_irData;

    void brokerSubscribe(QString topic);
    void brokerPublish(QString topic, QString text);
    void getSonoffStatus();
    void getTimers(int dev);
    void setSwitchState();
    void displayTimer(int index);
    void storeTimer(int index);
    void toggleButton(int index);
    void setLocation(int dev);
    void writeLog();
};

#endif // MAINWINDOW_H
