#ifndef BeagleClient_H
#define BeagleClient_H

#include <QWidget>
#include <QTcpSocket>
#include <QRegExp>
#include <aspectratiopixmaplabel.h>
#include <QGamepad>

namespace Ui {
class BeagleClient;
}

class BeagleClient : public QWidget
{
    Q_OBJECT

protected:

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
    QGamepad *gamePad;
    bool imageStarted;
    bool imageReceived;
    void setupGamepadMonitor();

private slots:

    void battSocketDataAvailable();
    void camSocketDataAvailable();

    void camConnected();
    void camDisconnected();

    void battConnected();
    void battDisconnected();

    void on_connectWebcamButton_clicked();
    void on_connectBatteryButton_clicked();
    void gamepadButtonPressed(int deviceId, QGamepadManager::GamepadButton button, double value);
    void gamepadAxisMoved(int deviceId, QGamepadManager::GamepadAxis axis, double value);

private:
    Ui::BeagleClient *ui;
};

#endif // BeagleClient_H
