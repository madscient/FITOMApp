#include "stdafx.h"
#include "util.h"

template<typename T> int vector_finder(std::vector<T>& v, T e)
{
	auto found_itr = std::find(v.begin(), v.end(), e);
	if (found_itr != v.end())
	{
		return std::distance(v.begin(), found_itr);
	}
	return -1;
}
