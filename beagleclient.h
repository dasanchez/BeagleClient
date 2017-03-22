#ifndef BeagleClient_H
#define BeagleClient_H

#define SDL_MAIN_HANDLED
#include <QWidget>
#include <QTcpSocket>
#include <QRegExp>
#include <aspectratiopixmaplabel.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
//#include <SDL2/SDL_joystick.h>

//#include <QWebEngineView>

namespace Ui {
class BeagleClient;
}

class BeagleClient : public QWidget
{
    Q_OBJECT

protected:
//    void resizeEvent(QResizeEvent* ev) Q_DECL_OVERRIDE;

public:
    explicit BeagleClient(QWidget *parent = 0);
    ~BeagleClient();
    QTcpSocket *battSocket;
    QTcpSocket *camSocket;
    QRegExp battRegex;
    QRegExp jpegStartRegex;
    QRegExp jpegEndRegex;
    QByteArray jpegArray;
    AspectRatioPixmapLabel arpLabel;
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
//    void resizeImage();
};

#endif // BeagleClient_H
