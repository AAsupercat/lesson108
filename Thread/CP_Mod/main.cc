#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "BlockingQueue.hpp"

using namespace std;


//定义全局的锁和线程

int mydata =1;
void* Consumer(void* args)
{
    while(true)
    {
        BlockingQueue<int>* bt = static_cast<BlockingQueue<int>* >(args);
        int tmp=bt->pop();
        cout<<"Consumer get a task: "<<tmp<<endl;
    }
    
    return nullptr;
}

void* Productor(void* args)
{
    while(true)
    {
        BlockingQueue<int>* bt = static_cast<BlockingQueue<int>* >(args);
        bt->push(mydata);   
        cout<<"Productor push a task: "<<mydata<<endl;
        mydata++;
        sleep(2);
    }

    return nullptr;
}

int main()
{
  //创建消费者和生产者线程
    pthread_t ctid; 
    pthread_t ptid;

    BlockingQueue<int>* bt = new BlockingQueue<int>(); //创建阻塞队列
    pthread_create(&ctid,nullptr,Consumer,bt);
    pthread_create(&ptid,nullptr,Productor,bt);

    pthread_join(ctid,nullptr);
    pthread_join(ptid,nullptr);
    delete bt;
    return 0;
}