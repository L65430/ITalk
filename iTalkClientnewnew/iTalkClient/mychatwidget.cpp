#include "mychatwidget.h"
#include "ui_mychatwidget.h"

Mychatwidget::Mychatwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Mychatwidget)
{
    ui->setupUi(this);
}

Mychatwidget::~Mychatwidget()
{
    delete ui;
}
void Mychatwidget::changehight()
{
    ui->label->resize(50,50);
}

void Mychatwidget::setlook(bool flag){
    if(flag){
        ui->label_3->setVisible(true);
    }else{
        ui->label_3->setVisible(false);
    }
}


void Mychatwidget::init(QString name,QString sign,QString id){
    ui->label_2->setText(name);
    ui->label_3->setText(sign);
    ui->label_4->setText(id);
    ui->label_4->setVisible(false);
    ui->label_3->setVisible(false);
}

QString Mychatwidget::getid(){
    return ui->label_4->text();
}

QString Mychatwidget::getname(){
    return ui->label_2->text();
}

QString Mychatwidget::getsign(){
    return ui->label_3->text();
}
