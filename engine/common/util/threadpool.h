#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

/**
 * 线程池
 * 一般用法
 * TThreadPool<MyClass, ParamType> threadpool(&MyClass, &MyClass::function, Param);
 * threadpool->init(10);
 * threadpool->start();
 */
//类名，还有参数类型
template <typename T, typename VT=T*>
class TThreadPool : public CRunnable
{
    public:
        TThreadPool(T* Container, void (T::*pFunc)(VT), VT Param=NULL):m_pFunc(pFunc),m_Container(Container),m_Param(Param)
        {
            m_pThreads = NULL;
			m_nThreadCount = 0;
        }

        virtual ~TThreadPool()
        {
			for ( int i = 0 ; i < m_nThreadCount ; i ++ )
        		delete m_pThreads[i];
			delete[] m_pThreads;
        };

        bool init(int nThreadCount)
        {
			if ( m_nThreadCount != 0 )
				return false;
			m_nThreadCount = nThreadCount;
			if ( m_nThreadCount <= 0 )
				return false;
			m_pThreads = new CThread*[m_nThreadCount];
			for ( int i = 0 ; i < m_nThreadCount ; i ++ )
			{
				m_pThreads[i] = CThreadFactory::createThread(*this);
				if ( m_pThreads[i] == NULL )
					return false;
			}
			return true;
        }

        void start()
        {
			for ( int i = 0 ; i < m_nThreadCount ; i ++ )
				m_pThreads[i]->start();
        };

        void stop()
        {
			for ( int i = 0 ; i < m_nThreadCount ; i ++ )
				m_pThreads[i]->stop();
        };

        void setParam(VT _Param)
        {
            m_Param = _Param;
        };

        VT getParam()
        {
            return m_Param;
        };

        void run()
        {
            (m_Container->*m_pFunc)(m_Param);
        };

private :
        void (T::*m_pFunc)(VT); 
        T* m_Container;
		//线程组
        CThread** m_pThreads;       
		//线程的数量
		int m_nThreadCount;
		//线程启动时使用的对象
        VT m_Param;
};


#endif
