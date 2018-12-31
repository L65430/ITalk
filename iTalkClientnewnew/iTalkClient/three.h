#ifndef THREE_H
#define THREE_H

#include<QWidget>
#include<QListWidgetItem>
namespace Ui{
class Three;
}

class Three :public QWidget{
    Q_OBJECT
public:
    explicit Three(QWidget *parent = 0);
    ~Three();

private:
    Ui::Three *ui;
};

#endif // THREE_H
