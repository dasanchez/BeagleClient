#include "beagleclient.h"
#include "ui_BeagleClient.h"

BeagleClient::BeagleClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BeagleClient)
{
    ui->setupUi(this);
    battSocket = new QTcpSocket;
    camSocket = new QTcpSocket;
    //    webView = new QWebView(this);
    imageStarted = false;
    imageReceived = false;
    battRegex.setPattern(QString("V(\\d+)\\*I(\\d+)\\*S(\\d+.?\\d*)\\*"));

    connect(battSocket,SIGNAL(readyRead()),this,SLOT(battSocketDataAvailable()));
    connect(camSocket,SIGNAL(readyRead()),this,SLOT(camSocketDataAvailable()));

    connect(camSocket,SIGNAL(connected()),this,SLOT(camConnected()));
    connect(camSocket,SIGNAL(disconnected()),this,SLOT(camDisconnected()));

    connect(battSocket,SIGNAL(connected()),this,SLOT(battConnected()));
    connect(battSocket,SIGNAL(disconnected()),this,SLOT(battDisconnected()));

    //    jpegStartRegex.setPattern(\\x);
}

BeagleClient::~BeagleClient()
{
    delete ui;
}

void BeagleClient::on_connectBatteryButton_clicked()
{
//    clientSocket->connectToHost(ui->hostAdressEdit->text(),ui->webcamPortEdit->text().toLong(),QIODevice::ReadOnly);
    if(battSocket->state() == QTcpSocket::ConnectedState)
        //    if(ui->connectWebcamButton->text()=="Disconnect Webcam")
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
        //        ui->dataDisplay->setText(ba);
    }
    QString dataIn = QString(ba);
    //    battRegex.indexIn(dataIn);
    if(battRegex.indexIn(dataIn)!=-1)
    {
//        qDebug() << "Batt regex index:" << battRegex.indexIn(dataIn);
//        qDebug() << battRegex.cap(1);
//        qDebug() << battRegex.cap(2);
//        qDebug() << battRegex.cap(3);
        capacity = battRegex.cap(3).toFloat();
        ui->batteryGauge->setValue(int(capacity));
    }
}

void BeagleClient::camSocketDataAvailable()
{
    //    QByteArray data = QByteArray::fromHex(camSocket->readAll());
    QByteArray data = camSocket->readAll();
    // Start capture when 0xff + 0xd8 come in.
    if(imageStarted)
    {
        //        qDebug() << "Image started";
        // Look for 0xFF,0xD9
        jpegArray.append(data);
        qint16 end = jpegArray.indexOf("\xFF\xD9");
        if(end!=-1)
        {
            jpegArray.left(end+2);
            imageReceived=true;
            imageStarted=false;
            //            qDebug() << jpegArray.size();
            //            qDebug() << jpegArray;
            QPixmap pmap;
            pmap.loadFromData(jpegArray);

            ui->imageLabel->setPixmap(pmap);

//            pmap = pmap.scaled(ui->imageLabel->size(),Qt::KeepAspectRatio);
//            qDebug() << ui->imageLabel->size();
//            ui->imageLabel->setPixmap(pmap);

            //            ui->imageLabel->setPixmap();
        }

    }
    else
    {
        //        qDebug() << "Image not started";
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
    //        qDebug() << data;
    //    if(imageReceived)
    //        camSocket->disconnectFromHost();
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
        //    if(ui->connectWebcamButton->text()=="Disconnect Webcam")
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

//void BeagleClient::resizeEvent(QResizeEvent *ev)
//{
//    if(imageReceived)
//    {
//        resizeImage();
//    }
//    QWidget::resizeEvent(ev);
//}

//void BeagleClient::resizeImage()
//{
//    if(imageReceived)
//    {
//        const QPixmap *p = ui->imageLabel->pixmap();
//        int w = ui->imageLabel->width();
//        int h = ui->imageLabel->height();
//        ui->imageLabel->setPixmap(p->scaled(w,h,Qt::KeepAspectRatio));
//    }
//}
