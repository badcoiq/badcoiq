/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#ifndef __BQ_LIST_H__
#define __BQ_LIST_H__

template<typename _type>
struct bqListNode
{
	bqListNode() {}
	~bqListNode() {}
	_type m_data;
	bqListNode* m_left = 0;
	bqListNode* m_right = 0;
};

// circular double linked list
// Хвост указывает на голову, голова на хвост.
template<typename _type>
class bqList
{
	bqList(const bqList& other) = delete;
	bqList(bqList&& other) = delete;
	bqList& operator=(const bqList&) = delete;
	bqList& operator=(bqList&&) = delete;

public:
	bqList() :m_head(0) {}
	~bqList() 
	{
		clear();
	}

	// Найти ноду по объекту
	bqListNode<_type>* find(const _type& data)
	{
		if (!m_head)
			return 0;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			if (node->m_data == data)
				return node;

			if (node == last)
				break;
			node = node->m_right;
		}
		return 0;
	}

	// Удалить все ноды
	void clear()
	{
		m_size = 0;

		if (!m_head)
			return;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = m_head->m_right;
			m_head->~bqListNode();
			bqMemory::free(m_head);

			if (m_head == last)
				break;
			m_head = next;
		}
		m_head = nullptr;
	}

	// Вставить ноду после ноды имеющей объект after_this
	// Вернётся новая нода
	bqListNode<_type>* insert_after(const _type& after_this, const _type& data) 
	{
		bqListNode<_type>* node = (bqListNode<_type>*)bqMemory::malloc(sizeof(bqListNode<_type>));
		new(node)bqListNode<_type>();

		node->m_data = data;
		++m_size;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto c = m_head;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_data == after_this)
				{
					auto r = c->m_right;

					node->m_left = c;
					node->m_right = r;

					c->m_right = node;
					r->m_left = node;

					return node;
				}

				if (c == l)
					break;
				c = c->m_right;
			}

			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
		return node;
	}

	// Вставить ноду перед нодой имеющей объект before_this
	// Вернётся новая нода
	bqListNode<_type>* insert_before(const _type& before_this, const _type& data)
	{
		bqListNode<_type>* node = (bqListNode<_type>*)bqMemory::malloc(sizeof(bqListNode<_type>));
		new(node)bqListNode<_type>();

		node->m_data = data;
		++m_size;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto c = m_head;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_data == before_this)
				{
					auto l = c->m_left;

					node->m_left = l;
					node->m_right = c;

					c->m_left = node;
					l->m_right = node;

					return node;
				}

				if (c == l)
					break;
				c = c->m_right;
			}

			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
		return node;
	}

	// Вставить в конец
	bqListNode<_type>* push_back(const _type& data)
	{
		bqListNode<_type>* node = (bqListNode<_type>*)bqMemory::malloc(sizeof(bqListNode<_type>));
		new(node)bqListNode<_type>();
		++m_size;

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
		return node;
	}

	// Вставить в начало
	bqListNode<_type>* push_front(const _type& data)
	{
		bqListNode<_type>* node = (bqListNode<_type>*)bqMemory::malloc(sizeof(bqListNode<_type>));
		new(node)bqListNode<_type>();
		++m_size;

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			node->m_right = m_head;
			node->m_left = m_head->m_left;
			m_head->m_left->m_right = node;
			m_head->m_left = node;
			m_head = node;
		}
		return node;
	}

	// Удалить начало.
	void pop_front()
	{
		if (m_head)
		{

			--m_size;

			auto next = m_head->m_right;
			auto last = m_head->m_left;
			m_head->~bqListNode();
			bqMemory::free(m_head);

			if (next == m_head)
			{
				m_head = nullptr;
			}
			else
			{
				m_head = next;
				next->m_left = last;
				last->m_right = next;
			}
		}
	}

	// Удалить конец
	void pop_back()
	{
		if (!m_head)
			return;

		--m_size;

		auto lastNode = m_head->m_left;
		lastNode->m_left->m_right = m_head;
		m_head->m_left = lastNode->m_left;

		lastNode->~bqListNode();
		bqMemory::free(lastNode);

		if (lastNode == m_head)
		{
			m_head = nullptr;
		}
	}

	// Удалить ноду по значению. Удалится первая попавшееся.
	bool erase_first(const _type& object) 
	{
		if (!m_head)
			return false;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data == object)
			{
				if (node == m_head)
					pop_front();
				else
				{
					node->m_left->m_right = node->m_right;
					node->m_right->m_left = node->m_left;
					node->~bqListNode();
					bqMemory::free(node);
					--m_size;

					// ???
					if (node == m_head)
						m_head = 0;
				}
				return true;
			}
			if (node == last)
				break;
			node = next;
		}
		return false;
	}

	// Заменить значения. первое попавшееся.
	void replace(const _type& oldObject, const _type& newObject) 
	{
		if (!m_head)
			return;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data == oldObject)
			{
				node->m_data = newObject;
				return;
			}
			if (node == last)
				break;
			node = next;
		}
	}

	// вывернуть наизнанку
	void reverse()
	{
		if (!m_head)
			return;
		bqListNode<_type>* tail = m_head->m_left;
		bqListNode<_type>* curr = m_head;
		while (true)
		{
			auto l = curr->m_left;
			auto r = curr->m_right;

			curr->m_left = r;
			curr->m_right = l;

			if (curr == tail)
				break;
			curr = r;
		}
		m_head = tail;
	}

	// Удалить ноду.
	void erase_by_node(bqListNode<_type>* object)
	{
		if (!m_head)
			return;

		object->m_left->m_right = object->m_right;
		object->m_right->m_left = object->m_left;

		if (object == m_head)
			m_head = m_head->m_right;

		if (object == m_head)
			m_head = 0;

		--m_size;

		object->~bqListNode();
		bqMemory::free(object);
	}

	_type& front()
	{
		return m_head->m_data;
	}

	class Iterator{
		bqList<_type>* m_list = 0;
		bqListNode<_type>* m_node = 0;
	public:
		Iterator(bqList<_type>* _l, bqListNode<_type>* n) :m_list(_l),m_node(n){}
		~Iterator() {}
		Iterator& operator++(){
			if(m_node == m_list->m_head->m_left)
			{
				m_node = 0;
				return *this;
			}

			m_node = m_node->m_right;
			return (*this);
		}
		bool operator ==(const Iterator& other) const {	return m_node == other.m_node;}
		bool operator !=(const Iterator& other) const {	return m_node != other.m_node;}
		const _type& operator*() {	return m_node->m_data;}
		_type* operator->() {	return &m_node->m_data;}
	};

	Iterator begin() {return Iterator(this, m_head);}
	Iterator end() {return Iterator(this, 0);}


	bqListNode<_type>* m_head;

	size_t m_size = 0;
	size_t size() const { return m_size; }
};

#include <mutex>
template<typename _type>
class bqThreadList
{
	bqList<_type> m_container;
	std::mutex m_mutex;
public:
	bqThreadList() {}
	~bqThreadList()
	{
		clear();
		if (m_mutex.try_lock())
			m_mutex.unlock();
	}

	bqListNode<_type>* find(const _type& data) 
	{
		m_mutex.lock();
		bqListNode<_type>* r = m_container.find(data);
		m_mutex.unlock();
		return r;
	}

	void clear()
	{
		m_mutex.lock();
		m_container.clear();
		m_mutex.unlock();
	}

	bqListNode<_type>* insert_after(const _type& after_this, const _type& data)
	{
		m_mutex.lock();
		bqListNode<_type>* r = m_container.insert_after(after_this, data);
		m_mutex.unlock();
		return r;
	}

	bqListNode<_type>* insert_before(const _type& before_this, const _type& data)
	{
		m_mutex.lock();
		bqListNode<_type>* r = m_container.insert_before(before_this, data);
		m_mutex.unlock();
		return r;
	}

	bqListNode<_type>* push_back(const _type& data)
	{
		m_mutex.lock();
		bqListNode<_type>* r = m_container.push_back(data);
		m_mutex.unlock();
		return r;
	}

	bqListNode<_type>* push_front(const _type& data)
	{
		m_mutex.lock();
		bqListNode<_type>* r = m_container.push_front(data);
		m_mutex.unlock();
		return r;
	}

	void pop_front()
	{
		m_mutex.lock();
		m_container.pop_front();
		m_mutex.unlock();
	}

	void pop_back()
	{
		m_mutex.lock();
		m_container.pop_back();
		m_mutex.unlock();
	}

	bool erase_first(const _type& object)
	{
		m_mutex.lock();
		bool r = m_container.erase_first(object);
		m_mutex.unlock();
		return r;
	}

	void replace(const _type& oldObject, const _type& newObject)
	{
		m_mutex.lock();
		m_container.replace(oldObject, newObject);
		m_mutex.unlock();
	}

	void replace()
	{
		m_mutex.lock();
		m_container.reverse();
		m_mutex.unlock();
	}

	void erase_by_node(bqListNode<_type>* object)
	{
		m_mutex.lock();
		m_container.erase_by_node(object);
		m_mutex.unlock();
	}

	_type& front()
	{
		m_mutex.lock();
		_type& r = m_container.front();
		m_mutex.unlock();
		return r;
	}

	size_t size() const { return m_container.m_size; }
	bool empty() const { return m_container.m_size == 0; }
};

#endif

