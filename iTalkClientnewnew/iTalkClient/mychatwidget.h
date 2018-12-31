#ifndef MYCHATWIDGET_H
#define MYCHATWIDGET_H

#include <QWidget>

namespace Ui {
class Mychatwidget;
}

class Mychatwidget : public QWidget
{
    Q_OBJECT

public:
    explicit Mychatwidget(QWidget *parent = 0);
    ~Mychatwidget();
    void setlook(bool flag);
    void init(QString name,QString sign,QString id);
    QString getid();
    QString getname();
    QString getsign();

private:
    Ui::Mychatwidget *ui;
public slots:
    void changehight();
};

#endif // MYCHATWIDGET_H
