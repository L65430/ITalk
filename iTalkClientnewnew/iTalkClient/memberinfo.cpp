#include "memberinfo.h"
#include "ui_memberinfo.h"

Memberinfo::Memberinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Memberinfo)
{
    ui->setupUi(this);
}

Memberinfo::~Memberinfo()
{
    delete ui;
}



void Memberinfo::init(QString id,QString name,QString builder){
    if(builder=="builder"){
        ui->label_2->setText("【群主】");
    }else{
        ui->label_2->setText("【成员】");
    }
    ui->label_3->setText(name);
    ui->label_4->setText("ID:"+id);
}


