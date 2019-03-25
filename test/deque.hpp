#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
	private:
		struct node {
			node *pre, *nxt;
			T val;
			explicit node (node *_pre = nullptr, node *_nxt = nullptr) {pre = _pre, nxt = _nxt;}
			explicit node (T _val, node *_pre = nullptr, node *_nxt = nullptr) {val = _val, pre = _pre, nxt = _nxt;}
		};
		struct block {
			node *head, *tail;
			block *pre, *nxt;
			int length;
			explicit block(int _length = 0, node *_head = nullptr, node *_tail = nullptr, block *_pre = nullptr, block *_nxt = nullptr) {
				length = _length;
				head = _head; tail = _tail;
				pre = _pre, nxt = _nxt;
			}
		};
		int length;
		block *head, *tail;

	public:
		class const_iterator;
		class iterator {
				friend class deque;
			private:
				deque *que;
				block *bl;
				node *p;
				inline int getpos() const {
					if(que == nullptr || bl == nullptr || p == nullptr) throw index_out_of_bound();
					iterator it = *this;
					int res = 0;
					while(it.p != it.bl -> head) {
						++res;
						it.p = it.p -> pre;
					}
					if(it.bl == it.que -> head) return res;
					it.bl = it.bl -> pre; ++res;
					while(it.bl != it.que -> head) {
						res += it.bl -> length;
						it.bl = it.bl -> pre;
					}
					it.p = it.bl -> tail;
					while(it.p != it.bl -> head) {
						++res;
						it.p = it.p -> pre;
					}
					return res;
				}
			public:
				iterator() {que = bl = p = nullptr;}
				iterator(deque *_que, block *_bl, node *_p) {que = _que, bl = _bl, p = _p;}
				iterator(const iterator &it) {que = it.que, bl = it.bl, p = it.p;}
				iterator(const const_iterator &it) {que = it.que, bl = it.bl, p = it.p;}
				iterator operator+(const int &n) const {
					if(n == 0) return *this;
					iterator ret = *this; int res = n;
					if(n > 0) {
						while (ret -> p != ret -> bl -> tail) {
							--res; ret -> p = ret -> p -> nxt;
							if(res == 0) return ret;
						}
						--res; ret -> bl = ret -> bl -> nxt;
						if(ret -> bl == nullptr) return iterator();
						while(res >= ret -> bl -> length) {
							res -= ret -> bl -> length;
							ret -> bl = ret -> bl -> nxt;
							if(ret -> bl == nullptr) return iterator();
						}
						ret -> p = ret -> bl -> head;
						while(res--) {
							ret -> p = ret -> p -> nxt;
							if(ret -> p == nullptr) return iterator();
						}
						return ret;
					} else {
						n = -n;
						while(ret -> p != ret -> bl -> head) {
							--res; ret -> p = ret -> p -> pre;
							if(res == 0) return ret;
						}
						--res; ret -> bl = ret -> bl -> pre;
						if(ret -> bl == nullptr) return iterator();
						while(res >= ret -> bl -> length) {
							res -= ret -> bl -> length;
							ret -> bl = ret -> bl -> pre;
							if(ret -> bl == nullptr) return iterator();
						}
						ret -> p = ret -> bl -> tail;
						while(res--) {
							ret -> p = ret -> p -> pre;
							if(ret -> p == nullptr) return iterator();
						}
						return ret;
					}
				}
				iterator operator-(const int &n) const {return *this + (-n);}
				// return th distance between two iterator,
				// if these two iterators points to different vectors, throw invaild_iterator.
				int operator-(const iterator &rhs) const {
					if(que != rhs.que) throw invalid_iterator();
					return this -> getpos() - rhs.getpos();
				}
				iterator& operator+=(const int &n) {*this = *this + n; return *this;}
				iterator& operator-=(const int &n) {*this = *this - n; return *this;}
				iterator operator++(int x) {iterator it = *this; *this += 1; return it;}
				iterator& operator++() {*this += 1; return *this;}
				iterator operator--(int x) {iterator it = *this; *this -= 1; return it;}
				iterator& operator--() {*this -= 1; return *this;}
				T& operator*() const {return p -> val;}
				T* operator->() const noexcept {return &(p -> val);}
				bool operator==(const iterator &rhs) const {return que == rhs.que && bl == rhs.bl && p == rhs.p;}
				bool operator==(const const_iterator &rhs) const {return que == rhs.que && bl == rhs.bl && p == rhs.p;}
				bool operator!=(const iterator &rhs) const {return que != rhs.que || bl != rhs.bl || p != rhs.p;}
				bool operator!=(const const_iterator &rhs) const {return que != rhs.que || bl != rhs.bl || p != rhs.p;}
		};
		class const_iterator {
				friend class deque;
			private:
				// TODO
			public:
				// TODO
		};
		/**
		 * TODO Constructors
		 */
		deque() {
			length = 0;
			head = new block();
			tail = head;
		}
		deque(const deque &other) {}
		/**
		 * TODO Deconstructor
		 */
		~deque() {}
		/**
		 * TODO assignment operator
		 */
		deque &operator=(const deque &other) {}
		/**
		 * access specified element with bounds checking
		 * throw index_out_of_bound if out of bound.
		 */
		T & at(const size_t &pos) {}
		const T & at(const size_t &pos) const {}
		T & operator[](const size_t &pos) {}
		const T & operator[](const size_t &pos) const {}
		/**
		 * access the first element
		 * throw container_is_empty when the container is empty.
		 */
		const T & front() const {}
		/**
		 * access the last element
		 * throw container_is_empty when the container is empty.
		 */
		const T & back() const {}
		/**
		 * returns an iterator to the beginning.
		 */
		iterator begin() {}
		const_iterator cbegin() const {}
		/**
		 * returns an iterator to the end.
		 */
		iterator end() {}
		const_iterator cend() const {}
		/**
		 * checks whether the container is empty.
		 */
		bool empty() const {}
		/**
		 * returns the number of elements
		 */
		size_t size() const {}
		/**
		 * clears the contents
		 */
		void clear() {}
		/**
		 * inserts elements at the specified locat on in the container.
		 * inserts value before pos
		 * returns an iterator pointing to the inserted value
		 *     throw if the iterator is invalid or it point to a wrong place.
		 */
		iterator insert(iterator pos, const T &value) {}
		/**
		 * removes specified element at pos.
		 * removes the element at pos.
		 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
		 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
		 */
		iterator erase(iterator pos) {}
		/**
		 * adds an element to the end
		 */
		void push_back(const T &value) {}
		/**
		 * removes the last element
		 *     throw when the container is empty.
		 */
		void pop_back() {}
		/**
		 * inserts an element to the beginning.
		 */
		void push_front(const T &value) {}
		/**
		 * removes the first element.
		 *     throw when the container is empty.
		 */
		void pop_front() {}
};

}

#endif
