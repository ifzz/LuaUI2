/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once

#include "stdafx.h"
#include <time.h>
#include <vector>

// RTTI
// RefCount  
// PostMessage Wraper
// Delegate (static member function) 

#include <string>


class RTTI
{
public:
    virtual const size_t TypeIdInstance() const = 0;
    
    virtual RTTI* QueryInterface(const size_t)
    {
        return NULL;
    }
    virtual const RTTI* QueryInterface(const size_t) const
    {
        return NULL;
    }

    virtual bool Is(const size_t id) const
    {
        return false;
    }

    virtual bool Is(const std::string& name) const
    {
        return false;
    }

    template <typename T>
    T* As() 
    {
        if (Is(T::TypeIdClass()))
        {
            return reinterpret_cast<T*>(this);
        }

        return NULL;
    }
    template <typename T>
    const T* As() const
    {
        if (Is(T::TypeIdClass()))
        {
            return reinterpret_cast<T*>(this);
        }

        return NULL;
    }
};

#define RTTI_DECLARATIONS(Type, ParentType)                            \
    public:                                                            \
        static std::string TypeName() { return std::string(#Type); }   \
        virtual const size_t TypeIdInstance() const                    \
        { return Type::TypeIdClass(); }                                \
        static const size_t TypeIdClass()                              \
        { static int d = 0; return (size_t) &d; }                      \
        virtual RTTI* QueryInterface( const size_t id )       \
        {                                                              \
            if (id == TypeIdClass())                                   \
                { return static_cast<RTTI*>(const_cast<Type*>(this)); }                                \
            else                                                       \
                { return ParentType::QueryInterface(id); }             \
        }                                                              \
        virtual const RTTI* QueryInterface( const size_t id ) const \
        {                                                              \
            if (id == TypeIdClass())                                   \
                { return static_cast<RTTI*>(const_cast<Type*>(this)); }                                \
            else                                                       \
                { return ParentType::QueryInterface(id); }             \
        }                                                              \
        virtual bool Is(const size_t id) const                         \
        {                                                              \
            if (id == TypeIdClass())                                  \
                { return true; }                                       \
            else                                                       \
                { return ParentType::Is(id); }                         \
        }                                                              \
        virtual bool Is(const std::string& name) const                 \
        {                                                              \
            if (name == TypeName())                                    \
                { return true; }                                       \
            else                                                       \
                { return ParentType::Is(name); }                       \
        }                                                              


class KObject;

typedef void (*KEventHandler)(KObject *reciver, KObject *sender, KObject *args);

template <class T>
class RefPtr
{
public:
	RefPtr()
	{
		m_ptr = NULL;
	}
	explicit RefPtr(T* ptr)
	{
		m_ptr = ptr;
		m_ptr->Ref();
	}
	RefPtr(const RefPtr& rhs)
	{
		m_ptr = rhs.m_ptr;
		m_ptr->Ref();
	}
	~RefPtr()
	{
		DoUnref();
	}
	void Allocate()
	{
		if (m_ptr)
		{
			DoUnref();
		}
		m_ptr = new T;
		m_ptr->Ref();
	}
	RefPtr& operator=(const RefPtr& rhs)
	{
		if (m_ptr)
		{
			DoUnref();
		}
		m_ptr = rhs.m_ptr;
		m_ptr->Ref();
		return *this;
	}
	T * operator->()
	{
		assert(m_ptr != NULL);
		return m_ptr;
	}
	T & operator*()
	{
		assert(m_ptr != NULL);
		return (*m_ptr);
	}
private:
	void DoUnref()
	{
		assert(m_ptr->RefCount() >= 0);
		m_ptr->Unref();
		if (m_ptr->RefCount() == 0)
		{
			delete m_ptr;
			m_ptr = NULL;
		}
	}
private:
	T *m_ptr;
};

template<class T>
class SimpleFactory
{
protected:
	SimpleFactory() {};
	~SimpleFactory() {};
public:
	static void CreateInstance(T **ppObj)
	{
		*ppObj = new T;
	}
};

class KObject : public RTTI
{
protected:
	KObject() 
	{
		m_refCount = 1;
		m_magic = m_sMagic;
	}
	virtual ~KObject() 
	{
		m_magic = 0; //感觉这样还能检查已销毁的对象
	}

public:
	void Ref()
	{
		m_refCount++;
	}
	void Unref()
	{
		m_refCount--;
		assert(m_refCount >= 0);
		if (m_refCount == 0)
		{
			delete this;
		}
	}
	int RefCount()
	{
		return m_refCount;
	}
	static void InitMagic()
	{
		if (m_sMagic == 0)
		{
			m_sMagic = _time32(NULL);
		}
	}
	// 山寨运行时类型识别
	bool IsValid()
	{
		return m_magic == m_sMagic;
	}

	RTTI_DECLARATIONS(KObject, RTTI);

private:
	KObject(const KObject&);
	void operator=(const KObject&);
	int m_magic;
	int m_refCount;
	static int m_sMagic;
};

__declspec(selectany) int KObject::m_sMagic = 0;

#define DECLARE_CREATE_INSTANCE(t) static t *CreateInstance() { return new t; }


class KDelegateItem
{
public:
	KEventHandler handler;
	KObject *reciever;
};

class KDelegate
{
public:
	KDelegate(){}
	~KDelegate(){}

	void AttachListener(KObject *reciever, KEventHandler handler)
	{
		KDelegateItem item;
		item.reciever = reciever;
		item.handler = handler;
		m_vecListern.push_back(item);
	}
	void FireEvent(KObject *sender, KObject *args)
	{
		for(size_t i = 0; i < m_vecListern.size(); i ++)
		{
			KEventHandler func = m_vecListern[i].handler;
			func(m_vecListern[i].reciever, sender, args);
		}
	}
private:
	std::vector<KDelegateItem> m_vecListern;
};

/*
class TestClass1 : public KObject
{
public:
	TestClass1()
	{
		m_name = NULL;
	}
	~TestClass1()
	{
	}
	void SetName(const char *name)
	{
		m_name = name;
	}
private:
	const char *m_name;
};

void fun1(RefPtr<TestClass1> p1)
{
	p1->RefCount();
}

int _tmain(int argc, _TCHAR* argv[])
{
	//RefPtr<TestClass1> obj = new TestClass1("lala");
	//fun1(obj);
	RefPtr<TestClass1> ojb2;
	ojb2.Allocate();
	ojb2->SetName("hi there");
	fun1(ojb2);
	size_t i = sizeof(ojb2);
	return 0;
}
*/
