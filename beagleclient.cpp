#include "beagleclient.h"
#include "ui_BeagleClient.h"

BeagleClient::BeagleClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BeagleClient)
{
    ui->setupUi(this);
    battSocket = new QTcpSocket;
    camSocket = new QTcpSocket;
    gamePad = new QGamepad;

    imageStarted = false;
    imageReceived = false;
    battRegex.setPattern(QString("V(\\d+)\\*I(\\d+)\\*S(\\d+.?\\d*)\\*"));

    setupGamepadMonitor();

    connect(battSocket,SIGNAL(readyRead()),this,SLOT(battSocketDataAvailable()));
    connect(camSocket,SIGNAL(readyRead()),this,SLOT(camSocketDataAvailable()));

    connect(camSocket,SIGNAL(connected()),this,SLOT(camConnected()));
    connect(camSocket,SIGNAL(disconnected()),this,SLOT(camDisconnected()));

    connect(battSocket,SIGNAL(connected()),this,SLOT(battConnected()));
    connect(battSocket,SIGNAL(disconnected()),this,SLOT(battDisconnected()));
}

BeagleClient::~BeagleClient()
{
    delete ui;
}

void BeagleClient::on_connectBatteryButton_clicked()
{
    if(battSocket->state() == QTcpSocket::ConnectedState)
    {
        battSocket->disconnectFromHost();
    }
    else
    {
        battSocket->connectToHost(ui->hostAdressEdit->text(),ui->batteryPortEdit->text().toLong(),QIODevice::ReadWrite);
    }
}

void BeagleClient::battSocketDataAvailable()
{
    QByteArray ba;
    float capacity;
    while(battSocket->bytesAvailable())
    {
        ba.append(battSocket->readAll());
    }
    QString dataIn = QString(ba);
    if(battRegex.indexIn(dataIn)!=-1)
    {
        capacity = battRegex.cap(3).toFloat();
        ui->batteryGauge->setValue(int(capacity));
    }
}

void BeagleClient::camSocketDataAvailable()
{
    QByteArray data = camSocket->readAll();
    // Start capture when 0xff + 0xd8 come in.
    if(imageStarted)
    {
        // Look for 0xFF,0xD9
        jpegArray.append(data);
        qint16 end = jpegArray.indexOf("\xFF\xD9");
        if(end!=-1)
        {
            jpegArray.left(end+2);
            imageReceived=true;
            imageStarted=false;
            QPixmap pmap;
            pmap.loadFromData(jpegArray);

            ui->imageLabel->setPixmap(pmap);
        }
    }
    else
    {
        // Look for 0xFF,0xD8
        qint16 start = data.indexOf("\xFF\xD8");
        if(start!=-1)
        {
            jpegArray.clear();
            jpegArray.append(data);
            jpegArray.remove(0,start);
            imageStarted=true;
        }
    }
}


void BeagleClient::battConnected()
{
    qDebug() << "Battery socket connected";
    ui->connectBatteryButton->setText("Disconnect Battery");
    if(!ui->connectBatteryButton->isChecked())
    {
        ui->connectBatteryButton->setChecked(true);
    }
}

void BeagleClient::battDisconnected()
{
    qDebug() << "Battery socket disconnected";
    ui->connectBatteryButton->setText("Connect Battery");
    if(ui->connectBatteryButton->isChecked())
    {
        ui->connectBatteryButton->setChecked(false);
    }
}

void BeagleClient::camConnected()
{
    qDebug() << "Webcam socket connected";
    ui->connectWebcamButton->setText("Disconnect Webcam");
    if(!ui->connectWebcamButton->isChecked())
    {
        ui->connectWebcamButton->setChecked(true);
    }
}

void BeagleClient::camDisconnected()
{
    qDebug() << "Webcam socket disconnected";
    ui->connectWebcamButton->setText("Connect Webcam");
    if(ui->connectWebcamButton->isChecked())
    {
        ui->connectWebcamButton->setChecked(false);
    }
}

void BeagleClient::on_connectWebcamButton_clicked()
{
    if(camSocket->state() == QTcpSocket::ConnectedState)
    {
        camSocket->disconnectFromHost();
    }
    else
    {
        imageStarted=false;
        imageReceived=false;
        camSocket->connectToHost(ui->hostAdressEdit->text(),ui->webcamPortEdit->text().toLong(),QIODevice::ReadWrite);
        camSocket->write("GET /?action=stream\r\n\r\n");
    }
}

void BeagleClient::setupGamepadMonitor()
{
    connect(QGamepadManager::instance(), &QGamepadManager::connectedGamepadsChanged, this,
            []() {
        auto gamepads = QGamepadManager::instance()->connectedGamepads();
        qDebug() << "Number of gamepads:" << gamepads.size();
        for (auto i : gamepads) {
            QGamepad *gamepad = new QGamepad(i);
            qDebug() << "Gamepad:" << i;
            qDebug() << "  device id:   " << gamepad->deviceId();
            qDebug() << "  name:        " << gamepad->name();
            qDebug() << "  is connected?" << gamepad->isConnected();
        }
    });

//    connect(QGamepadManager::instance(), &QGamepadManager::gamepadConnected, this,
//            [](int deviceId) { qDebug() << "gamepad connected:" << deviceId; });
//    connect(QGamepadManager::instance(), &QGamepadManager::gamepadDisconnected, this,
//            [](int deviceId) { qDebug() << "gamepad disconnected:" << deviceId; });
    connect(QGamepadManager::instance(), SIGNAL(gamepadButtonPressEvent(int,QGamepadManager::GamepadButton,double)), this,
            SLOT(gamepadButtonPressed(int, QGamepadManager::GamepadButton, double)));
//    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
//            [](int deviceId, QGamepadManager::GamepadButton button) { qDebug() << "button release event:" << deviceId << button; });
        connect(QGamepadManager::instance(), SIGNAL(gamepadAxisEvent(int,QGamepadManager::GamepadAxis,double)), this,
                SLOT(gamepadAxisMoved(int,QGamepadManager::GamepadAxis,double)));
//        connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent, this,
//            [](int deviceId, QGamepadManager::GamepadAxis axis, double value) { qDebug() << "axis event:" << deviceId << axis << value; });
//    connect(QGamepadManager::instance(), &QGamepadManager::buttonConfigured, this,
//            [](int deviceId, QGamepadManager::GamepadButton button) { qDebug() << "button configured:" << deviceId << button; });
//    connect(QGamepadManager::instance(), &QGamepadManager::axisConfigured, this,
//            [](int deviceId, QGamepadManager::GamepadAxis axis) { qDebug() << "axis configured:" << deviceId << axis; });
//    connect(QGamepadManager::instance(), &QGamepadManager::configurationCanceled, this,
//            [](int deviceId) { qDebug() << "configuration canceled:" << deviceId; });
}

void BeagleClient::gamepadButtonPressed(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    qDebug() << "Button press event: " << button << value;
}

void BeagleClient::gamepadAxisMoved(int deviceId, QGamepadManager::GamepadAxis axis, double value)
{
    qDebug() << "Axis moved event: " << axis << value;
}
