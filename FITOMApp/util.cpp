#include "stdafx.h"
#include "util.h"

template<typename T> int vector_finder(std::vector<T, std::allocator<T> >& v, T e)
{
	auto found_itr = std::find(v.begin(), v.end(), e);
	if (found_itr != v.end())
	{
		return std::distance(v.begin(), found_itr);
	}
	return -1;
}

template<typename T1, typename T2> int vector_finder(std::vector<T1*, std::allocator<T1*> >& v, T2 e, T2(T1::*pGetter)())
{
	for (auto found_itr = v.begin(); found_itr < v.end(); found_itr++) {
		if ((*found_itr)->*pGetter() == e) {
			return std::distance(v.begin(), found_itr);
		}
	}
	return -1;
}
