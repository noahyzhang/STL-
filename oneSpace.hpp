#pragma once
//һ���ռ�������
//ע�⣺�ޡ�template�ͱ�����������ڡ����ͱ������inst������ȫû�������ó�
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
	//�����Ǻ���ָ�룬������ĺ��������������ڴ治������
	//oom��out of memory
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();
public:
	// ��malloc�ķ�װ
	static void * allocate(size_t n)
	{
		// ����ռ�ɹ���ֱ�ӷ��أ�ʧ�ܽ���oom_malloc����
		void *result = malloc(n);  //һ���ռ�������ֱ��ʹ�� malloc()
		//�޷���������ʱ������ oom_malloc() 
		if (0 == result)
			result = oom_malloc(n); 
		return result;
	}
	// ��free�ķ�װ
	static void deallocate(void *p, size_t /* n */)  //���size_t��ȫ���Բ�Ҫ������Ϊ��ͳһ�ӿڣ����������size_t
	{
		free(p);  //һ���ռ�������ֱ��ʹ�� free()
	}
	// ��realloc�ķ�װ---�ú�����������
	static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void * result = realloc(p, new_sz);
		//�޷���������ʱ������oom_realloc()
		if (0 == result)
			result = oom_realloc(p, new_sz);
		return result;
	}
	// ģ��set_new_handle
	// �ú����Ĳ���Ϊ����ָ�룬����ֵ����ҲΪ����ָ��
	// void (* set_malloc_handler( void (*f)() ) )()
	static void(*set_malloc_handler(void(*f)()))()
	{
		void(*old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return(old);
	}
};


void(*__malloc_alloc_template::__malloc_alloc_oom_handler)() = 0;

// malloc����ռ�ʧ��ʱ���øú���
void * __malloc_alloc_template::oom_malloc(size_t n)
{
	void(*my_malloc_handler)();
	void *result;
	for (;;)   //���ϵĳ����ͷš����á����ͷš�������
	{
		// ����û��Ƿ����ÿռ䲻��Ӧ�Դ�ʩ�����û�����ã����쳣��ģʽnew�ķ�ʽ
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler)
		{
			__THROW_BAD_ALLOC;
		}
		// ������ã�ִ���û��ṩ�Ŀռ䲻��Ӧ�Դ�ʩ
		(*my_malloc_handler)();
		// ��������ռ䣬���ܾͻ�����ɹ�
		result = malloc(n);
		if (result)
			return(result);
	}
}
// ����oom_malloc
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
		(*my_malloc_handler)();  //���ô������̣���ͼ�ͷ��ڴ�
		result = realloc(p, n);  //�ٴγ��������ڴ� 
		if (result)
			return(result);
	}
}

typedef __malloc_alloc_template malloc_alloc;