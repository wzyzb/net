// threadTest.cpp : 定义控制台应用程序的入口点。
//

#include "thread/thread.h"
#include <iostream>
#include "lock/baseLock.h"

CBaseLock  g_baseLock;

class CMyRun:public CRunnable
{
public:
    virtual ~CMyRun(){}
    virtual void run()
    {
        while (true)
        {
            std::cout<<"I am thread "<< CThread::getCurrentThreadId()<<endl;
            g_baseLock.lock();
            g_baseLock.wait();
            std::cout<<"I am thread "<< CThread::getCurrentThreadId()<<"after wait"<<endl;
            g_baseLock.unlock();
        }
    }
};

class CSignalAllRun:public CRunnable
{
public:
    virtual ~CSignalAllRun(){}
    virtual void run()
    {
        while(true)
        {
            g_baseLock.lock();
            g_baseLock.notifyAll();
            g_baseLock.unlock();
        }
    }
};

void  createManyThread()
{
    int i = 0;
    while (i<3)
    {
        i++;
        CMyRun*  myRunObj = new CMyRun();
        CThreadFactory::createThread(*myRunObj,true)->start();
    }
}

int main(int argc, char* argv[])
{
    CThreadFactory::initialize();
    g_baseLock.initialize();
    createManyThread();
    std::cout <<CThread::getCurrentThreadId()<<"  "<<CThreadFactory::getThreadCount()<<endl;
    char c;
    cin>>c;
    g_baseLock.lock();
    g_baseLock.notifyAll();
    g_baseLock.unlock();


    cin>>c;
    std::cout <<CThread::getCurrentThreadId()<<"  "<<CThreadFactory::getThreadCount()<<endl;
    CThreadFactory::finalize();
    
    std::cin>>c;
	return 0;
}

