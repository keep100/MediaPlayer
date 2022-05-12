#ifndef PLAYMODE_H
#define PLAYMODE_H

#include<QObject>

class PlayMode:public QObject
{
    Q_OBJECT

public:
    enum mode{
            Random,     //随机
            Order,      //顺序
            Loop        //循环
        };
    Q_ENUM(mode)
};


#endif // PLAYMODE_H
