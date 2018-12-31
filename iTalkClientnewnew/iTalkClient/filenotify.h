#ifndef FILENOTIFY_H
#define FILENOTIFY_H

#include <QWidget>
#include<QTcpSocket>

namespace Ui {
class Filenotify;
}

class Filenotify : public QWidget
{
    Q_OBJECT

public:
    explicit Filenotify(QString filename,QString username,QTcpSocket* tcpSocket,QWidget *parent = 0);
    ~Filenotify();

private:
    Ui::Filenotify *ui;
    QString username;
    QString filename;
    QTcpSocket* tcpSocket;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // FILENOTIFY_H
