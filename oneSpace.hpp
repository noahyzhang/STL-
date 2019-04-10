#pragma once
//一级空间配置器
//注意：无“template型别参数”，至于“非型别参数”inst，则完全没有派上用场
#include<malloc.h>

#if 0
#include<new>
#define __THROW_BAD_ALLOC throw bad_alloc
#else !defined(__THROW_BAD_ALLOC)
#include<iostream>
#define __THROW_BAD_ALLOC std::cerr<<"out of memory"<< std::endl;exit(1)
#endif


class __malloc_alloc_template
{
private:
	//以下是函数指针，所代表的函数将用来处理内存不足的情况
	//oom：out of memory
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();
public:
	// 对malloc的封装
	static void * allocate(size_t n)
	{
		// 申请空间成功，直接返回，失败交由oom_malloc处理
		void *result = malloc(n);  //一级空间配置器直接使用 malloc()
		//无法满足需求时，改用 oom_malloc() 
		if (0 == result)
			result = oom_malloc(n); 
		return result;
	}
	// 对free的封装
	static void deallocate(void *p, size_t /* n */)  //这个size_t完全可以不要，但是为了统一接口，加上了这个size_t
	{
		free(p);  //一级空间配置器直接使用 free()
	}
	// 对realloc的封装---该函数基本不用
	static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void * result = realloc(p, new_sz);
		//无法满足需求时，改用oom_realloc()
		if (0 == result)
			result = oom_realloc(p, new_sz);
		return result;
	}
	// 模拟set_new_handle
	// 该函数的参数为函数指针，返回值类型也为函数指针
	// void (* set_malloc_handler( void (*f)() ) )()
	static void(*set_malloc_handler(void(*f)()))()
	{
		void(*old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return(old);
	}
};


void(*__malloc_alloc_template::__malloc_alloc_oom_handler)() = 0;

// malloc申请空间失败时代用该函数
void * __malloc_alloc_template::oom_malloc(size_t n)
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
void * __malloc_alloc_template::oom_realloc(void *p, size_t n)
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

typedef __malloc_alloc_template malloc_alloc;