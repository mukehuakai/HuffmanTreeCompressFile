#pragma once
#include<vector>
#include<queue>
#include<cassert>
#include<iostream>
using namespace std;
template<class T>
struct Greater
{
	bool operator()(const T& left,const T& right)
	{
		return left > right;
	}
};
template<class T>
struct Less
{
	bool operator()(const T& left, const T& right)
	{
		return left < right;
	}
};
template<class T,class Compare = Less<T>>
class Heap
{
public:
	Heap()
	{
	}
	Heap(const T* array, size_t size)
	{
		assert(array);
		for (size_t i = 0; i < size; ++i)
		{
			_vec.push_back(array[i]);
		}
		for (int i = _vec.size() / 2 - 1; i >= 0; --i)
		{
			_AdjustDown(_vec, i, _vec.size());
		}
	}
	Heap(const vector<T>& vec)
	{
		_vec.swap(vec);
		for (int i = _vec.size() / 2 - 1; i >= 0; --i)
		{
			_AdjustDown(_vec, i, _vec.size());
		}
	}
	void Push(const T& x)
	{
		_vec.push_back(x);
		if (_vec.size() > 0)
			_AdjustUp(_vec, _vec.size() - 1);
	}
	void Pop()
	{
		swap(_vec[0], _vec[_vec.size() - 1]);
		_vec.pop_back();
		_AdjustDown(_vec, 0, _vec.size());
	}
	const T& GetTop()
	{
		assert(_vec.size() > 0);
		return _vec[0];
	}
	bool Empty()
	{
		return _vec.empty();
	}
	size_t Size()
	{
		return _vec.size();
	}
private:
	void _AdjustDown(vector<T>& vec,int root,int size)
	{
		int left = root * 2 + 1;
		while (left < size)
		{
			if (left+1 < size && Compare()(vec[left+1], vec[left]))
				++left;
			if (Compare()(vec[left], vec[root]))
			{
				swap(vec[left], vec[root]);
				root = left;
				left = root * 2 + 1;
			}
			else
				break;
		}
	}
	void _AdjustUp(vector<T>& vec,int index)
	{
		int parent = index >> 1;
		while (Compare()(vec[index], vec[parent]))
		{
			swap(vec[index], vec[parent]);
			index = parent;
			parent = index >> 1;
		}
	}
protected:
	vector<T> _vec;
};
void test()
{
//	int arr[] = { 1, 2, 8, 9, 7, 4, 6, 5, 11, 10 };
	int arr[] = { 1};
	Heap<int> h(arr, sizeof(arr) / sizeof(arr[0]));
	h.Push(0);
	h.Pop();
	cout << h.GetTop() << endl;
	h.Pop();
}