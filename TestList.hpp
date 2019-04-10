#if 0
#include"alloc.hpp"
#include<new.h>

//#ifdef __USE_MALLOC
//
//typedef __malloc_alloc_template<0> malloc_alloc;
//typedef malloc_alloc alloc;
//
//#else
//typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
//
//#endif // !__USE_MALLOC

template <class T, class Alloc>
class List
{
	typedef T* link_type;
	// 实例化空间配置器
	typedef simple_alloc<T, Alloc> list_node_allocator;
	// ...
public:
	link_type get_node()
	{
		// 调用空间配置器接口先申请节点的空间
		return list_node_allocator::allocate();  // 申请一个T类型对象大小的空间
	}
	// 将节点归还给空间配置器
	void put_node(link_type p)
	{
		list_node_allocator::deallocate(p);  // 释放一个T类型对象大小的空间
	}
	// 创建节点：1. 申请空间 2. 完成节点构造
	//link_type create_node(const T& x)
	//{
	//	link_type p = get_node();  // 申请一个T类型对象大小的空间
	//	construct(&p->data, x);   // 空间申请好后调用该函数：利用placement-new完成对象的构造
	//	return p;
	//}
	//// 销毁节点： 1. 调用析构函数清理节点中资源 2. 将节点空间归还给空间配置器
	//void destroy_node(link_type p)
	//{
	//	destroy(&p->data);    // 归还空间时，先先调用该函数将对象中资源清理掉
	//	put_node(p);         // 将节点归还给空间配置器
	//}

	//// ...
	//iterator insert(iterator position, const T& x)
	//{
	//	link_type tmp = create_node(x);
	//	tmp->next = position.node;
	//	tmp->prev = position.node->prev;
	//	(link_type(position.node->prev))->next = tmp;
	//	position.node->prev = tmp;
	//	return tmp;
	//}
	//iterator erase(iterator position)
	//{
	//	link_type next_node = link_type(position.node->next);
	//	link_type prev_node = link_type(position.node->prev);
	//	prev_node->next = next_node;
	//	next_node->prev = prev_node;
	//	destroy_node(position.node);
	//	return iterator(next_node);
	//}
	//// ...
};


void TestListAlloc()
{
	List<int, __default_alloc_template> st;
	
	int* p = st.get_node();
	//st.put_node(p);

}

#endif