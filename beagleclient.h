#ifndef BeagleClient_H
#define BeagleClient_H

#include <QWidget>
#include <QTcpSocket>
#include <QRegExp>

//#include <QWebEngineView>

namespace Ui {
class BeagleClient;
}

class BeagleClient : public QWidget
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent* ev) Q_DECL_OVERRIDE;

public:
    explicit BeagleClient(QWidget *parent = 0);
    ~BeagleClient();
    QTcpSocket *battSocket;
    QTcpSocket *camSocket;
    QRegExp battRegex;
    QRegExp jpegStartRegex;
    QRegExp jpegEndRegex;
    QByteArray jpegArray;
    bool imageStarted;
    bool imageReceived;
    //    QWebView *webView;

private slots:

    void battSocketDataAvailable();
    void camSocketDataAvailable();

    void camConnected();
    void camDisconnected();

    void battConnected();
    void battDisconnected();

    void on_connectWebcamButton_clicked();
    void on_connectBatteryButton_clicked();

private:
    Ui::BeagleClient *ui;
    void resizeImage();
};

#endif // BeagleClient_H
