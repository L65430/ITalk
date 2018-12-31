#ifndef MEMBERINFO_H
#define MEMBERINFO_H

#include <QWidget>

namespace Ui {
class Memberinfo;
}

class Memberinfo : public QWidget
{
    Q_OBJECT

public:
    explicit Memberinfo(QWidget *parent = 0);
    ~Memberinfo();
    void init(QString id,QString name,QString builder);

private:
    Ui::Memberinfo *ui;
};

#endif // MEMBERINFO_H
