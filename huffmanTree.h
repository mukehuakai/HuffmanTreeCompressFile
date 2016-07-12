#pragma once
#include"Heap.h"
//#include<iostream>
//#include<cassert>
//using namespace std;
template<class T>
struct HuffmanNode
{
	T _weight;
	HuffmanNode<T>* _parent;
	HuffmanNode<T>* _left;
	HuffmanNode<T>* _right;
	HuffmanNode(const T& weight)
		:_weight(weight)
		, _parent(NULL)
		, _left(NULL)
		, _right(NULL)
	{}
};
template<class T>
class HuffmanTree
{
	typedef HuffmanNode<T> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}
	~HuffmanTree()
	{
		_Destroy(_root);
		_root = NULL;
	}
	Node* GetRootNode()
	{
		return _root;
	}
	void CreateHuffmanTree(const T* array, size_t size, const T& invalid)
	{
		assert(array && size > 0);
		struct Compare
		{
			bool operator()(Node*& l, Node*& r)
			{
				return (l->_weight < r->_weight);
			}
		};
		////Heap<Node*,Compare> minHeap(array, size);
		Heap<Node*, Compare> minHeap;
		for (size_t i = 0; i < size; ++i)
		{
			if (array[i] != invalid)
			{
				Node* node = new Node(array[i]);
				minHeap.Push(node);
			}

		}
		if (minHeap.Empty())
			return;
		Node* parent = minHeap.GetTop();
		while (minHeap.Size() > 1)
		{
			Node* first = minHeap.GetTop();
			minHeap.Pop();
			Node* second = minHeap.GetTop();
			minHeap.Pop();
			parent = new Node(first->_weight+second->_weight);
			parent->_left = first;
			parent->_right = second;
			minHeap.Push(parent);
		}
		_root = parent;
	}
	void LevelOrder()
	{
		queue<Node*> q;
		if (_root == NULL)
			return;
		q.push(_root);
		while (!q.empty())
		{
			Node* cur = q.front();
			q.pop();
			cout << cur->_weight << " ";
			if (cur->_left)
				q.push(cur->_left);
			if (cur->_right)
				q.push(cur->_right);
		}
	}
private:
	void _Destroy(Node*& root)
	{
		if (root == NULL)
			return;
		_Destroy(root->_left);
		_Destroy(root->_right);
		delete root;
		root = NULL;
	}

protected:
	Node* _root;
};
void TestTree()
{
	int ar[] = { 2, 3, 6, 0, 4, 5, 1, 9, 7, 8 };
	//int ar[] = { 1,1,1,1,2,2 };
	HuffmanTree<int> tree;
	tree.CreateHuffmanTree(ar, sizeof(ar) / sizeof(ar[0]), -1);
	tree.LevelOrder();
}