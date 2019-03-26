#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
	private:
		/* May need modify to satisfy sqrt n */
		/* Attention: one more node for "end" */
		static const int maxBlockLength = 64;
		struct dequeNode {
			dequeNode *nodePrev, *nodeNext;
			T *nodeValue;
			explicit dequeNode(dequeNode *_nodePrev = nullptr, dequeNode *_nodeNext = nullptr) {
				nodeValue = (T*) malloc (sizeof(T));
				nodePrev = _nodePrev; nodeNext = _nodeNext;
			}
			explicit dequeNode(T _nodeValue, dequeNode *_nodePrev = nullptr, dequeNode *_nodeNext = nullptr) {
				nodeValue = (T*) malloc (sizeof(T));
				*nodeValue = _nodeValue;
				nodePrev = _nodePrev; nodeNext = _nodeNext;
			}
			~dequeNode() {
				if(nodeValue) free(nodeValue);
			}
		};
		struct dequeBlock {
			dequeNode *headOfBlock, *tailOfBlock;
			dequeBlock *blockPrev, *blockNext;
			size_t blockLength;
			bool blockStillUsed;
			explicit dequeBlock(dequeNode *_headOfBlock = nullptr, dequeNode *_tailOfBlock = nullptr,
							   dequeBlock *_blockPrev = nullptr, dequeBlock *_blockNext = nullptr) {
				headOfBlock = _headOfBlock; tailOfBlock = _tailOfBlock;
				blockPrev = _blockPrev; blockNext = _blockNext;
				blockLength = 0;
				blockStillUsed = 0;
			}
			~dequeBlock() {
				if(blockStillUsed == 0) {
					dequeNode *p = headOfBlock, *q;
					while (p != nullptr) {
						q = p->nodeNext;
						delete p;
						p = q;
					}
				}
			}
		};
		dequeBlock *dequeHead, *dequeTail;
		dequeNode *dequeBegin, *dequeEnd;
		size_t dequeLength;

		void copyDequeBlock(dequeBlock *copyTo, dequeBlock *copyFrom) {
			copyTo -> headOfBlock = new dequeNode(*(copyFrom -> headOfBlock -> nodeValue));
			dequeNode *p = copyFrom -> headOfBlock -> nodeNext, *q = copyTo -> headOfBlock;
			while(p != nullptr) {
				q -> nodeNext = new dequeNode(*(p -> nodeValue), q, nullptr);
				q = q -> nodeNext;
				p = p -> nodeNext;
			}
			copyTo -> tailOfBlock = q;
			copyTo -> blockLength = copyFrom -> blockLength;
		}

		void splitBlock(dequeBlock *opBlock) {
			dequeNode *p = opBlock -> headOfBlock, *q;
			for (int i=1; i<maxBlockLength; ++i) p = p -> nodeNext;
			q = p -> nodeNext;
			// split between p and q ( which represents p -> nodeNext )
			dequeBlock *newBlock = new dequeBlock(q, opBlock -> tailOfBlock, opBlock, opBlock -> blockNext);
			if(opBlock -> blockNext != nullptr) opBlock -> blockNext -> blockPrev = newBlock;
			opBlock -> blockNext = newBlock;
			opBlock -> tailOfBlock = p;
			q -> nodePrev = nullptr;
			p -> nodeNext = nullptr;
			newBlock -> blockLength = opBlock -> blockLength - maxBlockLength;
			opBlock -> blockLength = maxBlockLength;
		}

		bool checkMerge(dequeBlock *blockLeft, dequeBlock *blockRight) {
			if(blockLeft == nullptr || blockRight == nullptr) return false;
			if(blockLeft == blockRight) throw "not gonna happen!";
			return (blockLeft -> blockLength + blockRight -> blockLength <= maxBlockLength);
		}

		void mergeLeft(dequeBlock *blockTo, dequeBlock *blockFrom) {
			blockFrom -> blockStillUsed = 1;
			blockTo -> tailOfBlock -> nodeNext = blockFrom -> headOfBlock;
			blockFrom -> headOfBlock -> nodePrev = blockTo -> tailOfBlock;
			blockTo -> tailOfBlock = blockFrom -> tailOfBlock;
			blockTo -> blockLength = blockTo -> blockLength + blockFrom -> blockLength;
			delete blockFrom;
		}

		void mergeRight(dequeBlock *blockFrom, dequeBlock *blockTo) {
			blockFrom -> blockStillUsed = 1;
			blockTo -> headOfBlock -> nodePrev = blockFrom -> tailOfBlock;
			blockFrom -> tailOfBlock -> nodeNext = blockTo -> headOfBlock;
			blockTo -> headOfBlock = blockFrom -> headOfBlock;
			blockTo -> blockLength = blockTo -> blockLength + blockFrom -> blockLength;
			delete blockFrom;
		}

	public:
		class const_iterator;
		class iterator {
			private:
				deque *belongDeque;
				dequeBlock *belongBlock;
				dequeNode *belongNode;
			public:
				/**
				 * return a new iterator which pointer n-next elements
				 *   even if there are not enough elements, the behaviour is **undefined**.
				 * as well as operator-
				 */
				iterator operator+(const int &n) const {
					if(n == 0) return *this;
					int restOfDelta = n;
					if(n > 0) {
						//TODO
					} else {
						restOfDelta = -restOfDelta;
						//TODO
					}
				}
				iterator operator-(const int &n) const { return *this + (-n); }
				// return th distance between two iterator,
				// if these two iterators points to different vectors, throw invaild_iterator.
				int operator-(const iterator &rhs) const {
					//TODO
				}
				iterator operator+=(const int &n) { *this = *this + n; return *this; }
				iterator operator-=(const int &n) { *this = *this - n; return *this; }
				iterator operator++(int x) { iterator tempIt = *this; *this += 1; return tempIt; }
				iterator& operator++() { *this += 1; return *this; }
				iterator operator--(int x) { iterator tempIt = *this; *this -= 1; return tempIt; }
				iterator& operator--() { *this -= 1; return *this; }
				T& operator*() const { return *(belongNode -> nodeValue); }
				T* operator->() const noexcept { return belongNode -> nodeValue; }
				bool operator==(const iterator &rhs) const { return belongDeque == rhs.belongDeque && belongBlock == rhs.belongBlock && belongNode == rhs.belongNode; }
				bool operator==(const const_iterator &rhs) const { return belongDeque == rhs.belongDeque && belongBlock == rhs.belongBlock && belongNode == rhs.belongNode; }
				bool operator!=(const iterator &rhs) const { return belongDeque != rhs.belongDeque || belongBlock != rhs.belongBlock || belongNode != rhs.belongNode; }
				bool operator!=(const const_iterator &rhs) const { return belongDeque != rhs.belongDeque || belongBlock != rhs.belongBlock || belongNode != rhs.belongNode; }
		};
		class const_iterator {
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
			private:
				// data members.
			public:
				const_iterator() {
					// TODO
				}
				const_iterator(const const_iterator &other) {
					// TODO
				}
				const_iterator(const iterator &other) {
					// TODO
				}
				// And other methods in iterator.
				// And other methods in iterator.
				// And other methods in iterator.
		};

		deque() {
			dequeHead = dequeTail = new dequeBlock;
			dequeHead -> headOfBlock = dequeHead -> tailOfBlock = new dequeNode;
			dequeBegin = dequeEnd = dequeHead -> headOfBlock;
			dequeHead -> blockLength = 1;
			dequeLength = 1;
		}

		deque(const deque &other) {
			dequeHead = new dequeBlock;
			copyDequeBlock(dequeHead, other.dequeHead);
			dequeBlock *p = other.dequeHead -> blockNext, *q = dequeHead;
			while(p != nullptr) {
				q -> blockNext = new dequeBlock(nullptr, nullptr, q, nullptr);
				q = q -> blockNext;
				copyDequeBlock(q, p);
				p = p -> blockNext;
			}
			dequeTail = q;
			dequeBegin = dequeHead -> headOfBlock;
			dequeEnd = dequeTail -> tailOfBlock;
			dequeLength = other.dequeLength;
		}

		~deque() {
			dequeBlock *p = dequeHead, *q;
			while(p != nullptr) {
				q = p -> blockNext;
				delete p;
				p = q;
			}
		}

		deque &operator=(const deque &other) {
			/* A combination of ~deque() and deque(const deque &other)) */
			dequeBlock *p = dequeHead, *q;
			while(p != nullptr) {
				q = p -> blockNext;
				delete p;
				p = q;
			}
			dequeHead = new dequeBlock;
			copyDequeBlock(dequeHead, other.dequeHead);
			p = other.dequeHead -> blockNext, q = dequeHead;
			while(p != nullptr) {
				q -> blockNext = new dequeBlock(nullptr, nullptr, q, nullptr);
				q = q -> blockNext;
				copyDequeBlock(q, p);
				p = p -> blockNext;
			}
			dequeTail = q;
			dequeBegin = dequeHead -> headOfBlock;
			dequeEnd = dequeTail -> tailOfBlock;
			dequeLength = other.dequeLength;
		}

		bool empty() const { return dequeLength == 1; }
		size_t size() const { return dequeLength - 1; }

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

		void clear() {
			/* A combination of ~deque() and deque() */
			dequeBlock *p = dequeHead, *q;
			while(p != nullptr) {
				q = p -> blockNext;
				delete p;
				p = q;
			}
			dequeHead = dequeTail = new dequeBlock;
			dequeHead -> headOfBlock = dequeHead -> tailOfBlock = new dequeNode;
			dequeBegin = dequeEnd = dequeHead -> headOfBlock;
			dequeHead -> blockLength = 1;
			dequeLength = 1;
		}
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
		void push_back(const T &value) {
			*(dequeEnd -> nodeValue) = value;
			dequeEnd -> nodeNext = new dequeNode(dequeEnd, nullptr);
			dequeEnd = dequeEnd -> nodeNext;
			++ dequeTail -> blockLength;
			++ dequeLength;
			if(dequeTail -> blockLength >= 2 * maxBlockLength) {
				splitBlock(dequeTail);
				dequeTail = dequeTail -> blockNext;
			}
		}
		/**
		 * removes the last element
		 *     throw when the container is empty.
		 */
		void pop_back() {
			if(empty()) throw container_is_empty();
			-- dequeLength;
			if(dequeEnd -> nodePrev != nullptr) {
				dequeEnd = dequeEnd -> nodePrev;
				dequeTail -> tailOfBlock = dequeEnd;
				delete dequeEnd -> nodeNext;
				-- dequeTail;
				while(checkMerge(dequeTail -> blockPrev, dequeTail)) mergeRight(dequeTail -> blockPrev, dequeTail);
			} else {
				dequeTail = dequeTail -> blockPrev;
				delete dequeTail -> blockNext;
				dequeEnd = dequeTail -> tailOfBlock;
				while(checkMerge(dequeTail -> blockPrev, dequeTail)) mergeRight(dequeTail -> blockPrev, dequeTail);
			}
		}
		/**
		 * inserts an element to the beginning.
		 */
		void push_front(const T &value) {
			dequeBegin -> nodePrev = new dequeNode(value, nullptr, dequeBegin);
			dequeBegin = dequeBegin -> nodePrev;
			++ dequeHead -> blockLength;
			++ dequeLength;
			if(dequeHead -> blockLength >= 2 * maxBlockLength) splitBlock(dequeHead);
		}
		/**
		 * removes the first element.
		 *     throw when the container is empty.
		 */
		void pop_front() {
			if(empty()) throw container_is_empty();
			-- dequeLength;
			if(dequeBegin -> nodeNext != nullptr) {
				dequeBegin = dequeBegin -> nodeNext;
				dequeHead -> headOfBlock = dequeBegin;
				delete dequeBegin -> nodePrev;
				-- dequeHead;
				while(checkMerge(dequeHead, dequeHead -> blockNext)) mergeLeft(dequeHead, dequeHead -> blockNext);
			} else {
				dequeHead = dequeHead -> blockNext;
				delete dequeHead -> blockPrev;
				dequeBegin = dequeHead -> headOfBlock;
				while(checkMerge(dequeHead, dequeHead -> blockNext)) mergeLeft(dequeHead, dequeHead -> blockNext);
			}
		}
};

}

#endif
