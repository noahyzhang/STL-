#include"OurTwoSpace.hpp"

void TestSpace()
{
	//static void * Allocate(size_t n)
	//static void Deallocate(void *p, size_t n)

	int* p = (int*)TwoSpace::Allocate(10);
	TwoSpace::Deallocate(p, 10);
}