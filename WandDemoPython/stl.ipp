#pragma once

#define CreateArray(Name, T, N) CreateListToCollection<T, array<T, N>>(CreateIndexing<size_t, T, array<T, N>>(CreateClass<array<T, N>>(Name)))
#define CreateVector(Name, T) CreateListToCollectionWithResize<T, vector<T>>(CreateIndexing<size_t, T, vector<T>>(CreateClass<vector<T>>(Name)))
