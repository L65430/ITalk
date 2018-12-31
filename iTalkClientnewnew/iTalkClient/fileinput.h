#ifndef FILEINPUT_H
#define FILEINPUT_H

#include <QWidget>
#include<QTcpSocket>
#include<QFile>

namespace Ui {
class Fileinput;
}

class Fileinput : public QWidget
{
    Q_OBJECT

public:
    explicit Fileinput(QString friendid,QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Fileinput();
    bool eventFilter(QObject *object, QEvent *e);

private:
    Ui::Fileinput *ui;
    QTcpSocket* tcpSocket;
    QFile *localFile;  //要发送的文件
    qint64 totalBytes;  //数据总大小
    qint64 bytesWritten;  //已经发送数据大小
    qint64 bytesToWrite;   //剩余数据大小
    qint64 loadSize;   //每次发送数据的大小
    QString fileName;  //保存文件路径
    QByteArray outBlock;  //数据缓冲区，即存放每次要发送的数据
    QString friendid;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
//    void updateClientProgress(qint64);
//    void startTransfer();
//    void displayError(QAbstractSocket::SocketError);
//    void openFile();

};

#endif // FILEINPUT_H
