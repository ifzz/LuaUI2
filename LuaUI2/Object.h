/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once

#include "stdafx.h"
#include <time.h>
#include <vector>
#include <string>

namespace cs {

class Object;

template <class T>
class RefPtr
{
public:
	RefPtr()
	{
		m_ptr = nullptr;
	}
	explicit RefPtr(T* ptr)
	{
		m_ptr = ptr;
	}
	RefPtr(const RefPtr& rhs)
	{
		m_ptr = rhs.m_ptr;
		m_ptr->Ref();
	}
    void operator=(const RefPtr&& rhs)
    {
        m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
    }
    RefPtr(RefPtr&& rhs)
    {
        m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
    }
	~RefPtr()
	{
        if (m_ptr)
        {
            m_ptr->Unref();
        }
	}
    void Reset(T *ptr)
    {
        if (m_ptr)
        {
            m_ptr->Unref();
        }
        m_ptr = ptr;
        //m_ptr->Ref();
    }
    bool Empty()
    {
        return m_ptr == nullptr;
    }
	RefPtr& operator=(const RefPtr& rhs)
	{
		if (m_ptr)
		{
            m_ptr->Unref();
		}
		m_ptr = rhs.m_ptr;
		m_ptr->Ref();
		return *this;
	}
	T * operator->()
	{
		return m_ptr;
	}
	T & operator*()
	{
        return *m_ptr;
	}

private:
	T *m_ptr;
};

class Object
{
protected:
	Object() 
	{
		m_refCount = 1;
	}
	virtual ~Object() 
	{
	}
    Object(const Object&) = delete;
    void operator=(const Object&) = delete;

public:
	void Ref()
	{
#ifdef CS_OBJECT_ATOMIC
        _InterlockedIncrement(&m_refCount);
#else
        m_refCount ++;
#endif
	}

    void Unref()
    {
#ifdef CS_OBJECT_ATOMIC
        if (_InterlockedDecrement(&m_refCount) == 0)
#else
        if (--m_refCount == 0)
#endif
        {
            delete this;
        }
    }

	int RefCount()
	{
		return m_refCount;
	}

private:
	volatile long m_refCount;
};

class INotify
{
public:
    virtual void OnNotify(UINT idSender, void *sender, UINT idMessage, void *message) = 0;
};

// 好像又不需要这个 可以 void SetNotify(INotify*) override; 呀,
// 注意有override,而且不要和基类用一个INotify *m_notify就可以了.
class NotifyInvoker
{
public:
    void Invoke(UINT idSender, void *sender, UINT idMessage, void *message)
    {
        if (m_notfy)
        {
            m_notfy->OnNotify(idSender, sender, idMessage, message);
        }
    }
private:
    INotify* m_notfy;
};

template<typename T>
RefPtr<T> MakeRefPtr()
{
    return RefPtr<T>(new T());
}

template<typename T, typename A1>
RefPtr<T> MakeRefPtr(const A1& a1)
{
    return RefPtr<T>(new T(a1));
}

template<typename T, typename A1, typename A2>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2)
{
    return RefPtr<T>(new T(a1, a2));
}

template<typename T, typename A1, typename A2, typename A3>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3)
{
    return RefPtr<T>(new T(a1, a2, a3));
}

template<typename T, typename A1, typename A2, typename A3, typename A4>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
{
    return RefPtr<T>(new T(a1, a2, a3, a4));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5, a6));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11));
}

template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
RefPtr<T> MakeRefPtr(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11, const A12& a12)
{
    return RefPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12));
}

} // namespace cs
