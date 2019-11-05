/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "Thread.h"
#include "Zephyr.h"

NS_CC_ZEPHYR_BEGIN

/////////////////////////////////////////////////////////////////////////////////////
// Thread

void* _threadfunc(void* param)
{
	Thread* pThread = (Thread*)param;

	pThread->m_state = Thread::_STATE_RUNNING;

	pThread->run();

	pThread->_cleanUp();

	//pThread->m_state = Thread::_STATE_END;

	return NULL;
}

//! 构造函数
Thread::Thread(ThreadFunc pFunc, void* param)
	: m_func(pFunc)
	, m_param(param)
	, m_state(_STATE_NONE)
	, m_pDat(NULL)
{
	memset(&m_thread, 0, sizeof(m_thread));
	//m_thread = NULL;
}

//! 析构函数
Thread::~Thread() 
{
	waitForExitThread();
}

//! 线程主函数
int Thread::run()
{
	if(m_func != NULL)
	{
		m_func(m_param);
	}

	return 0;
}

//! 线程结束后释放资源
void Thread::_cleanUp()
{
	m_state = _STATE_END;

	//pthread_exit(NULL);

	//if(m_hExitEvent != NULL)
	//{
	//	CloseHandle(m_hExitEvent);

	//	m_hExitEvent = NULL;
	//}

	//if(m_hWakeUpEvent != NULL)
	//{
	//	CloseHandle(m_hWakeUpEvent);

	//	m_hWakeUpEvent = NULL;
	//}
	//	
	//GetThreadMgr().KillThread(*this);
}

//! 是否已经结束，在线程循环中检查
bool Thread::_isEnd()
{
//	sigwait();

	return m_state == _STATE_CANEXIT;

	//__ZEPHYR7_ENTER_FUNCTION

	//if(m_hExitEvent == NULL)
	//	return true;

	//HANDLE handle[] = {m_hEventExit, m_hExitEvent};

	//DWORD ret = WaitForMultipleObjects(2, handle, false, 0);

	//return ret != WAIT_TIMEOUT && ret != WAIT_FAILED;

	//__ZEPHYR7_LEAVE_FUNCTION

	//return false;

	//return false;
}

//! 发送关闭线程消息，即时返回
void Thread::exitThread()
{
//	pthread_kill(m_thread, SIGKILL);

	m_state = _STATE_CANEXIT;
	//__ZEPHYR7_ENTER_FUNCTION

	//if(m_hExitEvent != NULL)
	//	SetEvent(m_hExitEvent);

	//__ZEPHYR7_LEAVE_FUNCTION
}

//! 关闭线程，阻塞操作
void Thread::waitForExitThread()
{
	//__ZEPHYR7_ENTER_FUNCTION

	//ExitThread();

	//if(m_hThreadHandle != NULL)
	//{
	//	if(WaitForSingleObject(m_hThreadHandle, INFINITE) != WAIT_OBJECT_0)
	//		__OutputLog(ZSTR("Error"), ZSTR("Thread : 0x%08x WaitForExitThread() WaitForSingleObject Fail!"), this);
	//}

	//__ZEPHYR7_LEAVE_FUNCTION
}

//! sleep，可以唤醒；Sleep(INFINITE)相当于挂起线程
bool Thread::_sleep(int time)
{
//	__ZEPHYR7_ENTER_FUNCTION
//
//	if(m_hExitEvent == NULL)
//		return false;
//
//	HANDLE handle[] = {m_hEventExit, m_hExitEvent, m_hWakeUpEvent};
//
////		ResetEvent(m_hWakeUpEvent);
//
////		__OutputLog(ZPROC_LOG_LEVEL_DEBUG, "Thread : 0x%08x Thread Sleep!", this);
//
//	DWORD ret = WaitForMultipleObjects(3, handle, false, time);
//
//	if(ret == WAIT_TIMEOUT)
//		return true;
//	else if(ret >= WAIT_OBJECT_0 && ret <= WAIT_OBJECT_0 + 2)
//	{
//		if(ret == WAIT_OBJECT_0 + 2)
//			ResetEvent(m_hWakeUpEvent);
//
//		return false;
//	}
//	else
//		return true;
//
//	__ZEPHYR7_LEAVE_FUNCTION
//
//	return false;
	return false;
}

//! Wake Up Thread
void Thread::wakeUp()
{
//	__ZEPHYR7_ENTER_FUNCTION
//
//	if(m_hWakeUpEvent != NULL)
//	{
////			__OutputLog(ZPROC_LOG_LEVEL_DEBUG, ZSTR("Thread : 0x%08x Thread WakeUp!"), this);
//
//		SetEvent(m_hWakeUpEvent);
//	}
//
//	__ZEPHYR7_LEAVE_FUNCTION
}

//! 设置线程函数
void Thread::setFunc(ThreadFunc pFunc, void* pParam)
{
	m_func = pFunc;
	m_param = pParam;
}

//! Set Data
void Thread::setData(void* pData)
{
	m_pDat = pData;
}

//! Get Data
void* Thread::getData()
{
	return m_pDat;
}

//! 启动线程
int Thread::start()
{
	pthread_create(&m_thread, NULL, _threadfunc, this);

	//__ZEPHYR7_ENTER_FUNCTION

	//return GetThreadMgr().CreateThread(*this, m_func, m_param, m_dwStackSize, m_dwCreationFlags, m_iPriority);

	//__ZEPHYR7_LEAVE_FUNCTION

	//return E_FAIL;

	return 0;
}

NS_CC_ZEPHYR_END
