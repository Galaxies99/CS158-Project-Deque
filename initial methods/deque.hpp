#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
#include "exceptions.hpp"

#include <cstddef>


# include<iostream>
using namespace std;

namespace sjtu { 

template<typename T>
class deque {
private:
  static const int CAPACITY_OF_NODE = 64;   // =========== NEED MODIFY !!! ============ //
  struct node {
    node *pre, *nxt;
    T *val;
    int size;
    node (node *pr = nullptr, node *nx = nullptr) {
      pre = pr, nxt = nx;
      val = (T*) malloc (sizeof(T) * CAPACITY_OF_NODE);
      size = 0;
    }
  };
  
  node *head, *tail; 
  size_t length;
  
  inline void nodeRelease(node *p) {
    if(p -> val) free(p -> val);
    delete p;
  }
  
  inline void nodeCopyData(node *p, T *r = nullptr) {
    if(r) {
      for (int i=0; i<CAPACITY_OF_NODE; ++i)
        p -> val[i] = r[i];
    }
  }
  
  inline void nodeInsert(node *p, int _size, T *r = nullptr) {
    node *q = new node(p, p -> nxt);
    p -> nxt = q;
    q -> nxt -> pre = q;
    nodeCopyData(q, r);
    q -> size = _size;
  }
  
public:
  
  class const_iterator;
  class iterator {
      friend class deque;
      int getpos() const {
        iterator it = *this;
        if(it.p == it.que -> head) return it.p -> size - 1 - it.pos;
        node *p = it.p;
        int ret = it.pos;
        p = p -> pre;
        while(p != it.que -> head) {
          ret += p -> size;
          p = p -> pre;
        }
        ret += p -> size;
        return ret;
      }
    private:
      deque *que;
      node *p;
      int pos;
    public:
      iterator() { que = nullptr, p = nullptr; pos = -1; }
      iterator(deque *_que, node *_p, int _pos) {
        que = _que;
        p = _p;
        pos = _pos;
      }
      iterator(const const_iterator &other) {
        que = other.que;
        p = other.p;
        pos = other.pos;
      }
      iterator(const iterator &other) {
        que = other.que;
        p = other.p;
        pos = other.pos;
      }
      iterator operator+(const int &n) const {
        int res = n;
        iterator ret = *this;
        // head
        if(ret.p == que -> head) {
          if(res <= ret.pos) {
            ret.pos -= res;
            return ret;
          } else {
            res -= ret.pos+1;
            ret.p = ret.p -> nxt;
            ret.pos = 0;
          }
        }
        // mid
        while(ret.p != que -> tail) {
          int r = ret.p -> size - 1 - ret.pos;
          if(res <= r) {
            ret.pos += res;
            return ret;
          } else {
            res -= r+1;
            ret.p = ret.p -> nxt;
            ret.pos = 0;
          }
        }
        // tail
        if(res <= ret.p -> size - ret.pos) {
          ret.pos += res;
          return ret;
        } else return iterator();
      }
      iterator operator-(const int &n) const {
        int res = n;
        iterator ret = *this;
        // tail && mid
        while(ret.p != que -> head) {
          if(res <= ret.pos) {
            ret.pos -= res;
            return ret;
          } else {
            res -= ret.pos + 1;
            ret.p = ret.p -> pre;
            if(ret.p == que -> head) ret.pos = 0;
            else ret.pos = ret.p->size - 1;
          }
        }
        // head
        if(res <= ret.p->size - 1) {
          ret.pos += res;
          return ret;
        } else return iterator();
      }
      // return th distance between two iterator,
      // if these two iterators points to different vectors, throw invaild_iterator.
      int operator-(const iterator &rhs) const {
        if(que != rhs.que) throw invalid_iterator();
        return this -> getpos() - rhs.getpos();
      }
      iterator operator+=(const int &n) {
        *this = *this + n;
        return *this; 
      }
      iterator operator-=(const int &n) {
        *this = *this - n;
        return *this;
      }
      iterator operator++(int x) {
        iterator ret = *this;
        *this += 1;
        return ret;
      }
      iterator& operator++() {
        *this += 1;
        return *this;
      }
      iterator operator--(int x) {
        iterator ret = *this;
        *this -= 1;
        return ret;
      }
      iterator& operator--() {
        *this -= 1;
        return *this;
      }
      T& operator*() const {
        return p -> val[pos];
      }
      T* operator->() const noexcept {
        return &(p -> val[pos]);   // ???
      }
      bool operator==(const iterator &rhs) const {
        return p == rhs.p && pos == rhs.pos;
      }
      bool operator==(const const_iterator &rhs) const {
        return p == rhs.p && pos == rhs.pos;
      }
      bool operator!=(const iterator &rhs) const {
        return !(*this == rhs);
      }
      bool operator!=(const const_iterator &rhs) const {
        return !(*this == rhs);
      }
  };
  class const_iterator {
      friend class deque;
      int getpos() const {
        iterator it = *this;
        if(it.p == it.que -> head) return it.p -> size - 1 - it.pos;
        node *p = it.p;
        int ret = it.pos;
        p = p -> pre;
        while(p != it.que -> head) {
          ret += p -> size;
          p = p -> pre;
        }
        ret += p -> size;
        return ret;
      }
    private:
      deque *que;
      node *p;
      int pos;

    public:
      const_iterator() { que = nullptr, p = nullptr; pos = -1; }
      const_iterator(const const_iterator &other) {
        que = other.que;
        p = other.p;
        pos = other.pos;
      }
      const_iterator(const iterator &other) {
        que = other.que;
        p = other.p;
        pos = other.pos;
      }
      // return th distance between two iterator,
      // if these two iterators points to different vectors, throw invaild_iterator.
      int operator-(const const_iterator &rhs) const {
        if(que != rhs.que) throw invalid_iterator();
        return this->getpos() - rhs.getpos(); 
      }
      T& operator*() const {
        return p -> val[pos];
      }
      T* operator->() const noexcept {
        return &(p -> val[pos]);   // ???
      }
      bool operator==(const iterator &rhs) const {
        return p == rhs.p && pos == rhs.pos;
      }
      bool operator==(const const_iterator &rhs) const {
        return p == rhs.p && pos == rhs.pos;
      }
      bool operator!=(const iterator &rhs) const {
        return !(*this == rhs);
      }
      bool operator!=(const const_iterator &rhs) const {
        return !(*this == rhs);
      }
  };
  
  deque() {
    length = 0;
    head = new node;
    tail = new node;
    head -> nxt = tail;
    tail -> pre = head;
  }
  
  deque(const deque &other) {
    length = other.length;
    head = new node; 
    tail = new node; 
    head -> nxt = tail; head -> size = other.head -> size;
    tail -> pre = head; tail -> size = other.tail -> size;
    nodeCopyData(head, other.head -> val);
    nodeCopyData(tail, other.tail -> val);
    
    node *q = head, *op = other.head -> nxt;
    while(op != other.tail) {
      nodeInsert(q, op -> size, op -> val);
      q = q -> nxt;
      op = op -> nxt;
    }
  }
  
  ~deque() {
    node *p = head, *q;
    while(p != tail) {
      q = p->nxt;
      nodeRelease(p);
      p = q;
    }
    nodeRelease(tail);
  }

  deque &operator=(const deque &other) {
    if(&other == this) return *this;
    // copy from ~deque()
    node *p = head, *q;
    while(p != tail) {
      q = p->nxt;
      nodeRelease(p);
      p = q;
    }
    nodeRelease(tail);
    
    // copy from deque(const deque &other)
    length = other.length;
    head = new node; 
    tail = new node; 
    head -> nxt = tail; head -> size = other.head -> size;
    tail -> pre = head; tail -> size = other.tail -> size;
    nodeCopyData(head, other.head -> val);
    nodeCopyData(tail, other.tail -> val);
    
    q = head; node *op = other.head -> nxt;
    while(op != other.tail) {
      nodeInsert(q, op -> size, op -> val);
      q = q -> nxt;
      op = op -> nxt;
    }
  }
  
  /**
   * access specified element with bounds checking
   * throw index_out_of_bound if out of bound.
   */
  T & at(const size_t &pos) {
    size_t ind = pos;
    if(ind < 0 || ind >= length) throw index_out_of_bound();
    if(ind < head -> size) return head -> val[head -> size - ind - 1];
    else ind -= head -> size;
    node *p = head -> nxt;
    while(p != nullptr) {
      if(ind < p -> size) return p -> val[ind];
      ind -= p -> size;
      p = p -> nxt;
    }
  }
  const T & at(const size_t &pos) const {
    size_t ind = pos;
    if(ind < 0 || ind >= length) throw index_out_of_bound();
    if(ind < head -> size) return head -> val[head -> size - ind - 1];
    else ind -= head -> size;
    node *p = head -> nxt;
    while(p != nullptr) {
      if(ind < p -> size) return p -> val[ind];
      ind -= p -> size;
      p = p -> nxt;
    }

  }
  T & operator[](const size_t &pos) {
    size_t ind = pos;
    if(ind < 0 || ind >= length) throw index_out_of_bound();
    if(ind < head -> size) return head -> val[head -> size - ind - 1];
    else ind -= head -> size;
    node *p = head -> nxt;
    while(p != nullptr) {
      if(ind < p -> size) return p -> val[ind];
      ind -= p -> size;
      p = p -> nxt;
    }
  }
  const T & operator[](const size_t &pos) const {
    size_t ind = pos;
    if(ind < 0 || ind >= length) throw index_out_of_bound();
    if(ind < head -> size) return head -> val[head -> size - ind - 1];
    else ind -= head -> size;
    node *p = head -> nxt;
    while(p != nullptr) {
      if(ind < p -> size) return p -> val[ind];
      ind -= p -> size;
      p = p -> nxt;
    }
  }
  const T & front() const {
    if(length == 0) throw container_is_empty();
    return head -> val[head -> size - 1];
  }
  const T & back() const {
    if(length == 0) throw container_is_empty();
    if(tail -> size == 0) return tail -> pre -> val[tail -> pre -> size - 1];
    else return tail -> val[tail -> size - 1];
  }
  iterator end() {
    return iterator(this, tail, tail -> size);
  }
  const_iterator cend() const {
    return const_iterator(this, tail, tail -> size);
  }
  iterator begin() {
    if(length == 0) return end();
    else return iterator(this, head, head -> size - 1);
  }
  const_iterator cbegin() const {
    if(length == 0) return end();
    else return const_iterator(this, head, head -> size - 1);
  }
  bool empty() const {
    return length == 0;
  }
  size_t size() const {
    return length;
  }
  void clear() {
    // copy from ~deque()
    node *p = head, *q;
    while(p != tail) {
      q = p->nxt;
      nodeRelease(p);
      p = q;
    }
    nodeRelease(tail);
    
    // copy from deque()
    length = 0;
    head = new node;
    tail = new node;
    head -> nxt = tail;
    tail -> pre = head;
  }
  void push_back(const T &value) {
    if(length == 0) {
      head -> val[head -> size] = value;
      head -> size ++;
      length ++;
      return ;
    }
    tail -> val[tail -> size] = value;
    tail -> size ++;
    length ++;
    if(tail -> size == CAPACITY_OF_NODE) {
      tail -> nxt = new node(tail, nullptr);
      tail = tail -> nxt;
    }
  }
  void pop_back() {
    if(length == 0) throw container_is_empty();
    if(tail -> size == 0) {
      if(tail -> pre != head) {
        tail = tail -> pre;
        delete tail -> nxt;
        tail -> nxt = nullptr;
      } else {
        for (int i=1; i < head -> size; ++i) head -> val[i-1] = head -> val[i];
        -- head -> size;
        return ;  
      }
    }
    tail -> size --;
    length --;
  }
  void push_front(const T &value) {
    length ++;
    if(head -> size == CAPACITY_OF_NODE) {
      T *tmp = (T*) malloc (sizeof(T) * CAPACITY_OF_NODE);
      for (int i=0; i<CAPACITY_OF_NODE; ++i) tmp[i] = head -> val[CAPACITY_OF_NODE - i - 1];
      for (int i=0; i<CAPACITY_OF_NODE; ++i) head -> val[i] = tmp[i];
      free(tmp); 
      head -> pre = new node(nullptr, head);
      head = head -> pre;
      head -> val[0] = value;
      head -> size ++;
    } else {
      head -> val[head -> size] = value;
      head -> size ++;
    }
  }
  void pop_front() {
    if(length == 0) throw container_is_empty();
    head -> size --;
    length --;
    if(head -> size == 0) {
      head = head -> nxt;
      delete head -> pre;
      head -> pre = nullptr;
      T *tmp = (T*) malloc (sizeof(T) * CAPACITY_OF_NODE);
      for (int i=0; i < head -> size; ++i) tmp[i] = head -> val[i];
      for (int i=0; i < head -> size; ++i) head -> val[i] = tmp[head -> size - i - 1];
      free(tmp); 
      if(head == tail) {
        tail = new node;
        head -> nxt = tail;
        tail -> pre = head;
      }
    }
  }

  /**
	 * inserts elements at the specified locate on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *   throw if the iterator is invalid or it point to a wrong place.
	 */
  iterator insert(iterator pos, const T &value) {
    if (pos.que != this) throw invalid_iterator();
    if (pos.p == nullptr) throw index_out_of_bound();
    if (pos.p == pos.que -> tail && pos.pos == pos.que -> tail -> size) {
      push_back(value);
      pos ++;
      return pos;
    }
    if (pos.pos >= pos.p -> size || pos.pos < 0) throw index_out_of_bound();
    if (pos.p == pos.que -> head && pos.pos == pos.que -> head -> size - 1) {
      push_front(value);
      pos --;
      return pos;
    }
    node *p = pos.p; int ind = pos.pos;
    T las = p -> val[ind];
    p -> val[ind] = value;
    while(p != nullptr) {
      T llas = p -> val[p -> size - 1];
      for (int i=p -> size - 1; i > ind + 1; --i) p -> val[i] = p -> val[i-1];
      p -> val[ind + 1] = las;
      las = llas;
      p = p -> nxt;
      ind = -1;
    }
    push_back(las);
    return pos;
  }
  /**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
  iterator erase(iterator pos) {
    if (length == 0) throw container_is_empty();
    if (pos.que != this) throw invalid_iterator();
    if (pos.p->val == nullptr) throw index_out_of_bound();
    if (pos.p == pos.que->tail && pos.pos == pos.que->tail->size - 1) {
      pop_back();
      return end();
    }
    node *p = pos.p;
    int ind = pos.pos;
    while (p != pos.que -> tail) {
      for (int i = ind; i < p->size - 1; ++i)
        p->val[i] = p->val[i + 1];
      if (p->nxt->size) p->val[p->size - 1] = p->nxt->val[0];
      p = p->nxt;
      ind = 0;
    }
    for (int i = ind; i < p->size - 1; ++i)
      p->val[i] = p->val[i + 1];
    pop_back();
    return pos;
  }
};

}

#endif
