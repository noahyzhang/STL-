#pragma once
#include<cstdlib>  //Ϊ��malloc��free
#include<iostream>

#define __THROW_BAD_ALLOC std::cerr<<"out of memory"<< std::endl; exit(1)

class OneSpce
{
private:
	//�����Ǻ���ָ�룬������ĺ��������������ڴ治������
	//oom��out of memory
	static void *Oom_Malloc(size_t);
	static void *Oom_Realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();
public:
	// ��malloc�ķ�װ
	static void * Allocate(size_t n)
	{
		// ����ռ�ɹ���ֱ�ӷ��أ�ʧ�ܽ���oom_malloc����
		void *result = malloc(n);  //һ���ռ�������ֱ��ʹ�� malloc()
		//�޷���������ʱ������ oom_malloc() 
		if (0 == result)
			result = Oom_Malloc(n);
		return result;
	}
	// ��free�ķ�װ
	static void Deallocate(void *p, size_t /* n */)  //���size_t��ȫ���Բ�Ҫ������Ϊ��ͳһ�ӿڣ����������size_t
	{
		free(p);  //һ���ռ�������ֱ��ʹ�� free()
	}

	// ��realloc�ķ�װ---�ú�����������
	static void * Reallocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void * result = realloc(p, new_sz);
		//�޷���������ʱ������oom_realloc()
		if (0 == result)
			result = Oom_Realloc(p, new_sz);
		return result;
	}
	// ģ��set_new_handle
	// �ú����Ĳ���Ϊ����ָ�룬����ֵ����ҲΪ����ָ��
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


void(*OneSpce::__malloc_alloc_oom_handler)() = 0;  //���û�����õĻ���������ĺ�����ֻ�ܻ����쳣


// malloc����ռ�ʧ��ʱ���øú���
void * OneSpce::Oom_Malloc(size_t n)
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
		(*my_malloc_handler)();  //���ô������̣���ͼ�ͷ��ڴ�
		result = realloc(p, n);  //�ٴγ��������ڴ� 
		if (result)
			return(result);
	}
}
