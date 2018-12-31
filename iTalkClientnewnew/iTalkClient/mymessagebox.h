#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QWidget>
#include "mylistwidget.h"

namespace Ui {
class Mymessagebox;
}

class Mymessagebox : public QWidget
{
    Q_OBJECT

public:
    explicit Mymessagebox(Mylistwidget* mylistwidget,QWidget *parent = 0);
    ~Mymessagebox();
     bool eventFilter(QObject *object, QEvent *e);
      void setname(QString name);
private:
    Ui::Mymessagebox *ui;
    Mylistwidget* mylistwidget;
};

#endif // MYMESSAGEBOX_H
