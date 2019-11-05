/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_THREAD_H__
#define __ZEPHYRP_THREAD_H__

#include "ZephyrBaseDef.h"

#include <pthread.h>

NS_CC_ZEPHYR_BEGIN

//! 以下划线开头的接口，均属于线程内部接口，即仅能在线程循环内部调用

class Thread{
	friend void* _threadfunc(void* param);
public:
	enum{
		_STATE_NONE			=	0,
		_STATE_WAITING		=	1,		//! 等待运行状态
		_STATE_RUNNING		=	2,		//! 运行
		_STATE_CANEXIT		=	3,		//! 可以结束
		_STATE_END			=	4		//! 运行结束
	};
public:
	//! 构造函数
	Thread(ThreadFunc pFunc, void* param);
	//! 析构函数
	virtual ~Thread();
public:
	//! 启动线程
	int start();
public:
	//! 是否在运行中
	bool isRunning() const { return m_state == _STATE_RUNNING; }

	//! 是否在运行结束
	bool isEnd() const { return m_state == _STATE_END; }

	//! 唤醒线程
	void resume() {
		wakeUp();
	}

	//! Wake Up Thread
	void wakeUp();

	//! 发送关闭线程消息，即时返回
	void exitThread();
	//! 关闭线程，阻塞操作
	void waitForExitThread();

	//! 挂起线程，只到线程结束、进程结束，或者被WakeUp
	void _suspend() {
		_sleep(-1);
	}
	//! sleep，可以唤醒；Sleep(INFINITE)相当于挂起线程
	//! 如果返回为false，表示线程需要结束
	bool _sleep(int time);
public:
	//! Set Data
	void setData(void* pData);
	//! Get Data
	void* getData();
protected:

	//! 线程主函数
	virtual int run();
public:
	//! 是否已经结束，在线程循环中检查
	bool _isEnd();
protected:
	//! 设置线程函数
	void setFunc(ThreadFunc pFunc, void* pParam);
private:
	//! 线程结束后释放资源
	void _cleanUp();
protected:
	ThreadFunc			m_func;

	void*				m_param;

	void*				m_pDat;				//! 附属线程数据－用户可设置&获取

	int					m_state;

	pthread_t			m_thread;
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYRP_THREAD_H__
