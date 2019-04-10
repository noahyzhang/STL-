#if 0
#pragma once

#include"TwoSpace.hpp"

// T: 元素类型
// Alloc: 空间配置器
// 注意：该类只负责申请与归还对象的空间，不否则空间中对象的构造与析构
template<class T, class Alloc>
class simple_alloc
{
public:
	// 申请n个T类型对象大小的空间
	static T *allocate(size_t n)
	{
		return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
	}
	// 申请一个T类型对象大小的空间
	static T *allocate(void)
	{
		return (T*)Alloc::allocate(sizeof(T));
	}
	// 释放n个T类型对象大小的空间
	static void deallocate(T *p, size_t n)
	{
		if (0 != n)
			Alloc::deallocate(p, n * sizeof(T));
	}
	// 释放一个T类型对象大小的空间
	static void deallocate(T *p)
	{
		Alloc::deallocate(p, sizeof(T));
	}
};

// 归还空间时，先先调用该函数将对象中资源清理掉
template <class T>
inline void destroy(T* pointer)
{
	pointer->~T();
}

// 空间申请好后调用该函数：利用placement-new完成对象的构造
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