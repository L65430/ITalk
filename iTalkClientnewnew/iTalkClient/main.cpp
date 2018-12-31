#include "widget.h"
#include <QApplication>
#include<form.h>
#include "talk.h"
#include "drawer.h"
#include "one.h"
#include "chat.h"
#include "wechat.h"
#include "search.h"
#include "emotion.h"
#include "mymessagebox.h"
#include<QTextCodec>
#include "sunshine.h"
#include "friendmsg.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Form f;
    f.show();
     QTextCodec* codec=QTextCodec::codecForName("UTF-8");
     QTextCodec::setCodecForLocale(codec);

    return a.exec();
}

