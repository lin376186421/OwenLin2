/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_LOCK_H__
#define __ZEPHYR_LOCK_H__

#include "ZephyrBaseDef.h"
#if (CC_PLATFORM_WP8 != CC_TARGET_PLATFORM)
#include <pthread.h>
#endif

NS_CC_ZEPHYR_BEGIN

#if (CC_PLATFORM_WP8 != CC_TARGET_PLATFORM)
class LockImp{
public:
	LockImp() { 
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	}
	virtual ~LockImp() { pthread_mutex_destroy(&m_mutex); }
public:
	void Lock() { pthread_mutex_lock(&m_mutex); }

	void Unlock() { pthread_mutex_unlock(&m_mutex); }
protected:
	pthread_mutex_t		m_mutex;
};
#else
class LockImp{
public:
	LockImp() {
		
	}
	virtual ~LockImp() { }
public:
	void Lock() {  }

	void Unlock() { }
protected:
};
#endif

template<class T>
class AutoLock{
public:
//		typedef T*	TYPE_PTR;
public:
	// 构造函数自动锁定
	AutoLock(T& lock) : m_lock(lock) {
		m_lock.Lock();
	}

	// 析构函数自动解锁
	~AutoLock() {
		m_lock.Unlock();
	}
private:
	//! 赋值操作符，不做任何处理
	void operator = (const AutoLock<T>& ) {}
private:
	T&		m_lock;
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_LOCK_H__
