#pragma once

#define CreateArray(T, N) CreateIndexing<size_t, T, array<T, N>>(CreateClass<array<T, N>>())
#define CreateVector(T) CreateIndexing<size_t, T, vector<T>>(CreateClass<vector<T>>())

CreateArray(int, 2);
CreateVector(unsigned char);