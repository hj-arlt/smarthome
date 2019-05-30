#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <memory.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCursor(Qt::CrossCursor);

    /* MQTT parts */
    m_client = new QMqttClient;
    m_client->setHostname(hostname);
    m_client->setPort(port);

    /* new style > 5.10 */
    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);
    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic)
             { messageReceived(message, topic); });
    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]()
             { pingResponse(); });

    ui->labelTimeDate->setText(QDateTime::currentDateTime().toString());

    /* timer */
    m_switchNr = 0;
    m_timerNr = 0;
    memset(m_timer, 0, sizeof(struct timers) * NB_DEVICES * NB_TIMERS);

    /* MQTT start */
    m_subscribed = false;
    updateLogStateChange();
    m_client->connectToHost();
}

MainWindow::~MainWindow()
{
    m_client->disconnectFromHost();
    delete ui;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    qDebug() << "Tab changed to" << index;
    if (index == 3) {
        getTimers(m_switchNr);
    }
}

/*
 * Timer
 */
void MainWindow::setSwitchState()
{
    QString dev = deviceName[m_switchNr];
    if (m_timerGlobal[m_switchNr])
         dev.append(" An");
    else dev.append(" Aus");

    ui->pButtonSwOnoff->setFlat(! m_timerGlobal[m_switchNr]);
    ui->pButtonSwOnoff->setText(dev);
}

/* timer 0..15 */
void MainWindow::displayTimer(int index)
{
    bool pbool;

    ui->spinBoxTmr->setValue(index+1);

    if (m_timer[m_switchNr][index].onoff) {
        ui->checkBoxTmr->setChecked(true);
        ui->checkBoxTmr->setText("An");
    } else {
        ui->checkBoxTmr->setChecked(false);
        ui->checkBoxTmr->setText("Aus");
    }
    /* Days */
    pbool = (m_timer[m_switchNr][index].days & 0x40)?true:false;
    ui->checkBoxSo->setChecked(pbool);
    pbool = (m_timer[m_switchNr][index].days & 0x20)?true:false;
    ui->checkBoxMo->setChecked(pbool);
    pbool = (m_timer[m_switchNr][index].days & 0x10)?true:false;
    ui->checkBoxDi->setChecked(pbool);
    pbool = (m_timer[m_switchNr][index].days & 0x08)?true:false;
    ui->checkBoxMi->setChecked(pbool);
    pbool = (m_timer[m_switchNr][index].days & 0x04)?true:false;
    ui->checkBoxDo->setChecked(pbool);
    pbool = (m_timer[m_switchNr][index].days & 0x02)?true:false;
    ui->checkBoxFr->setChecked(pbool);
    pbool = (m_timer[m_switchNr][index].days & 0x01)?true:false;
    ui->checkBoxSa->setChecked(pbool);
    /* Time */
    QTime time;
    time.setHMS(m_timer[m_switchNr][index].hour, m_timer[m_switchNr][index].min, 0, 0);
    ui->timeEdit->setTime(time);
    /* Action */
    ui->checkBoxAction->setChecked(m_timer[m_switchNr][index].action);
    QString txt = "Aus";
    if (m_timer[m_switchNr][index].action == 1)
        txt = "An";
    else if (m_timer[m_switchNr][index].action == 2)
        txt = "Toggle";
    else if (m_timer[m_switchNr][index].action == 3)
        txt = "Blink";
    ui->checkBoxAction->setText(txt);
}

/*
 * mosquitto_pub -h 192.168.178.28 -t sonoff/2531/cmnd/Timer1 -m
 * {"Timer1":{"Arm":1,"Time":"15:55","Window":0,"Days":"1111111","Repeat":1,"Output":1,"Action":1}}
 *
 */
void MainWindow::storeTimer(int index)
{
    QString topic, msg;
    topic.append("sonoff/");
    topic.append(device[index]);
    topic.append("/cmnd/Timer");
    topic.append(QString::number(index));

    msg.append("{\"Timer");
    msg.append(QString::number(index));
    msg.append("\":{\"Arm\":");
    if (m_timer[m_switchNr][index].onoff)
         msg.append("1,\"Time\":\"");
    else msg.append("0,\"Time\":\"");
    msg.append(QString::number(m_timer[m_switchNr][index].hour));
    msg.append(":");
    msg.append(QString::number(m_timer[m_switchNr][index].min));
    msg.append("\",\"Window\":0,\"Days\":\"");
    for (int i=7; i>0; i--) {
        if ((m_timer[m_switchNr][index].days >> i) & 1)
             msg.append("1");
        else msg.append("0");
    }
    msg.append("\",\"Repeat\":1,\"Output\":1,\"Action\":");
    if (m_timer[m_switchNr][index].action)
         msg.append("1");
    else msg.append("0");
    msg.append("}}");

    qDebug() << "pub" << topic << "-m" << msg;
    brokerPublish(topic, msg);

}

/*
 * ************** TIMER TAB ****************
 */
void MainWindow::on_pButtonSwitch_clicked()
{
    if (m_switchNr < NB_DEVICES-1)
        m_switchNr++;
    else m_switchNr = 0;
    getTimers(m_switchNr);

    setSwitchState();
}

/* global timer of switch */
void MainWindow::on_pButtonSwOnoff_clicked()
{
    if (m_timerGlobal[m_switchNr]) {
        m_timerGlobal[m_switchNr] = false;
    } else {
        m_timerGlobal[m_switchNr] = true;
    }
    setSwitchState();
}

void MainWindow::on_pButtonTmr_clicked()
{
    qDebug() << "Button Tmr clicked";
    m_timerNr++;
    if (m_timerNr >= NB_TIMERS)
        m_timerNr = 0;

    displayTimer(m_timerNr);
}

void MainWindow::on_pButtonDay_clicked()
{
    qDebug() << "Button Day clicked";
}

void MainWindow::on_pButtonTime_clicked()
{
    qDebug() << "Button Time clicked";
}

void MainWindow::on_pButtonAction_clicked()
{
    qDebug() << "Button Action clicked";
}

/*
 * ************** HOME TAB ****************
 */
void MainWindow::on_pButtonBasic_clicked()
{
    qDebug() << "Button Basic clicked";

    QString topic;
    topic.append("sonoff/");
    topic.append(device[0]);
    topic.append("/cmnd/POWER");

    brokerPublish(topic, "2");
}

void MainWindow::on_pButtonTH10_clicked()
{
    qDebug() << "Button TH10 clicked";

    QString topic;
    topic.append("sonoff/");
    topic.append(device[1]);
    topic.append("/cmnd/POWER");

    brokerPublish(topic, "2");
}

void MainWindow::on_pButtonKlingel_clicked()
{
    qDebug() << "Button Klingel clicked";

    brokerPublish(gongTopic, "1");
    brokerPublish(gongTopic, "0");
}

/*
 *  ************ MQTT functions *************
 */
void MainWindow::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    qDebug() << content;

    switch (m_client->state())
    {
    case 0: break;
    case 1: break;
    case 2:
            if (! m_subscribed) {
                m_subscribed = true;
                brokerSubscribe(sonoffTopic);
                brokerSubscribe(smarthomeTopic);
                getSonoffStatus();
            }
            break;
    }
}

void MainWindow::brokerDisconnected()
{
    qDebug() << "Broker disconnectet.";
}

void  MainWindow::pingResponse()
{
    const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" PingResponse")
                + QLatin1Char('\n');
    qDebug() << content;
}

void MainWindow::brokerSubscribe(QString topic)
{
    auto subscription = m_client->subscribe((const QMqttTopicFilter) topic);
    if (!subscription) {
        qDebug() << "Could not subscribe. Is there a valid connection?";
    }
}

void MainWindow::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    int idx, bidx, eidx, fidx;
    QByteArray tmr;
    bool pbool;
    QCheckBox *pCheckBox = nullptr;
    QLabel *pLabelT = nullptr;
    QLabel *pLabelH = nullptr;
    QProgressBar *pProgressT = nullptr;
    QProgressBar *pProgressH = nullptr;

    QString content =
//                  QDateTime::currentDateTime().toString()
//                + QLatin1String(" Received Topic: ")
//                +
                  topic.name()
                + QLatin1String(" Message: ")
                + message;

    ui->labelTimeDate->setText(QDateTime::currentDateTime().toString());

    ui->labelStatus->setText(content);

    content += "\n";

    qDebug() << content;

    /*
     * Select module
     */
    if (topic.name().contains(device[0]) == true) {
        pCheckBox = ui->cBoxInfo_1;
        pLabelT = ui->labelTemp_1;
        pProgressT = ui->pBarTemp_1;
    }
    else if (topic.name().contains(device[1]) == true) {
        pCheckBox = ui->cBoxInfo_2;
        pLabelT = ui->labelTemp_2;
        pProgressT = ui->pBarTemp_2;
        pLabelH = ui->labelHum_2;
        pProgressH = ui->pBarHum_2;
    }
    else if (topic.name().contains(device[2]) == true) {
        pCheckBox = ui->cBoxInfo_3;
        pLabelT = ui->labelTemp_3;
        pProgressT = ui->pBarTemp_3;
    }
    else if (topic.name().contains(device[3]) == true) {
        pCheckBox = ui->cBoxInfo_4;
        pLabelT = ui->labelTemp_4;
        pProgressT = ui->pBarTemp_4;
    }
    else if (topic.name().contains(device[4]) == true) {
        pCheckBox = ui->cBoxInfo_5;
        pLabelT = ui->labelTemp_5;
        pProgressT = ui->pBarTemp_5;
    }
    else if (topic.name().contains(device[5]) == true) {
        pCheckBox = ui->cBoxInfo_6;
        pLabelT = ui->labelTemp_6;
        pProgressT = ui->pBarTemp_6;
    }

    /*
     * Switch
     */
    if (topic.name().contains(powerTopic) == true) {
        qDebug() << "POWER ON received";
        pbool = false;
        if (message.contains("ON")) {
            pbool = true;
        }
        else if (message.contains("OFF")) {
            qDebug() << "POWER OFF received";
        }     
        if (pCheckBox) {
            //ui->cBoxBasic->setChecked(pbool);
            pCheckBox->setChecked(pbool);
        }
    }

    /*
     * Label
     */
    if ((topic.name().contains(sensorTopic) == true)
       || (topic.name().contains(sensor8Topic) == true)) {
        QByteArray temp, hum;

        /* temperature */
        idx = message.indexOf("Temperature");
        bidx = message.indexOf(":", idx);
        eidx = message.indexOf(",", bidx);
        fidx = message.indexOf("}", bidx);
        if ((idx < 0) || (bidx < 0))
            return;

        if ((fidx < eidx) && (fidx >= 0)) eidx = fidx;
        temp.append(message.mid(bidx+1), eidx - bidx - 1);
        if (pProgressT)
            pProgressT->setValue(temp.toInt());
        temp.append(" °C");
        if (pLabelT)
            pLabelT->setText(temp);

        qDebug() << "SENSOR received temp" << temp << " - " << temp.toInt();

        idx = message.indexOf("Humidity");
        bidx = message.indexOf(":", idx);
        eidx = message.indexOf(",", bidx);
        fidx = message.indexOf("}", bidx);
        if ((idx < 0) || (bidx < 0))
            return;

        if ((fidx < eidx) && (fidx >= 0)) eidx = fidx;
        hum.append(message.mid(bidx+1), eidx - bidx - 1);
        if (pProgressH)
            pProgressH->setValue(hum.toInt());
        hum.append(" %");
        if (pLabelH)
            pLabelH->setText(hum);

        qDebug() << "SENSOR received hum" << hum << " - " << hum.toInt();
    }

    /*
     * Status
     */
    if (topic.name().contains(statusTopic) == true) {
        QByteArray pow;
        qDebug() << "STATUS received" << QString(message);

        idx = message.indexOf("Power");
        bidx = message.indexOf(":", idx);
        if ((idx < 0) || (bidx < 0))
            return;

        pow.append(message.mid(bidx+1), 1);
        pbool = false;
        if (pow == "1")
            pbool = true;
        else if (pow != "0")
            return;

        qDebug() << "STATUS received" << pow;

        if (pCheckBox) {
            //ui->cBoxBasic->setChecked(pbool);
            pCheckBox->setChecked(pbool);
        }
    }
    if (topic.name().contains(stateTopic) == true) {
        qDebug() << "STATE received";
    }
    /*
     * timers result, m_switchNr, m_timerNr
     *
     * {\"Timers1\":
     *     {\"Timer1\":
     *         {\"Arm\":1,
     *          \"Mode\":0,
     *          \"Time\":\"15:55\",
     *          \"Window\":0,
     *          \"Days\":\"1111111\",
     *          \"Repeat\":1,
     *          \"Output\":1,
     *          \"Action\":1},
     */
    if (topic.name().contains(resultTopic) == true) {
        if ((message.indexOf("Timers1") >= 0)
           || (message.indexOf("Timers2") >= 0)
           || (message.indexOf("Timers3") >= 0)
           || (message.indexOf("Timers4") >= 0))
        {
            int ivar;
            bidx = message.indexOf("Timers");
            idx = message.indexOf("{", bidx);

            /* timer 1..16 */
            while (idx >= 0) {
                bidx = message.indexOf("Timer", idx);
                tmr.clear();
                tmr.append(message.mid(bidx+5), 2);
                ivar = tmr.toInt();
                if (ivar == 0) {
                    tmr.clear();
                    tmr.append(message.mid(bidx+5), 1);
                    ivar = tmr.toInt();
                }
                /* 0..15 */
                ivar--;
                //qDebug() << "Result TIMER" << ivar << bidx;

                if ((ivar >= 0) && (ivar < NB_TIMERS)) {
                    /* Arm */
                    eidx = message.indexOf("Arm", bidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+1), 1);
                        if (tmr == "1")
                             m_timer[m_switchNr][ivar].onoff = true;
                        else m_timer[m_switchNr][ivar].onoff = false;
                        qDebug() << "Result TIMER" << ivar << "onoff" << tmr;
                    }
                    /* Mode */
                    eidx = message.indexOf("Mode", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+1), 1);
                        m_timer[m_switchNr][ivar].mode = (quint8)tmr.toInt();
                        qDebug() << "Result TIMER" << ivar << "mode" << tmr;
                    }
                    /* Time */
                    eidx = message.indexOf("Time", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+2), 2);
                        m_timer[m_switchNr][ivar].hour = tmr.toInt();
                        qDebug() << "Result TIMER" << ivar << "std" << tmr << m_timer[m_switchNr][ivar].hour;
                        tmr.clear();
                        tmr.append(message.mid(fidx+5
                                               ), 2);
                        m_timer[m_switchNr][ivar].min = tmr.toInt();
                        qDebug() << "Result TIMER" << ivar << "min" << tmr << m_timer[m_switchNr][ivar].min;
                    }
                    /* Window = 0 */
                    eidx = message.indexOf("Window", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+1), 1);
                        qDebug() << "Result TIMER" << ivar << "window" << tmr;
                    }
                    /* Days */
                    eidx = message.indexOf("Days", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        m_timer[m_switchNr][ivar].days = 0;
                        tmr.clear();
                        tmr.append(message.mid(fidx+2), 1);
                        if (tmr != "0")
                            m_timer[m_switchNr][ivar].days |= 0x40; // So
                        tmr.clear();
                        tmr.append(message.mid(fidx+3), 1);
                        if (tmr != "0")
                            m_timer[m_switchNr][ivar].days |= 0x20; // Mo
                        tmr.clear();
                        tmr.append(message.mid(fidx+4), 1);
                        if (tmr != "0")
                            m_timer[m_switchNr][ivar].days |= 0x10;
                        tmr.clear();
                        tmr.append(message.mid(fidx+5), 1);
                        if (tmr != "0")
                            m_timer[m_switchNr][ivar].days |= 0x08;
                        tmr.clear();
                        tmr.append(message.mid(fidx+6), 1);
                        if (tmr != "0")
                            m_timer[m_switchNr][ivar].days |= 0x04;
                        tmr.clear();
                        tmr.append(message.mid(fidx+7), 1);
                        if (tmr != "0")
                            m_timer[m_switchNr][ivar].days |= 0x02; // Fr
                        tmr.clear();
                        tmr.append(message.mid(fidx+8), 1);
                        if (tmr != "0")
                            m_timer[m_switchNr][ivar].days |= 0x01; // Sa
                        qDebug() << "Result TIMER" << ivar << "days" << m_timer[m_switchNr][ivar].days;
                    }
                    /* Repeat */
                    eidx = message.indexOf("Repeat", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+1), 1);
                        if (tmr == "0")
                             m_timer[m_switchNr][ivar].repeat = false;
                        else m_timer[m_switchNr][ivar].repeat = true;
                        qDebug() << "Result TIMER" << ivar << "repeat" << tmr;
                    }
                    /* Output 1..16 */
                    eidx = message.indexOf("Output", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+1), 1);
                        m_timer[m_switchNr][ivar].out = tmr.toInt();
                        qDebug() << "Result TIMER" << ivar << "out" << tmr;
                    }
                    /* Action 0..3 (2 toggle, 3 blink) */
                    eidx = message.indexOf("Action", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+1), 1);
                        m_timer[m_switchNr][ivar].action = tmr.toInt();
                        qDebug() << "Result TIMER" << ivar << "action" << tmr;
                    }
                    idx = message.indexOf("}", eidx);
                } else {
                    idx = -1;
                }
            }

            /* display results */
            displayTimer(m_timerNr);
        }
        /*
         * {\"Timers\":\"OFF\"}\n"
         */
        else if (message.indexOf("Timers") >= 0) {
            bidx = message.indexOf("Timers");
            //idx = message.indexOf(":", bidx);
            idx = message.indexOf(":");
            eidx = message.indexOf("}", bidx);
            if ((bidx < 0) || (eidx < 0))
                return;
            tmr.clear();
            tmr.append(message.mid(bidx+3), 2);
            pbool = false;
            if (tmr == "ON") {
                pbool = true;
            }
            qDebug() << "Result received TIMERS.." << tmr;

            m_timerGlobal[m_switchNr] = pbool;
            setSwitchState();
        }
    }
}

void MainWindow::brokerPublish(QString topic, QString text)
{
    if (m_client->state() < 2)
        return;

    if (m_client->publish((const QMqttTopicName)topic, text.toUtf8()) == -1) {
        qDebug()<< "Could not publish message";
        return;
    }
    QThread::msleep(500);
}

void MainWindow::getSonoffStatus()
{
    QString topic;
    for (int i=0; i<NB_DEVICES; i++) {
        topic.clear();
        topic.append("sonoff/");
        topic.append(device[i]);
        topic.append("/cmnd/STATUS");

        brokerPublish(topic, "");

        topic.clear();
        topic.append("sonoff/");
        topic.append(device[i]);
        topic.append("/cmnd/STATUS");

        brokerPublish(topic, "8");
    }
}

void MainWindow::getTimers(int dev)
{
    QString topic;

    topic.clear();
    topic.append("sonoff/");
    topic.append(device[dev]);
    topic.append("/cmnd/TIMERS");

    brokerPublish(topic, "");
}

/*
 * sonoff/2531/cmnd/Timer1 -m
 * {"Timer1":
 *     {"Arm":1,"Time":"15:55","Window":0,"Days":"1111111",
 *      "Repeat":1,"Output":1,"Action":1}
 * }
 */
void MainWindow::setTimer(int dev)
{
    QString topic;

    topic.clear();
    topic.append("sonoff/");
    topic.append(device[dev]);
    topic.append("/cmnd/TIMER");
    topic.append(QString::number(dev));

}
