#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButtonUpdate_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonDisconnect_clicked();

    void readData();

    void on_pushButtonOpenDLT_clicked();

    void on_pushButtonCloseDLT_clicked();

    void newConnection();
    void connected();
    void disconnected();

    void on_pushButtonRelais1On_clicked();

    void on_pushButtonRelais1Off_clicked();

    void on_pushButtonRelais1Trigger_clicked();

    void on_pushButtonRelais2On_clicked();

    void on_pushButtonRelais2Off_clicked();

    void on_pushButtonRelais2Trigger_clicked();

    void on_pushButtonRelais3On_clicked();

    void on_pushButtonRelais3Off_clicked();

    void on_pushButtonRelais3Trigger_clicked();

private:
    Ui::Dialog *ui;

    QSerialPort serialPort;
    QByteArray rawData;
    float value,lastValue;
    QString unit;
    QTcpServer tcpServer;
    QTcpSocket *tcpSocket;

    void calculateValue();
    int calculateNumber(unsigned char a,unsigned char b);
    void sendValue(QString text);
    void sendValue2(QString text1,QString text2);
};
#endif // DIALOG_H
