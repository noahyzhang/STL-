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
	// ʵ�����ռ�������
	typedef simple_alloc<T, Alloc> list_node_allocator;
	// ...
public:
	link_type get_node()
	{
		// ���ÿռ��������ӿ�������ڵ�Ŀռ�
		return list_node_allocator::allocate();  // ����һ��T���Ͷ����С�Ŀռ�
	}
	// ���ڵ�黹���ռ�������
	void put_node(link_type p)
	{
		list_node_allocator::deallocate(p);  // �ͷ�һ��T���Ͷ����С�Ŀռ�
	}
	// �����ڵ㣺1. ����ռ� 2. ��ɽڵ㹹��
	//link_type create_node(const T& x)
	//{
	//	link_type p = get_node();  // ����һ��T���Ͷ����С�Ŀռ�
	//	construct(&p->data, x);   // �ռ�����ú���øú���������placement-new��ɶ���Ĺ���
	//	return p;
	//}
	//// ���ٽڵ㣺 1. ����������������ڵ�����Դ 2. ���ڵ�ռ�黹���ռ�������
	//void destroy_node(link_type p)
	//{
	//	destroy(&p->data);    // �黹�ռ�ʱ�����ȵ��øú�������������Դ�����
	//	put_node(p);         // ���ڵ�黹���ռ�������
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