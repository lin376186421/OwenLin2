/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_ENCRYPTINT_H__
#define __ZEPHYR_ENCRYPTINT_H__

#include "ZephyrBaseDef.h"

NS_CC_ZEPHYR_BEGIN

class EncryptInt
{
	enum{
		_MASK0	=	0x89,
		_MASK1	=	0x6e,
		_MASK2	=	0x9f,
		_MASK3	=	0xf8,
	};
public:
	EncryptInt();
	~EncryptInt();
public:
    
    int operator + (int val) {  // +
		return get() + val;
	}

    int operator + (double val) {  // +
		return get() + val;
	}
    
    int operator - (int val) {  // -
		return get() - val;
	}
    
    int operator - (double val) {  // -
		return get() - val;;
	}
    
    int operator * (int val) {  // *
		return get() * val;
	}
    
    int operator * (double val) {  // *
		return get() * val;
	}
    
    int operator / (int val) {  // /
		return get() / val;
	}
    
    int operator / (double val) {  // /
		return get() / val;
	}

	EncryptInt& operator += (int val) {  // +=
		set(get() + val); 
		return *this;
	}
    
    EncryptInt& operator += (double val) {  // +=
		set(get() + val);
		return *this;
	}
    
    EncryptInt& operator -= (int val) {  // -=
		set(get() - val);
		return *this;
	}
    
    EncryptInt& operator -= (double val) {  // -=
		set(get() - val);
		return *this;
	}
    
    EncryptInt& operator *= (int val) {  // *=
		set(get() * val);
		return *this;
	}
    
    EncryptInt& operator *= (double val) {  // *=
		set(get() * val);
		return *this;
	}

	EncryptInt& operator /= (int val) {  // /=
		set(get() / val);
		return *this;
	}
    
    EncryptInt& operator /= (double val) {  // /=
		set(get() / val);
		return *this;
	}
    
    EncryptInt& operator ++ () {  // ++
		set(get() + 1);
		return *this;
	}
    
    EncryptInt& operator -- () {  // --
		set(get() - 1);
		return *this;
	}
    
    bool operator > (int val) {  // >
		return (get() > val);
	}
    
    bool operator > (double val) {  // >
		return (get() > val);
	}
    
    bool operator < (int val) {  // <
		return (get() < val);
	}
    
    bool operator < (double val) {  // <
		return (get() < val);
	}
    
    bool operator >= (int val) {  // >=
		return (get() >= val);
	}
    
    bool operator >= (double val) {  // >=
		return (get() >= val);
	}
    
    bool operator <= (int val) {  // <=
		return (get() <= val);
	}
    
    bool operator <= (double val) {  // <=
		return (get() <= val);
	}
    
    bool operator == (int val) {  // ==
		return (get() == val);
	}
    
    bool operator != (int val) {  // !=
		return (get() != val);
	}
    
    EncryptInt& operator = (int val) {  // =
		set(val);
		return *this;
	}
    
    operator int(){
		return get();
	}
    
	operator int() const{
		return get();
	}
    
protected:
	int		m_val;
    
    void set(int val);
	int get() const;
};

NS_CC_ZEPHYR_END

#endif  // __ZEPHYR_ENCRYPTINT_H__