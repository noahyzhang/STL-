#if 0
#pragma once

#include"TwoSpace.hpp"

// T: Ԫ������
// Alloc: �ռ�������
// ע�⣺����ֻ����������黹����Ŀռ䣬������ռ��ж���Ĺ���������
template<class T, class Alloc>
class simple_alloc
{
public:
	// ����n��T���Ͷ����С�Ŀռ�
	static T *allocate(size_t n)
	{
		return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
	}
	// ����һ��T���Ͷ����С�Ŀռ�
	static T *allocate(void)
	{
		return (T*)Alloc::allocate(sizeof(T));
	}
	// �ͷ�n��T���Ͷ����С�Ŀռ�
	static void deallocate(T *p, size_t n)
	{
		if (0 != n)
			Alloc::deallocate(p, n * sizeof(T));
	}
	// �ͷ�һ��T���Ͷ����С�Ŀռ�
	static void deallocate(T *p)
	{
		Alloc::deallocate(p, sizeof(T));
	}
};

// �黹�ռ�ʱ�����ȵ��øú�������������Դ�����
template <class T>
inline void destroy(T* pointer)
{
	pointer->~T();
}

// �ռ�����ú���øú���������placement-new��ɶ���Ĺ���
template <class T1, class T2>
inline void construct(T1* p, const T2& value)
{
	new (p) T1(value);
}


void TestAlloc()
{
	simple_alloc<int, __default_alloc_template> space;
	int* arr = space.allocate(10);
	int* oneArr = space.allocate();

	arr[8] = 10;
	std::cout << sizeof(arr) / sizeof(arr[0]) << std::endl;
	*oneArr = 3;
	std::cout << sizeof(oneArr) / sizeof(oneArr[0]) << std::endl;

	space.deallocate(oneArr);
	space.deallocate(arr, 10);
}

#endif