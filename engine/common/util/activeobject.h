#if !defined (_ACTIVEOBJECT_H_)
#define _ACTIVEOBJECT_H_

/**
 * 活动对像
 * 一般用法
 * TActiveObject<MyClass, ParamType> activeObject(&MyClass, &MyClass::function, Param);
 * @param MyClass为某个类的指针
 * @param &MyClass::function为某个类的某个成员方法，即本活动对象线程调用的函数
 * @param Param为类个自定义的参数，可无，缺省为NULL
 * 用法详见测试用例
 */
template <typename Container, typename ValueType=Container*>    //类名，还有参数类型
class TActiveObject : public CRunnable
{
    public:
        TActiveObject(Container* _Container, void (Container::*_Action)(ValueType), ValueType _Param=NULL):m_pfn(_Action),m_Container(_Container),m_Param(_Param)
        {
            m_pThread = NULL;
        }

        virtual ~TActiveObject()
        {
        	delete m_pThread;
        };

        bool init()
        {
            if ( NULL != m_pThread )
                return true;

            m_pThread = CThreadFactory::createThread(*this);
            
            if ( NULL == m_pThread )
                return false;
            else
                return true;
        }

        void start()
        {
            m_pThread->start();
        };

        void stop()
        {
           m_pThread->stop();
        };

        void setParam(ValueType _Param)
        {
            m_Param = _Param;
        };

        ValueType getParam()
        {
            return m_Param;
        };

    private:
        void run()
        {
            (m_Container->*m_pfn)(m_Param);
        };

        void (Container::*m_pfn)(ValueType); 
        Container* m_Container;
        CThread*         m_pThread;       
        ValueType m_Param;  //活动对象的私有参数
};

#endif

