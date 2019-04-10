#pragma once
#include<cstdlib>  //为了malloc、free
#include<iostream>

#define __THROW_BAD_ALLOC std::cerr<<"out of memory"<< std::endl; exit(1)

class OneSpce
{
private:
	//以下是函数指针，所代表的函数将用来处理内存不足的情况
	//oom：out of memory
	static void *Oom_Malloc(size_t);
	static void *Oom_Realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();
public:
	// 对malloc的封装
	static void * Allocate(size_t n)
	{
		// 申请空间成功，直接返回，失败交由oom_malloc处理
		void *result = malloc(n);  //一级空间配置器直接使用 malloc()
		//无法满足需求时，改用 oom_malloc() 
		if (0 == result)
			result = Oom_Malloc(n);
		return result;
	}
	// 对free的封装
	static void Deallocate(void *p, size_t /* n */)  //这个size_t完全可以不要，但是为了统一接口，加上了这个size_t
	{
		free(p);  //一级空间配置器直接使用 free()
	}

	// 对realloc的封装---该函数基本不用
	static void * Reallocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void * result = realloc(p, new_sz);
		//无法满足需求时，改用oom_realloc()
		if (0 == result)
			result = Oom_Realloc(p, new_sz);
		return result;
	}
	// 模拟set_new_handle
	// 该函数的参数为函数指针，返回值类型也为函数指针
	// void (* set_malloc_handler( void (*f)() ) )()
	typedef void(*PFUNC)();
	static PFUNC set_malloc_handler(PFUNC f)
	//static void(*set_malloc_handler(void(*f)()))()
	{
		void(*old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return(old);
	}
};


void(*OneSpce::__malloc_alloc_oom_handler)() = 0;  //如果没有设置的话，在下面的函数就只能会抛异常


// malloc申请空间失败时代用该函数
void * OneSpce::Oom_Malloc(size_t n)
{
	void(*my_malloc_handler)();
	void *result;
	for (;;)   //不断的尝试释放、配置、再释放、在配置
	{
		// 检测用户是否设置空间不足应对措施，如果没有设置，抛异常，模式new的方式
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler)
		{
			__THROW_BAD_ALLOC;
		}
		// 如果设置，执行用户提供的空间不足应对措施
		(*my_malloc_handler)();
		// 继续申请空间，可能就会申请成功
		result = malloc(n);
		if (result)
			return(result);
	}
}


// 类似oom_malloc
void* OneSpce::Oom_Realloc(void *p,size_t n)
{
	void(*my_malloc_handler)();
	void *result;
	for (;;)
	{
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler)
		{
			__THROW_BAD_ALLOC;
		}
		(*my_malloc_handler)();  //调用处理例程，企图释放内存
		result = realloc(p, n);  //再次尝试设置内存 
		if (result)
			return(result);
	}
}
