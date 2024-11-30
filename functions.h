#pragma once
#include <iostream>
#include "myvector.h"

template <typename T, typename Allocator = std::allocator<T>>
void printMyVector(const MyVector<T, Allocator>& vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		std::cout << vec[i] << " ";
	}
	std::cout << "\n";
}