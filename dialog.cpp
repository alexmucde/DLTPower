#include <QSerialPortInfo>

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    on_pushButtonUpdate_clicked();

    tcpSocket = 0;

    setWindowTitle("DLTRelais");

    ui->pushButtonCloseDLT->setDisabled(true);
    ui->pushButtonDisconnect->setDisabled(true);

    value = 0;
    lastValue = 0;
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_pushButtonUpdate_clicked()
{
    QList<QSerialPortInfo> 	availablePorts  = QSerialPortInfo::availablePorts();

    ui->comboBoxSerialPort->clear();

    for(int num = 0; num<availablePorts.length();num++)
    {
        ui->comboBoxSerialPort->addItem(availablePorts[num].portName());
    }
}

void Dialog::on_pushButtonConnect_clicked()
{
    if(serialPort.isOpen())
        return;

    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setPortName(ui->comboBoxSerialPort->lineEdit()->text());

    if(serialPort.open(QIODevice::ReadWrite)==true)
    {
        ui->pushButtonConnect->setDisabled(true);
        ui->pushButtonDisconnect->setDisabled(false);
        ui->lineEditStatus->setText("Connected");
        connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    }
    else
    {
        ui->lineEditStatus->setText("Connection failed");
    }

}

void Dialog::on_pushButtonDisconnect_clicked()
{
    if(!serialPort.isOpen())
        return;

    serialPort.close();
    disconnect(&serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    ui->pushButtonConnect->setDisabled(false);
    ui->pushButtonDisconnect->setDisabled(true);
    ui->lineEditStatus->setText("");

}

void Dialog::readData()
{
    QByteArray data = serialPort.readAll();

    for(int num=0;num<data.length();num++)
    {
        switch(data[num]&0xf0)
        {
            case 0x10:
                rawData.clear();
                rawData+=data[num];
                break;
            case 0xe0:
                data.clear();
                rawData+=data[num];
                //ui->textEditRawData->setText(rawData.toHex());
                calculateValue();
                //ui->lineEditValue->setText(QString::number(value));
                //ui->lineEditUnit->setText(unit);
                if(tcpSocket && tcpSocket->isOpen() && lastValue!=value)
                {
                    sendValue2(QString::number(value), unit);
                    lastValue = value;
                }
                break;
            default:
                rawData+=data[num];
                break;
        }
    }
}

void Dialog::calculateValue()
{
    if(rawData.length()!=14)
        return;

    value=calculateNumber(rawData[1]&0x7,rawData[2]&0xf);
    value=value*10+calculateNumber(rawData[3]&0x7,rawData[4]&0xf);
    value=value*10+calculateNumber(rawData[5]&0x7,rawData[6]&0xf);
    value=value*10+calculateNumber(rawData[7]&0x7,rawData[8]&0xf);

    if(rawData[1]&0x08) value*=-1;

    if(rawData[3]&0x08) value/=1000;
    if(rawData[5]&0x08) value/=100;
    if(rawData[7]&0x08) value/=10;

    if(rawData[10]&0x08) value/=1000; // milli
    if(rawData[9]&0x08) value/=1000; // mycro

    if(rawData[12]&0x08)
        unit = "A";
    else if(rawData[12]&0x04)
        unit = "V";
    else
        unit = "unknown";
}

int Dialog::calculateNumber(unsigned char a,unsigned char b)
{
    if(a==0x7 && b==0x0d)
        return 0;
    else if(a==0x0 && b==0x05)
        return 1;
    else if(a==0x5 && b==0x0b)
        return 2;
    else if(a==0x1 && b==0x0f)
        return 3;
    else if(a==0x2 && b==0x07)
        return 4;
    else if(a==0x3 && b==0x0e)
        return 5;
    else if(a==0x7 && b==0x0e)
        return 6;
    else if(a==0x1 && b==0x05)
        return 7;
    else if(a==0x7 && b==0x0f)
        return 8;
    else if(a==0x3 && b==0x0f)
        return 9;

}

void Dialog::on_pushButtonOpenDLT_clicked()
{
    if(tcpServer.isListening())
        return;

    tcpServer.setMaxPendingConnections(1);
    tcpServer.listen(QHostAddress::Any,ui->lineEditPortDLT->text().toUInt());
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    ui->pushButtonOpenDLT->setDisabled(true);
    ui->pushButtonCloseDLT->setDisabled(false);
    ui->lineEditStatusDLT->setText("Listening");
}

void Dialog::on_pushButtonCloseDLT_clicked()
{
    if(tcpSocket && tcpSocket->isOpen())
    {
        disconnect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
        disconnect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        tcpSocket->close();
    }

    disconnect(&tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    tcpServer.close();

    ui->pushButtonOpenDLT->setDisabled(false);
    ui->pushButtonCloseDLT->setDisabled(true);
    ui->lineEditStatusDLT->setText("");
}

void Dialog::newConnection()
{
    tcpSocket = tcpServer.nextPendingConnection();
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    ui->lineEditStatusDLT->setText("Connected");
    tcpServer.pauseAccepting();
}

void Dialog::connected()
{
    ui->lineEditStatusDLT->setText("Connected2");

}

void Dialog::disconnected()
{
    ui->lineEditStatusDLT->setText("Listening");
    tcpSocket->close();
    disconnect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    disconnect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    //delete tcpSocket;
    tcpSocket = 0;
    tcpServer.resumeAccepting();
}

void Dialog::sendValue(QString text)
{
    if(!tcpSocket->isOpen())
    {
        return;
    }

    QByteArray data;

    // Standard Header (4 Byte)
    data += 0x21; // htyp: Use extended header, version 0x1
    data += (char)0x00; // message counter
    data += (char)0x00; // length high byte
    data += (char)4+10+4+2+text.length(); // length low byte

    // Extended Header (10 Byte)
    data += (char)0x41; // MSIN: Verbose,NW_TRACE, CAN 0x25
    data += (char)0x01; // NOAR
    data += (char)'M'; // APID
    data += (char)'U'; // APID
    data += (char)'L'; // APID
    data += (char)0; // APID
    data += (char)'V'; // CTID
    data += (char)'A'; // CTID
    data += (char)'L'; // CTID
    data += (char)0; // CTID

    // Payload Type Info (4 Byte)
    data += (char)0x00;
    data += (char)0x02; // String
    data += (char)0x00;
    data += (char)0x00;

    // Payload Type Data Length
    data += ((char)text.length()); // length low byte
    data += ((char)0x00); // length high byte

    // Payload Type Data
    data += text.toUtf8();

    tcpSocket->write(data);
}

void Dialog::sendValue2(QString text1,QString text2)
{
    if(!tcpSocket->isOpen())
    {
        return;
    }

    QByteArray data;

    // Standard Header (4 Byte)
    data += 0x21; // htyp: Use extended header, version 0x1
    data += (char)0x00; // message counter
    data += (char)0x00; // length high byte
    data += (char)4+10+4+2+text1.length()+4+2+text2.length(); // length low byte

    // Extended Header (10 Byte)
    data += (char)0x41; // MSIN: Verbose,NW_TRACE, CAN 0x25
    data += (char)0x02; // NOAR
    data += (char)'M'; // APID
    data += (char)'U'; // APID
    data += (char)'L'; // APID
    data += (char)0; // APID
    data += (char)'V'; // CTID
    data += (char)'A'; // CTID
    data += (char)'L'; // CTID
    data += (char)0; // CTID

    // Payload Type Info (4 Byte)
    data += (char)0x00;
    data += (char)0x02; // String
    data += (char)0x00;
    data += (char)0x00;

    // Payload Type Data Length
    data += ((char)text1.length()); // length low byte
    data += ((char)0x00); // length high byte

    // Payload Type Data
    data += text1.toUtf8();

    // Payload Type Info (4 Byte)
    data += (char)0x00;
    data += (char)0x02; // String
    data += (char)0x00;
    data += (char)0x00;

    // Payload Type Data Length
    data += ((char)text2.length()); // length low byte
    data += ((char)0x00); // length high byte

    // Payload Type Data
    data += text2.toUtf8();

    tcpSocket->write(data);

}

void Dialog::on_pushButtonRelais1On_clicked()
{
    serialPort.write("D11\n");
    ui->lineEditRelais1Status->setText("ON");
}

void Dialog::on_pushButtonRelais1Off_clicked()
{
    serialPort.write("D10\n");
    ui->lineEditRelais1Status->setText("OFF");
}

void Dialog::on_pushButtonRelais1Trigger_clicked()
{
    serialPort.write("D1T\n");
    ui->lineEditRelais1Status->setText("Trigger");
}

void Dialog::on_pushButtonRelais2On_clicked()
{
    serialPort.write("D21\n");
    ui->lineEditRelais2Status->setText("ON");
}

void Dialog::on_pushButtonRelais2Off_clicked()
{
    serialPort.write("D20\n");
    ui->lineEditRelais2Status->setText("OFF");
}

void Dialog::on_pushButtonRelais2Trigger_clicked()
{
    serialPort.write("D2T\n");
    ui->lineEditRelais2Status->setText("Trigger");
}

void Dialog::on_pushButtonRelais3On_clicked()
{
    serialPort.write("D61\n");
    ui->lineEditRelais3Status->setText("ON");
}

void Dialog::on_pushButtonRelais3Off_clicked()
{
    serialPort.write("D60\n");
    ui->lineEditRelais3Status->setText("OFF");
}

void Dialog::on_pushButtonRelais3Trigger_clicked()
{
    serialPort.write("D6T\n");
    ui->lineEditRelais3Status->setText("Trigger");
}
