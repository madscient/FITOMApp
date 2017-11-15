#pragma once

template<typename T> int vector_finder(std::vector<T, std::allocator<T> >& v, T e);
template<typename T1, typename T2> int vector_finder(std::vector<T1*, std::allocator<T1*> >& v, T2 e, T2 (T1::*pGetter)());
