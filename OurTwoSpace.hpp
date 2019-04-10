#pragma once
#include"OurOneSpace.hpp"

class TwoSpace
{
private:
	enum { __ALIGN = 8 }; // ����û������ڴ治��8�������������϶��뵽8��������
	enum { __MAX_BYTES = 128 }; // ��С�ڴ��ķֽ���
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN }; // ���ù�ϣͰ����С���ڴ�ʱ����Ͱ�ĸ���
	// ����û������ڴ�鲻��8�������������϶��뵽8��������
	static size_t ROUND_UP(size_t bytes)
	{
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}

private:
	// ����������ά������ṹ---�ȽϽ�ʡ�ռ�
	union obj
	{
		union obj * free_list_link;
		char client_data[1]; /* The client sees this. */
	};
private:
	static obj * free_list[__NFREELISTS];
	// ��ϣ�����������û��ṩ�ֽ����ҵ���Ӧ��Ͱ��
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}
	// start_free��end_free��������ڴ���д���ڴ����ʼ��ĩβλ��
	static char *start_free;
	static char *end_free;
	// ������¼�ÿռ��������Ѿ���ϵͳ��Ҫ�˶��ٵ��ڴ��
	static size_t heap_size;

	// �������ܣ���ռ���������Ҫ�ռ�
	// ����n: �û�����ռ��ֽ���
	// ����ֵ�����ؿռ���׵�ַ
public:
	static void * Allocate(size_t n)
	{
		obj * volatile * my_free_list;
		obj * result;
		//restrict ��ֻ���������޶���Լ��ָ�룬������ָ���Ƿ���һ�����ݶ����Ψһ�ҳ�ʼ�ķ�ʽ.�������߱�������
		//�����޸ĸ�ָ����ָ���ڴ������ݵĲ���������ͨ����ָ�����޸�,������ͨ������;��(����������ָ��)���޸�

		// ����û�����ռ��ͷų���128(���Ƿ�ΪС���ڴ�)
		if (n > (size_t)__MAX_BYTES)
		{
			// ����С���ڴ潻��һ���ռ�����������
			return (OneSpce::Allocate(n));
		}
		// �����û������ֽ��ҵ���Ӧ��Ͱ��
		my_free_list = free_list + FREELIST_INDEX(n);
		result = *my_free_list;
		// �����Ͱ��û���ڴ��ʱ�����Ͱ�в���ռ�
		if (result == 0)
		{
			// ��n���϶��뵽8������������֤��Ͱ�в����ڴ��ʱ���ڴ��һ����8��������
			void *r = refill(ROUND_UP(n));
			return r;
		}
		// ά��Ͱ��ʣ���ڴ�����ʽ��ϵ
		*my_free_list = result->free_list_link;
		return (result);
	}
public:
	// �������ܣ��û����ռ�黹���ռ�������
	// ������p�ռ��׵�ַ n�ռ��ܴ�С
	static void Deallocate(void *p, size_t n)
	{
		obj *q = (obj *)p;
		obj ** my_free_list;
		// ����ռ䲻��С���ڴ棬����һ���ռ�����������
		if (n > (size_t)__MAX_BYTES)
		{
			OneSpce::Deallocate(p, n);
			return;
		}
		// �ҵ���Ӧ�Ĺ�ϣͰ�����ڴ���ڹ�ϣͰ��
		my_free_list = free_list + FREELIST_INDEX(n);
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}
private:
	// �������ܣ����ϣͰ�в���ռ�
	// ����n��С���ڴ��ֽ���
	// ����ֵ���׸�С���ڴ���׵�ַ
	static void* refill(size_t n)
	{
		// һ�������ڴ����Ҫ20��n�ֽڵ�С���ڴ�
		int nobjs = 20;
		char * chunk = chunk_alloc(n, nobjs);
		obj ** my_free_list;
		obj *result;
		obj *current_obj, *next_obj;
		int i;
		// ���ֻҪ��һ�飬ֱ�ӷ��ظ��û�ʹ��
		if (1 == nobjs)
			return(chunk);
		// �ҵ���Ӧ��Ͱ��
		my_free_list = free_list + FREELIST_INDEX(n);
		// ����һ�鷵��ֵ�û��������������ڶ�Ӧ��Ͱ��
		// ע���˴������߼��Ƚϼ򵥣�����׼��ʵ����΢�е㸴�ӣ�ͬѧ�ǿ����Լ�ʵ��
		result = (obj *)chunk;
		*my_free_list = next_obj = (obj *)(chunk + n);
		for (i = 1; ; i++)
		{
			current_obj = next_obj;
			next_obj = (obj *)((char *)next_obj + n);
			if (nobjs - 1 == i)
			{
				current_obj->free_list_link = 0;
				break;
			}
			else
			{
				current_obj->free_list_link = next_obj;
			}
		}
		return(result);
	}

	static char* chunk_alloc(size_t size, int& nobjs)
	{
		// ����nobjs��size�ֽ��ڴ����ܴ�С�Լ��ڴ����ʣ��ռ��ܴ�С
		char * result;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;
		// ����ڴ�ؿ����ṩtotal_bytes�ֽڣ�����
		if (bytes_left >= total_bytes)
		{
			result = start_free;
			start_free += total_bytes;
			return(result);
		}
		else if (bytes_left >= size)
		{
			// nobjs���޷��ṩ���������ٿ����ṩ1��size�ֽ��ڴ�飬�ṩ�󷵻�
			nobjs = bytes_left / size;
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;
			return(result);
		}
		else
		{
			// �ڴ�ؿռ䲻�㣬��һ��С����ڶ������ṩ
			// ��ϵͳ�����������ڴ���в���ռ�
			// �������ڴ��в���ռ��С�����οռ��ܴ�С���� + ��ϵͳ�����ܴ�С/16
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			// ����ڴ����ʣ��ռ�(�ÿռ�һ����8��������)�����ÿռ�ҵ���Ӧ��ϣͰ��
			if (bytes_left > 0)
			{
				// �Ҷ��ù�ϣͰ����ʣ��ռ��������
				obj ** my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj *)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj *)start_free;
			}
			// ͨ��ϵͳ�����ڴ�ز���ռ䣬�������ɹ����ݹ��������
			start_free = (char *)malloc(bytes_to_get);
			if (0 == start_free)
			{
				// ͨ��ϵͳ�Ѳ���ռ�ʧ�ܣ��ڹ�ϣͰ�����Ƿ���û��ʹ�õĽϴ���ڴ��
				int i;
				obj ** my_free_list, *p;
				for (i = size; i <= __MAX_BYTES; i += __ALIGN)
				{
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					// ����У������ڴ�鲹����ڴ�أ��ݹ��������
					if (0 != p)
					{
						*my_free_list = p->free_list_link;
						start_free = (char *)p;
						end_free = start_free + i;
						return(chunk_alloc(size, nobjs));
					}
				}
				// ɽ��ˮ����ֻ����һ���ռ�����������
				// ע�⣺�˴�һ��Ҫ��end_free�ÿգ���Ϊһ���ռ�������һ�����쳣�ͻ������
				end_free = 0;
				start_free = (char*)OneSpce::Allocate(bytes_to_get);
			}
			// ͨ��ϵͳ�����ڴ�ز���ռ�ɹ���������Ϣ����������
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return(chunk_alloc(size, nobjs));
		}
	}
};

char* TwoSpace::start_free = 0;
char* TwoSpace::end_free = 0;
size_t TwoSpace::heap_size = 0;

TwoSpace::obj* TwoSpace::free_list[__NFREELISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };