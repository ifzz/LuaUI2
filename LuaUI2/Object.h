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

class LuaObject;

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

    virtual LuaObject *GetLuaSide() = 0;

private:
	volatile long m_refCount;
};

#ifndef UNREF
#define UNREF(x)\
    if(x) {\
        x->Unref();\
        x = NULL;}
#endif

// https://stackoverflow.com/questions/3060006/is-it-worth-setting-pointers-to-null-in-a-destructor
#ifndef INVALID_POINTER
#define INVALID_POINTER(type) reinterpret_cast<type *>(0xDEADBEEF)
#endif

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

} // namespace cs
