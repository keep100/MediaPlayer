#ifndef RANDOMLIST_H
#define RANDOMLIST_H
#include<QList>
#include<random>
#include <QDebug>


//用于实现随机列表的功能
//列表中存放的是序号,对应着实际数据列表中的序号
//用法：只需要在对数据列表进行增删时同步对它的增删即可保持数据一致
// 通过设置开始序号，上一个，下一个即可实现随机播放
class RandomList
{
public:
    RandomList();
    void ini(int n);                //初始化
    void setOredr(int order);       //设置当前序号，即使得list[curIndex] = order
    void addOrder();                //添加序号
    void moveOrder(int order);      //删除序号，对应的大于order的序号-1，并重置curIndex;
    int next();                     //返回下一个随机序号
    int pre();                      //返回上一个序号
    void reset();                   //将curIndex置为-1,相当于重置记录
    void show(){
        qDebug()<<"curIndex: "<<curIndex<<' '<<"curOrder: "<<list[curIndex];
        QString str;
        for(auto i:list){
            str+=QString::number(i)+" ";
        }
        qDebug()<<str;
    }
private:
    QList<int> list;        //存放顺序的列表
    int curIndex = -1;      //当前的索引[0,curIndex)是已经遍历过的，（curIndex,size()-1]是待遍历的
    std::random_device rd;
};

#endif // RANDOMLIST_H
