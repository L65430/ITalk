#include "mymessagebox.h"
#include "ui_mymessagebox.h"
#include<QDesktopWidget>
#include<QListWidget>
#include<mylistwidget.h>
Mymessagebox::Mymessagebox(Mylistwidget* mylistwidget,QWidget *parent) :
    QWidget(parent),mylistwidget(mylistwidget),
    ui(new Ui::Mymessagebox)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->label_2->installEventFilter(this);
    QRect rect = QApplication::desktop()->availableGeometry();
    move(rect.width()-width()-10,rect.height()-height()-3);
    QListWidgetItem* item1=new QListWidgetItem();
    item1->setSizeHint(QSize(0,70));
    ui->listWidget->addItem(item1);
    ui->listWidget->setItemWidget(item1,mylistwidget);


}

Mymessagebox::~Mymessagebox()
{
    delete ui;
}

bool Mymessagebox::eventFilter(QObject *object, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress&&object==ui->label_2)
    {
         hide();
    }
}

void Mymessagebox::setname(QString name){
    ui->label->setText(name+"收到的消息");
}
