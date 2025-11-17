//快速使用一下pthread库
#include<iostream>
#include<thread>
#include<vector>
#include<pthread.h>
#include<unistd.h>
#include<string.h>

#define NUM 3

// int g_val = 0;
// void show(const std::string& name)
// {
//     std::cout<<name<<std::endl;
// }

// void* threadRoutine(void* arg)
// {
//     char* name = (char*)arg;
//     int cnt=5;
//     while(cnt)
//     {
//         printf("%s pid: %d\n",name,getpid());
//         sleep(1);
//         cnt--;
//     }
//     pthread_exit((void*)11); //2. 用来终止线程
//     //exit(11); //终止整个进程（非仅当前线程），会执行 atexit 和 stdio 清理，但不应在线程中调用
//     return (void*)1; //1. 终止线程的一种方式
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid,nullptr,threadRoutine,(void*)"Thread 1");
//     sleep(1);
//     pthread_cancel(tid);    //3. 线程取消，返回值是-1 == PTHREAD_CANCELED

//     void* retval;   //八个字节 需要用long long来收取
//     pthread_join(tid,&retval); //不用考虑异常问题，新线程异常，整个进程都会挂，所以做不到监视异常
//     printf("return val : %lld\n",(long long )retval);

//     return 0;
// }

// struct request
// {
//     int num1_;
//     int num2_;
//     char oper_;

//     request(int num1,int num2,char oper)
//     :num1_(num1),num2_(num2),oper_(oper)
//     {}
// };

// struct response
// {   
//     int result_;
//     std::string retval_="0";

//     response* resRun(request* rep)
//     {
//         switch (rep->oper_)
//         {
//         case '+':
//             result_=rep->num1_+rep->num2_;
//             break;
//         case '-':
//             result_=rep->num1_-rep->num2_;
//             break;
//         case '*':
//             result_=rep->num1_*rep->num2_;
//             break;
//         case '/':
//             if(rep->num2_==0) 
//             {
//                 retval_="除零错误";
//                 return this;
//             }
//             result_=rep->num1_/rep->num2_;
//             break;
//         default:
//             break;
//         }

//         return this;
//     }
// };

// void* threadRoutine(void* arg)
// {
//     //强转接受
//     request* rep = static_cast<request*>(arg);
//     //逻辑处理
//     response* rsp = new response();
//     rsp->resRun(rep);
//     //新线程退出
//     pthread_exit(rsp);
// }
// int main()
// {
//     pthread_t tid;
//     request* rep = new request(1,0,'/');
//     pthread_create(&tid,nullptr,threadRoutine,rep);
//     //数据等待，接受数据
//     void* ret;
//     pthread_join(tid,&ret);
//     response* rsp=static_cast<response*>(ret);
//     std::cout<<rsp->result_<<" "<<rsp->retval_<<std::endl;
//     return 0;
// }

// void threadrun()
// {
//     while(1)
//     {
//         std::cout<<"这是C++11的线程库"<<std::endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     std::thread t1(threadrun);
//     t1.join();
//     return 0;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid,nullptr,threadRoutine,nullptr);
//     pthread_exit();
//     pthread_self();
//     pthread_join(tid,nullptr);

//     thread t1(threadRun);
//     t1.join()


//     return 0;
// }
std::string toHex(pthread_t tid)
{
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%lx", tid);
    return buffer;
}

void* threadRoutine(void* args)
{
    int test_i=0;
    for(test_i=0;test_i<10;test_i++)
    {
        std::cout<<toHex(pthread_self())<<", test_i="<<test_i<<std::endl;
        sleep(1);
    }
    pthread_exit((void*)0);
}

int main()
{
    std::vector<pthread_t> tids;
    for(int i=0;i<NUM;i++)
    {
        pthread_t tid;
        tids.push_back(tid);
        pthread_create(&tids[i],nullptr,threadRoutine,nullptr);
    }
    for(int i=0;i<NUM;i++)
    {
        pthread_join(tids[i],nullptr);
    }

    return 0;
}