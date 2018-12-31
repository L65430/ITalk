#include "information.h"
#include<QDebug>
Information* Information::in_p=0;
Information* Information::getInstance(){
    if(!Information::in_p){
        in_p=new Information;
    }else
        return in_p;
}

