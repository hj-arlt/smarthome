#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <memory.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int i;
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

    /* log file */
    QDate cd = QDate::currentDate();
    QTime ct = QTime::currentTime();
    QString fileName = "log_";
    fileName.append(cd.toString("dd-MM-yy"));
    fileName.append("_");
    fileName.append(ct.toString("hh:mm"));
    fileName.append(".txt");
    qDebug() << "Log file:" << fileName;

    m_logFile.setFileName(fileName);
    // Trying to open in WriteOnly and Text mode
    if(!m_logFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << " Could not open file for writing" << fileName;
    } else {
        isLog = true;
    }
    if (isLog) {
        m_out.setDevice(&m_logFile);
        m_out << "Date; Time; ";
        for (i=0; i<NB_DEVICES; i++) {
            m_out << deviceName[i] << "; ";
        }
        m_out << "Klingel;\n";
    }
    ui->labelStatus->setText("Date; Time; T0; T1; T2; T3, T4, T5");
    for (i=0; i<NB_DEVICES+1; i++) {
        m_t[i] = "20.0";
    }

    /*
     *   Temperatur Chart
     */
    m_minChart = 50.0;
    m_maxChart = -50.0;

    /* Chart, Theme
     *
     *  themeComboBox->addItem("Light", QChart::ChartThemeLight);
        themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
        themeComboBox->addItem("Dark", QChart::ChartThemeDark);
        themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
        themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
        themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
        themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
        themeComboBox->addItem("Qt", QChart::ChartThemeQt);
     */
    chart = new QChart();
    chart->legend()->hide();
    chart->setTitle("Temperatur Diagramm");
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(QChart::ChartThemeLight);
    chart->setTheme(theme);
    /*
     *  animationComboBox->addItem("No Animations", QChart::NoAnimation);
        animationComboBox->addItem("GridAxis Animations", QChart::GridAxisAnimations);
        animationComboBox->addItem("Series Animations", QChart::SeriesAnimations);
        animationComboBox->addItem("All Animations", QChart::AllAnimations);
     */
    chart->setAnimationOptions(QChart::AllAnimations);
    /* colors */

    /* X Achse */
    startTime = QDateTime::currentDateTime();

    axisX = new QDateTimeAxis; //QValueAxis;
    axisX->setLinePenColor(Qt::green);
    axisX->setFormat("dd-MM hh:mm");
    //axisX->setLabelFormat("dd-MM-yyyy h:mm");
    //axisX->setTitleText("Date");
    //axisX->setTickCount(10); // how many gridline
    axisX->setMin(startTime);
    chart->addAxis(axisX, Qt::AlignBottom);

    /* Y left Achse */
    axisY = new QValueAxis;
    axisY->setLinePenColor(Qt::blue);
    axisY->setLabelFormat("%.1f C");
    axisY->setRange(-10.0, 40.0);
    axisY->setTickCount(9);
    chart->addAxis(axisY, Qt::AlignLeft);

    /* Y right Achse */
    /* pen definition
    QPen pen(Qt::red);
    pen.setCapStyle(Qt::RoundCap);
    QCategoryAxis *axisY3 = new QCategoryAxis;
    axisY3->append("Low", 0);
    axisY3->append("Medium", 15);
    axisY3->append("High", 30);
    axisY3->setLinePenColor(Qt::red);
    axisY3->setGridLinePen(pen);
    chart->addAxis(axisY3, Qt::AlignRight); */

    /* Serie 1..6 */
    for (int i=0; i<NB_DEVICES+NB_ESP8266; i++) {
        pen[i].setColor(Qt::GlobalColor(i+6));
        pen[i].setWidth(2);

        series[i] = new QSplineSeries;
        series[i]->setPen(pen[i]);

        chart->addSeries(series[i]);

        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
    }

    /* graphicView bind to class chart in ui */
    ui->graphicsView->setChart(chart);

    /* Timer */
    m_logTimer = new QTimer(this);
    connect(m_logTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    m_logTimer->start(TIMER_CYCLE);

    /* MQTT start */
    m_subscribed = false;
    updateLogStateChange();
    m_client->connectToHost();

    writeLog();
}

MainWindow::~MainWindow()
{
    m_client->disconnectFromHost();

    writeLog();
    m_logFile.close();
    delete ui;
}

void MainWindow::timerUpdate()
{
    writeLog();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    qDebug() << "Tab changed to" << index;
    /* timer */
    if (index == 1) {
        m_switchNr = 0;
        m_timerNr = 0;
        for (int j=0; j<NB_DEVICES; j++) {
            for (int i=0; i<NB_TIMERS; i++) {
                memset(&m_timer[j][i], 0, sizeof(struct timers));
            }
        }
        setLocation(m_switchNr);
        getTimers(m_switchNr);
    } else {

    }
}

/*
 * Timer
 *
 * Global on/off
 */
void MainWindow::setSwitchState()
{
    QString dev = deviceName[m_switchNr];
    /* Icon */
    if (m_timerGlobal[m_switchNr])
          ui->labelSwitchOnOff->setPixmap(pixmapTmrOn);
    else  ui->labelSwitchOnOff->setPixmap(pixmapTmrOff);
    /* Button */
    ui->pButtonSwOnoff->setFlat(! m_timerGlobal[m_switchNr]);
    ui->pButtonSwOnoff->setText(dev);
    /* Lon, Lat */
    ui->labelLatitude->setText(m_timerLat[m_switchNr]);
    ui->labelLongitude->setText(m_timerLon[m_switchNr]);
    /* Zone */
    QString zone;
    if (m_timerZone[m_switchNr] >= 0)
        zone.append("+");
    zone.append(QString::number(m_timerZone[m_switchNr]));
    zone.append(":00");
    ui->pButtonZone->setText(zone);
}

/* timer 0..15 */
void MainWindow::displayTimer(int index)
{
    bool pbool;
    QString txt;

    /* Nr. spin */
    ui->spinBoxTmr->setValue(index+1);

    /* Timer 1..16 enabled */
    if (m_timer[m_switchNr][index].onoff) {
        ui->labelTimerOnOff->setPixmap(pixmapTmrOn);
    } else {
        ui->labelTimerOnOff->setPixmap(pixmapTmrOff);
    }
    /* Mode */
    txt = "Uhr";
    if (m_timer[m_switchNr][index].mode == 1)
        txt = "Sonne auf";
    else if (m_timer[m_switchNr][index].mode == 2)
        txt = "Sonne unter";
    /* label with mode */
    ui->pButtonTmrMode->setText(txt);

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

    /* Repeat */
    if (m_timer[m_switchNr][index].repeat)
          ui->pButtonTmrRepeat->setText("Wiederholt");
    else  ui->pButtonTmrRepeat->setText("Einmal");

    /* Action */
    txt = "Aus";
    if (m_timer[m_switchNr][index].action == 1)
        txt = "An";
    else if (m_timer[m_switchNr][index].action == 2)
        txt = "Toggle";
    else if (m_timer[m_switchNr][index].action == 3)
        txt = "Blink";
    ui->labelActionMode->setText(txt);
    if (txt != "Aus") {
        ui->labelActionOnOff->setPixmap(pixmapOn);
    } else {
        ui->labelActionOnOff->setPixmap(pixmapOff);
    }
}

/*
 * Store Timer
 *
 * mosquitto_pub -h 192.168.178.28 -t sonoff/2531/cmnd/Timer1 -m
 * {"Timer1":{"Arm":1,"Time":"15:55","Window":0,"Days":"1111111","Repeat":1,"Output":1,"Action":1}}
 */
void MainWindow::storeTimer(int dev)
{
    int i = dev;
    QString topic, message;

    /* timezone */
    topic.clear();
    topic.append("sonoff/");
    topic.append(device[dev]);
    topic.append("/cmnd/Timezone");
    message.clear();
    message.append(QString::number(m_timerZone[m_switchNr]));

    qDebug() << "Store TimeZone:" << topic << message;

    brokerPublish(topic, message);

    /* global on/off */
    topic.clear();
    topic.append("sonoff/");
    topic.append(device[dev]);
    topic.append("/cmnd/Timers");
    message.clear();

    for (i=0; i<NB_TIMERS; i++) {
        if (m_timer[m_switchNr][i].onoff)
            break;
    }
    if (i < NB_TIMERS)
          message = "1";
    else  message = "0";

    qDebug() << "Store Timers:" << topic << message;

    brokerPublish(topic, message);

    for (i=0; i<NB_TIMERS; i++) {

        topic.clear();
        topic.append("sonoff/");
        topic.append(device[dev]);
        topic.append("/cmnd/Timer");
        topic.append(QString::number(i+1));

        /*
         Timer1 {\"Arm\":0,\"Time\":\"0:0\",\"Window\":0,\"Days\":\"0000000\",\"Repeat\":0,\"Output\":1,\"Action\":0}
         */
        message.clear();
        message.append("{\"Arm\":");
        message.append(QString::number(m_timer[m_switchNr][i].onoff));
        message.append(",\"Time\":\"");
        message.append(QString::number(m_timer[m_switchNr][i].hour));
        message.append(":");
        message.append(QString::number(m_timer[m_switchNr][i].min));
        message.append("\",\"Window\":");
        message.append(QString::number(m_timer[m_switchNr][i].window));
        message.append(",\"Days\":\"");
        for (int j=6; j>=0; j--) {
            if ((m_timer[m_switchNr][i].days >> j) & 1)
                 message.append("1");
            else message.append("0");
        }
        message.append("\",\"Repeat\":");
        message.append(QString::number(m_timer[m_switchNr][i].repeat));
        message.append(",\"Output\":");
        message.append(QString::number(m_timer[m_switchNr][i].out));
        message.append(",\"Action\":");
        message.append(QString::number(m_timer[m_switchNr][i].action));
        message.append("}");

        qDebug() << "Store Timer:" << topic << message;

        brokerPublish(topic, message);
    }
}

/*
 * ************** INFO TAB ****************
 */
void MainWindow::on_pushButton1_pressed()
{
    qDebug() << "Button1 pressed";
    toggleButton(0);
}

void MainWindow::on_pushButton2_pressed()
{
    qDebug() << "Button2 pressed";
    toggleButton(1);
}

void MainWindow::on_pushButton3_pressed()
{
    qDebug() << "Button3 pressed";
    toggleButton(2);
}

void MainWindow::on_pushButton4_pressed()
{
    qDebug() << "Button4 pressed";
    toggleButton(3);
}

void MainWindow::on_pushButton5_pressed()
{
    qDebug() << "Button5 pressed";
    toggleButton(4);
}

void MainWindow::on_pushButton6_pressed()
{
    qDebug() << "Button6 pressed";
    toggleButton(5);
}

void MainWindow::on_pushButton7_pressed()
{
    qDebug() << "Button7 Klingel pressed";
    toggleButton(6);
}

/*
 * ************** TIMER TAB ****************
 */
/* timer switch changed */
void MainWindow::on_pButtonSwitch_clicked()
{
    //storeTimer(m_switchNr);

    if (m_switchNr < NB_DEVICES-1)
        m_switchNr++;
    else m_switchNr = 0;

    setLocation(m_switchNr);

    getTimers(m_switchNr);
    m_timerNr = 0;

    qDebug() << "Button Tmr switch to" << device[m_switchNr];

    setSwitchState();
}

/* global timer on/off switch */
void MainWindow::on_pButtonSwOnoff_clicked()
{
    if (m_timerGlobal[m_switchNr]) {
        m_timerGlobal[m_switchNr] = false;
        m_timer[m_switchNr][m_timerNr].onoff = 0;
    } else {
        m_timerGlobal[m_switchNr] = true;
    }
    m_timerNr = 0;

    setSwitchState();
}

/* timer nr. changed */
void MainWindow::on_pButtonTmr_clicked()
{
    m_timerNr++;
    if (m_timerNr >= NB_TIMERS)
        m_timerNr = 0;
    qDebug() << "Button Tmr nr. changed to" << m_timerNr;

    displayTimer(m_timerNr);
}

void MainWindow::on_pButtonTmrMode_clicked()
{
    int md = m_timer[m_switchNr][m_timerNr].mode;
    md++;
    if (md > 2)
        md = 0;
    m_timer[m_switchNr][m_timerNr].mode = md;
    /* enabled now */
    m_timer[m_switchNr][m_timerNr].onoff = 1;

    qDebug() << "Button Timer mode to" << md;

    displayTimer(m_timerNr);
}

/* mo..so einzeln, alle, sa,so */
void MainWindow::on_pButtonDay_clicked()
{
    /* 0x40; // So
     * -,So,Mo,Di Mi,Do,Fr,Sa
     */
    switch (m_timer[m_switchNr][m_timerNr].days) {
    case 0:    m_timer[m_switchNr][m_timerNr].days = 0x20; break;
    case 0x20: m_timer[m_switchNr][m_timerNr].days = 0x10; break;
    case 0x10: m_timer[m_switchNr][m_timerNr].days = 0x08; break;
    case 0x08: m_timer[m_switchNr][m_timerNr].days = 0x04; break;
    case 0x04: m_timer[m_switchNr][m_timerNr].days = 0x02; break;
    case 0x02: m_timer[m_switchNr][m_timerNr].days = 0x01; break;
    case 0x01: m_timer[m_switchNr][m_timerNr].days = 0x40; break;
    case 0x40: m_timer[m_switchNr][m_timerNr].days = 0x7F; break;
    case 0x7F: m_timer[m_switchNr][m_timerNr].days = 0x3E; break;
    case 0x3E: m_timer[m_switchNr][m_timerNr].days = 0x41; break;
    case 0x41: m_timer[m_switchNr][m_timerNr].days = 0x43; break;
    case 0x43: m_timer[m_switchNr][m_timerNr].days = 0x0; break;
    default:   m_timer[m_switchNr][m_timerNr].days = 0x0; break;
    }
    qDebug() << "Button Tmr Day clicked";

    displayTimer(m_timerNr);
}

void MainWindow::on_pButtonTime_clicked()
{
    qDebug() << "Button Tmr time clicked";

    displayTimer(m_timerNr);
}

void MainWindow::on_pButtonTmrRepeat_clicked()
{
    if (m_timer[m_switchNr][m_timerNr].repeat) {
        m_timer[m_switchNr][m_timerNr].repeat = 0;
    } else {
        m_timer[m_switchNr][m_timerNr].repeat = 1;
    }
    qDebug() << "Button Tmr repeat clicked" << m_timer[m_switchNr][m_timerNr].repeat;

    displayTimer(m_timerNr);
}

void MainWindow::on_pButtonAction_clicked()
{
    int act = m_timer[m_switchNr][m_timerNr].action;
    act++;
    if (act > 3)
        act = 0;
    m_timer[m_switchNr][m_timerNr].action = act;
    qDebug() << "Button Tmr Action clicked" << act;

    displayTimer(m_timerNr);
}

void MainWindow::on_pButtonZone_clicked()
{
    if (m_timerZone[m_switchNr] == 2)
          m_timerZone[m_switchNr] = 1;
    else  m_timerZone[m_switchNr] = 2;
    qDebug() << "Button Zone clicked dev" << m_switchNr << "to" << m_timerZone[m_switchNr];

    setSwitchState();
}

void MainWindow::on_pButtonSave_clicked()
{
    qDebug() << "Button Tmr Store clicked" << m_switchNr;

    storeTimer(m_switchNr);
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
    QMqttSubscription *subscription = m_client->subscribe((const QMqttTopicFilter) topic);
    if (!subscription) {
        qDebug() << "Could not subscribe. Is there a valid connection?";
    }
}

void MainWindow::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    int idx, bidx, eidx, fidx, val, dev = 0;
    QByteArray tmr, temp, hum;
    bool pbool;
    QLabel *pLabelT = nullptr;
    QLabel *pLabelH = nullptr;
    QLabel *pLabelOnOff = nullptr;
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

    //qDebug() << content;

    /*
     * Select module
     *
     * "Carport"
     */
    if (topic.name().contains(device[0]) == true) {
        pLabelOnOff = ui->labelOn_1;
        pLabelT = ui->labelTemp_1;
        pProgressT = ui->pBarTemp_1;
        dev = 0;
    }
    /* "Multi" */
    else if (topic.name().contains(device[1]) == true) {
        pLabelOnOff = ui->labelOn_2;
        pLabelT = ui->labelTemp_2;
        pProgressT = ui->pBarTemp_2;
        dev = 1;
    }
    /* "Keller" */
    else if (topic.name().contains(device[2]) == true) {
        pLabelOnOff = ui->labelOn_3;
        pLabelT = ui->labelTemp_3;
        pProgressT = ui->pBarTemp_3;
        dev = 2;
    }
    /* "Stube" */
    else if (topic.name().contains(device[3]) == true) {
        pLabelOnOff = ui->labelOn_4;
        pLabelT = ui->labelTemp_4;
        pProgressT = ui->pBarTemp_4;
        dev = 3;
    }
    /* "Büro TH10" */
    else if (topic.name().contains(device[4]) == true) {
        pLabelOnOff = ui->labelOn_5;
        pLabelT = ui->labelTemp_5;
        pProgressT = ui->pBarTemp_5;
        pLabelH = ui->labelH_5;
        dev = 4;
    }
    /* "USB option" */
    else if (topic.name().contains(device[5]) == true) {
        pLabelOnOff = ui->labelOn_6;
        pLabelT = ui->labelTemp_6;
        pProgressT = ui->pBarTemp_6;
        dev = 5;
    }
    /* Klingel Temp */
    else if (topic.name().contains(tempTopic) == true) {
        pLabelT = ui->labelTemp_7;
        pProgressT = ui->pBarTemp_7;
        dev = 6;
    }
    /* Klingel Taste */
    else if (topic.name().contains(gongTopic) == true) {
        pLabelOnOff = ui->labelOn_7;
        dev = 7;
    }
    /* Klingel Led */
    else if (topic.name().contains(ledTopic) == true) {
        pLabelOnOff = ui->labelOn_7;
        dev = 7;
    }
    /* IR receiver */
    else if (topic.name().contains(irRecTopic) == true) {
        pLabelOnOff = ui->labelOn_ir;
        dev = 8;
    }
    /* IR sender */
    else if (topic.name().contains(irSendTopic) == true) {
        dev = 8;
    }
    //qDebug() << "Device received" << dev;

    /*
     * Klingel
     */
    if (topic.name().contains(tempTopic) == true) {
        val = static_cast<int>(message.toFloat());
        qDebug() << "Klingel temp received" << message << " - " << val;
        temp.append(message);
        m_t[dev].setNum(message.toFloat());

        if (pProgressT)
            pProgressT->setValue(val);
        temp.append(" °C");
        if (pLabelT)
            pLabelT->setText(temp);
    }
    else if (topic.name().contains(gongTopic) == true) {
        qDebug() << "Klingel taste received" << message;
        pbool = false;
        if (message.contains("1")) {
            pbool = true;
            qDebug() << "Gong on received";
        }
        else if (message.contains("0")) {
            qDebug() << "Gong off received";
        }
    }
    else if (topic.name().contains(ledTopic) == true) {
        qDebug() << "Klingel led received" << message;
        pbool = false;
        if (message.contains("0")) {
            pbool = true;
            qDebug() << "Led on received";
        }
        else if (message.contains("1")) {
            qDebug() << "Led off received";
        }
        if (pLabelOnOff) {
            if (pbool)
                 pLabelOnOff->setPixmap(pixmapOn);
            else pLabelOnOff->setPixmap(pixmapOff);
        }
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
        if (pLabelOnOff) {
            if (pbool)
                 pLabelOnOff->setPixmap(pixmapOn);
            else pLabelOnOff->setPixmap(pixmapOff);
        }
    }

    /*
     * Label
     */
    if ((topic.name().contains(sensorTopic) == true)
       || (topic.name().contains(sensor8Topic) == true)) {

        /* temperature */
        idx = message.indexOf("Temperature");
        bidx = message.indexOf(":", idx);
        eidx = message.indexOf(",", bidx);
        fidx = message.indexOf("}", bidx);
        if ((idx < 0) || (bidx < 0))
            return;

        if ((fidx < eidx) && (fidx >= 0)) eidx = fidx;
        temp.append(message.mid(bidx+1, eidx - bidx - 1));  // !!!
        val = static_cast<int>(temp.toFloat());
        m_t[dev] = temp;

        qDebug() << "SENSOR received temp" << temp << " - " << val;

        if (pProgressT)
            pProgressT->setValue(val);
        temp.append(" °C");
        if (pLabelT)
            pLabelT->setText(temp);

        idx = message.indexOf("Humidity");
        bidx = message.indexOf(":", idx);
        eidx = message.indexOf(",", bidx);
        fidx = message.indexOf("}", bidx);
        if ((idx < 0) || (bidx < 0))
            return;

        if ((fidx < eidx) && (fidx >= 0)) eidx = fidx;
        hum.append(message.mid(bidx+1), eidx - bidx - 1);
        val = static_cast<int>(hum.toFloat());

        qDebug() << "SENSOR received hum" << hum << " - " << val;

        if (pProgressH)
            pProgressH->setValue(val);
        hum.append(" %");
        if (pLabelH)
            pLabelH->setText(hum);
    }
    /* IR remote
     * msg : 16712445,0,64
     */
    if (topic.name().contains(irRecTopic) == true) {
        m_irTyp = IR_UNKNOWN;
        if (topic.name().contains("NEC") == true) {
            qDebug() << "IR received NEC" << message;
            m_irTyp = IR_NEC;
        }
        else if (topic.name().contains("RC5") == true) {
            qDebug() << "IR received RC5" << message;
            m_irTyp = IR_RC5;
        }
        if (pLabelOnOff) {
            pLabelOnOff->setPixmap(pixmapIrOn);
            repaint();
            QThread::msleep(500);
            pLabelOnOff->setPixmap(pixmapIrOff);
        }
        idx = message.indexOf(","); // nach value
        if (idx > 0) {
            QByteArray tmp;
            tmp.append(message.mid(0, idx - 1));
            m_irValue = tmp.toLong();
            tmp = message.mid(idx + 1);
            eidx = message.indexOf(",", idx + 1); // nach address
            if (eidx > 0) {
                tmp = message.mid(eidx + 1);
                m_irData = tmp.toInt();
            }
        }
        if (m_irValue > 0) {
            qDebug() << "IR received " << m_irValue << "," << m_irData;
            switch (m_irData) {
            case 69:  toggleButton(0);  break;
            case 71:  toggleButton(1);  break;
            case 64:  toggleButton(2);  break;
            case 7:   toggleButton(3);  break;
            case 21:  toggleButton(4);  break;
            case 9:   toggleButton(5);  break;
            case 25:  toggleButton(6);  break;
            default:
                break;
            }
        }
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

        qDebug() << "STATUS" << dev << "power is" << pow;
        if (pLabelOnOff) {
            if (pbool)
                 pLabelOnOff->setPixmap(pixmapOn);
            else pLabelOnOff->setPixmap(pixmapOff);
        }
    }

    /*
     * State
     */
    if (topic.name().contains(stateTopic) == true) {
        qDebug() << "STATE received" << message;
    }

    /*
     * timers result, m_switchNr, m_timerNr
     *
     * "sonoff/6816/RESULT Message: {\"Timers1\":{
     *                               \"Timer1\":{\"Arm\":1,\"Mode\":2,\"Time\":\"00:00\",\"Window\":15,
     *                               \"Days\":\"0000011\",\"Repeat\":1,\"Output\":1,\"Action\":1},
     *
     *                               \"Timer2\":{\"Arm\":1,\"Mode\":0,\"Time\":\"22:50\",\"Window\":0,
     *                               \"Days\":\"0000011\",\"Repeat\":1,\"Output\":1,\"Action\":0},
     *
     *                               \"Timer3\":{\"Arm\":0,\"Mode\":0,\"Time\":\"00:00\",\"Window\":0,
     *                               \"Days\":\"0000000\",\"Repeat\":0,\"Output\":1,\"Action\":0},
     *
     *                               \"Timer4\":{\"Arm\":0,\"Mode\":0,\"Time\":\"00:00\",\"Window\":0,
     *                               \"Days\":\"0000000\",\"Repeat\":0,\"Output\":1,\"Action\":0}}}\n"
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
                        tmr.append(message.mid(fidx+5, 2));
                        m_timer[m_switchNr][ivar].min = tmr.toInt();
                        qDebug() << "Result TIMER" << ivar << "min" << tmr << m_timer[m_switchNr][ivar].min;
                    }
                    /* Window = 0 */
                    eidx = message.indexOf("Window", fidx);
                    fidx = message.indexOf(":", eidx);
                    if (fidx >= 0) {
                        tmr.clear();
                        tmr.append(message.mid(fidx+1), 1);
                        m_timer[m_switchNr][ivar].window = tmr.toInt();
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
         * sonoff/7236/RESULT: {\"Timers\":\"OFF\"}\n"
         */
        else if (message.indexOf("Timers") >= 0) {
            bidx = message.indexOf("Timers");
            eidx = message.indexOf(":", bidx);
            if ((bidx < 0) || (eidx < 0))
                return;
            tmr.clear();
            tmr.append(message.mid(eidx+2), 2);
            pbool = false;
            if (tmr == "ON") {
                pbool = true;
            }
            qDebug() << "Result received TIMERS" << dev << tmr;

            m_timerGlobal[m_switchNr] = pbool;
            setSwitchState();
        }
        /*
         * "sonoff/7236/RESULT Message: {\"Longitude\":\"12.575721\"}\n"
         *  "sonoff/7236/RESULT Message: {\"Latitude\":\"51.312799\"}\n"
         */
        else if (message.indexOf("Longitude") >= 0) {
            idx = message.indexOf(":");
            if (idx < 0)
                return;
            m_timerLon[m_switchNr] = message.mid(idx+2, 9);
            setSwitchState();
        }
        else if (message.indexOf("Latitude") >= 0) {
            idx = message.indexOf(":");
            if (idx < 0)
                return;
            m_timerLat[m_switchNr] = message.mid(idx+2, 9);
            setSwitchState();
        }
        /*
         * {\"Timezone\":\"+02:00\"}
         * {\"Timezone\":1}
         */
        else if (message.indexOf("Timezone") >= 0) {
            m_timerZone[m_switchNr] = 0; // default
            idx = message.indexOf(":");
            if (idx >= 0) {
                eidx = message.indexOf(":", idx+1);
                if (eidx < 0) {
                    m_timerZone[m_switchNr] = message.mid(idx+1, 1).toInt();
                } else {
                    m_timerZone[m_switchNr] = message.mid(idx+2, 3).toInt();
                }
            }
            qDebug() << "Result received Timezone" << m_timerZone[m_switchNr];
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
    topic.append("/cmnd/Timezone");

    brokerPublish(topic, "");

    topic.clear();
    topic.append("sonoff/");
    topic.append(device[dev]);
    topic.append("/cmnd/TIMERS");

    brokerPublish(topic, "");
}

/* toggle switch on/off */
void MainWindow::toggleButton(int index)
{
    if (index < NB_DEVICES) {
        QString topic;
        topic.append("sonoff/");
        topic.append(device[index]);
        topic.append("/cmnd/POWER");

        brokerPublish(topic, "2");

    } else if (index == NB_DEVICES) {

        brokerPublish(gongTopic, "1");
        brokerPublish(gongTopic, "0");
    }
}

void MainWindow::setLocation(int dev)
{
    QString topic;

    if (m_timerLon[m_switchNr] != MY_LONGITUDE) {
        topic.clear();
        topic.append("sonoff/");
        topic.append(device[dev]);
        topic.append("/cmnd/Longitude");

        brokerPublish(topic, MY_LONGITUDE);
    }

    if (m_timerLat[m_switchNr] != MY_LATITUDE) {
        topic.clear();
        topic.append("sonoff/");
        topic.append(device[dev]);
        topic.append("/cmnd/Latitude");

        brokerPublish(topic, MY_LATITUDE);
    }
}

void  MainWindow::writeLog()
{
    QString content = QDate::currentDate().toString("yyyy.MM.dd")
                + ";"
                + QTime::currentTime().toString()
                + ";";

    for (int i=0; i<NB_DEVICES + NB_ESP8266; i++) {
        content += m_t[i] + ";";
    }
    content += "\n";

    ui->labelStatus->setText(content);

    if (isLog) {
        m_out << content;

        m_out.flush();
        if (! m_logFile.flush()) {
           qDebug() << "Log: Err flush!";
        }

        qDebug() << "Log: " << content << "size " << m_logFile.size();
    }

    /*
     * chart values
     */
    double f;
    nowTime = QDateTime::currentDateTime();
    axisX->setMin(startTime);
    if (startTime.addMSecs(TIMER_CYCLE) < nowTime)
        axisX->setMin(startTime.addMSecs(TIMER_CYCLE));
    axisX->setMax(nowTime);

    for (int i=0; i<NB_DEVICES + NB_ESP8266; i++) {
        f = m_t[i].toDouble();

        series[i]->append(nowTime.toMSecsSinceEpoch(), f);

        if (f > m_maxChart) m_maxChart = f;
        if (f < m_minChart) m_minChart = f;
    }
    if (m_minChart < m_maxChart) {
        axisY->setRange(m_minChart-2.0, m_maxChart+2.0);
    }
    //qDebug() << "Chart: draw range" << m_minChart << m_maxChart;
}

void MainWindow::on_pButtonExit_clicked()
{
    QApplication::quit();
}

void MainWindow::on_checkBoxMo_stateChanged(int arg1)
{
    qDebug() << "Date Mo" << arg1;
    if (arg1)
          m_timer[m_switchNr][m_timerNr].days |= 0x20; // Mo
    else  m_timer[m_switchNr][m_timerNr].days &= ~0x20;
    displayTimer(m_timerNr);
}

void MainWindow::on_checkBoxDi_stateChanged(int arg1)
{
    qDebug() << "Date Di" << arg1;
    if (arg1)
          m_timer[m_switchNr][m_timerNr].days |= 0x10;
    else  m_timer[m_switchNr][m_timerNr].days &= ~0x10;
    displayTimer(m_timerNr);
}

void MainWindow::on_checkBoxMi_stateChanged(int arg1)
{
    qDebug() << "Date Mi" << arg1;
    if (arg1)
          m_timer[m_switchNr][m_timerNr].days |= 0x08;
    else  m_timer[m_switchNr][m_timerNr].days &= ~0x08;
    displayTimer(m_timerNr);
}

void MainWindow::on_checkBoxDo_stateChanged(int arg1)
{
    qDebug() << "Date Do" << arg1;
    if (arg1)
          m_timer[m_switchNr][m_timerNr].days |= 0x04;
    else  m_timer[m_switchNr][m_timerNr].days &= ~0x04;
    displayTimer(m_timerNr);
}

void MainWindow::on_checkBoxFr_stateChanged(int arg1)
{
    qDebug() << "Date Fr" << arg1;
    if (arg1)
          m_timer[m_switchNr][m_timerNr].days |= 0x02;
    else  m_timer[m_switchNr][m_timerNr].days &= ~0x02;
    displayTimer(m_timerNr);
}

void MainWindow::on_checkBoxSa_stateChanged(int arg1)
{
    qDebug() << "Date Sa" << arg1;
    if (arg1)
          m_timer[m_switchNr][m_timerNr].days |= 0x01;
    else  m_timer[m_switchNr][m_timerNr].days &= ~0x01;
    displayTimer(m_timerNr);
}

void MainWindow::on_checkBoxSo_stateChanged(int arg1)
{
    qDebug() << "Date So" << arg1;
    if (arg1)
          m_timer[m_switchNr][m_timerNr].days |= 0x40;
    else  m_timer[m_switchNr][m_timerNr].days &= ~0x40;
    displayTimer(m_timerNr);
}

void MainWindow::on_timeEdit_userTimeChanged(const QTime &time)
{
    qDebug() << "Time changed" << time;
    m_timer[m_switchNr][m_timerNr].hour = ui->timeEdit->time().hour();
    m_timer[m_switchNr][m_timerNr].min = ui->timeEdit->time().minute();
}
