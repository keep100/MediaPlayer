#include "randomlist.h"

RandomList::RandomList()
{

}

void RandomList::ini(int n){
    list.resize(n);
    for(int i=0;i<n;i++)
        list[i] = i;
}

void RandomList::setOredr(int order){
    int pos = list.indexOf(order);
    if(pos==-1){
        return;
    }
    if(curIndex==-1){
        curIndex = 0;
        std::mt19937 rng(rd());
        std::shuffle(list.begin(),list.end(),rng);
        list.swapItemsAt(curIndex,list.indexOf(order));
    }
    else
        curIndex = pos;
}

void RandomList::reset(){
    curIndex = -1;
}

void RandomList::addOrder(){
    int order = list.size();
    list.append(order);
    if(curIndex!=-1)
        list.swapItemsAt(list.size()-1,curIndex+1);

}

void RandomList::moveOrder(int order){
    if(order>list.size()||order<0)
        return;
    int pos = -1;
    for(int i = 0;i<list.size();i++){
        if(order==list[i])
            pos = i;
        list[i] -= order<list[i]?1:0;
    }
    //修改curIndex
    if(pos < curIndex){
        curIndex--;
    }
    if(pos>=0){
        list.removeAt(pos);
    }
}

int RandomList::next(){
    if(curIndex<0||list.size()==0){
        return -1;
    }
    if(curIndex<list.size()-1)
        return list[curIndex+1];
    else{
        return list[0];     //相当于回到了队列头部
    }
}

int RandomList::pre(){
    if(curIndex<0||list.size()==0){
        return -1;
    }
    if(curIndex>0)
        return list[curIndex-1];
    else{
        return list[list.size()-1];     //相当于回到了队列尾部
    }
}
