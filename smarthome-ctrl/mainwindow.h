#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDateTime>
#include <QMqttClient>
#include <QThread>

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

    void on_pButtonDay_clicked();

    void on_pButtonTime_clicked();

    void on_pButtonAction_clicked();

    void on_pButtonSwitch_clicked();

    void on_pButtonBasic_clicked();

    void on_pButtonTH10_clicked();

    void on_pButtonKlingel_clicked();

    void on_pButtonSwOnoff_clicked();

private:

    const QString hostname = "192.168.178.28";
    const quint16 port = 1883;

    const QString sonoffTopic = "sonoff/#";
    const QString smarthomeTopic = "smarthome/#";
    const QString gongTopic = "smarthome/klingel/taste";

    #define NB_DEVICES 6
    /*                          basic   th10 */
    const QString device[NB_DEVICES] = {"0616", "2531", "3424", "3432", "6816", "7236"};
    const QString deviceName[NB_DEVICES] = {"Basic1", "TH10","Basic3","Basic4","Basic5","Basic6"};

    const QString powerTopic = "POWER";
    const QString sensorTopic = "SENSOR";
    const QString sensor8Topic = "STATUS8";
    const QString stateTopic = "STATE";
    const QString statusTopic = "STATUS";
    const QString resultTopic = "RESULT";

    #define NB_TIMERS 16

    struct timers {
        bool onoff;
        int mode;
        quint8 days;
        int hour;
        int min;
        bool repeat;
        int out;
        int action;
    };

    Ui::MainWindow *ui;
    QMqttClient *m_client;
    bool m_subscribed;

    int m_switchNr;
    bool m_timerGlobal[NB_DEVICES];
    int m_timerNr;
    struct timers m_timer[NB_DEVICES][NB_TIMERS];

    void brokerSubscribe(QString topic);
    void brokerPublish(QString topic, QString text);
    void getSonoffStatus();
    void getTimers(int dev);
    void setTimer(int dev);
    void setSwitchState();
    void displayTimer(int index);
    void storeTimer(int index);
};

#endif // MAINWINDOW_H
