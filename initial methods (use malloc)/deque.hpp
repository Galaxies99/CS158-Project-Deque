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

		struct dequeNode;
		struct dequeBlock;

		dequeNode* allocateDequeNode(dequeNode *_nodePrev = nullptr, dequeNode *_nodeNext = nullptr) {
//			dequeNode *newNode = (dequeNode*) malloc (sizeof(dequeNode));
//			newNode -> nodeValue = (T*) malloc (sizeof(T));
//			newNode -> nodePrev = _nodePrev;
//			newNode -> nodeNext = _nodeNext;
			dequeNode *newNode = new dequeNode(_nodePrev, _nodeNext);
			return newNode;
		}

		dequeNode* allocateDequeNode(const T& _nodeValue, dequeNode *_nodePrev = nullptr, dequeNode *_nodeNext = nullptr) {
//			dequeNode *newNode = (dequeNode*) malloc (sizeof(dequeNode));
//			newNode -> nodeValue = (T*) malloc (sizeof(T));
//			*(newNode -> nodeValue) = _nodeValue;
//			newNode -> nodePrev = _nodePrev;
//			newNode -> nodeNext = _nodeNext;
			dequeNode *newNode = new dequeNode(_nodeValue, _nodePrev, _nodeNext);
			return newNode;
		}

		dequeBlock* allocateDequeBlock(dequeNode *_headOfBlock = nullptr, dequeNode *_tailOfBlock = nullptr, dequeBlock *_blockPrev = nullptr, dequeBlock *_blockNext = nullptr) {
//			dequeBlock *newBlock = (dequeBlock*) malloc (sizeof(dequeBlock));
//			newBlock -> headOfBlock = _headOfBlock;
//			newBlock -> tailOfBlock = _tailOfBlock;
//			newBlock -> blockPrev = _blockPrev;
//			newBlock -> blockNext = _blockNext;
			dequeBlock *newBlock = new dequeBlock(_headOfBlock, _tailOfBlock, _blockPrev, _blockNext);
			return newBlock;
		}

		void deallocateDequeNode(dequeNode *itNode) {
//			if(itNode -> nodeValue) free(itNode -> nodeValue);
//			if(itNode != nullptr) free(itNode), itNode = nullptr;
			free(itNode -> nodeValue);
			delete itNode;
		}

		void deallocateDequeBlock(dequeBlock *itBlock) {
			if(itBlock -> blockStillUsed == 0) {
				dequeNode *p = itBlock -> headOfBlock, *q;
				while (p != nullptr) {
					q = p -> nodeNext;
					deallocateDequeNode(p);
					p = q;
				}
			}
//			if(itBlock != nullptr) free(itBlock), itBlock = nullptr;
			delete itBlock;
		}

		struct dequeNode {
			dequeNode *nodePrev, *nodeNext;
			T *nodeValue;
			explicit dequeNode(dequeNode *_nodePrev = nullptr, dequeNode *_nodeNext = nullptr) {
				// nodeValue = new T;
				nodeValue = (T*) malloc (sizeof(T));
				nodePrev = _nodePrev; nodeNext = _nodeNext;
			}
			explicit dequeNode(const T &_nodeValue, dequeNode *_nodePrev = nullptr, dequeNode *_nodeNext = nullptr) {
				// nodeValue = new T(_nodeValue);
				nodeValue = (T*) malloc (sizeof(T));
				*nodeValue = _nodeValue;
				nodePrev = _nodePrev; nodeNext = _nodeNext;
			}
			~dequeNode() {}
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
			~dequeBlock() {}
		};
		dequeBlock *dequeHead, *dequeTail;
		dequeNode *dequeBegin, *dequeEnd;
		size_t dequeLength;

		void copyDequeBlock(dequeBlock *copyTo, dequeBlock *copyFrom, const deque &dequeFrom) {
			if(copyFrom -> headOfBlock != dequeFrom.dequeEnd)
				copyTo -> headOfBlock = allocateDequeNode(*(copyFrom -> headOfBlock -> nodeValue));
			else copyTo -> headOfBlock = allocateDequeNode();
			dequeNode *p = copyFrom -> headOfBlock -> nodeNext, *q = copyTo -> headOfBlock;
			while(p != nullptr) {
				if(p != dequeFrom.dequeEnd)
					q -> nodeNext = allocateDequeNode(*(p -> nodeValue), q, nullptr);
				else q -> nodeNext = allocateDequeNode(q, nullptr);
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
			dequeBlock *newBlock = allocateDequeBlock(q, opBlock -> tailOfBlock, opBlock, opBlock -> blockNext);
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
			blockTo -> blockNext = blockFrom -> blockNext;
			if(blockFrom -> blockNext != nullptr) blockFrom -> blockNext -> blockPrev = blockTo;
			if(dequeTail == blockFrom) dequeTail = blockTo;
			deallocateDequeBlock(blockFrom);
		}

		void mergeRight(dequeBlock *blockFrom, dequeBlock *blockTo) {
			blockFrom -> blockStillUsed = 1;
			blockTo -> headOfBlock -> nodePrev = blockFrom -> tailOfBlock;
			blockFrom -> tailOfBlock -> nodeNext = blockTo -> headOfBlock;
			blockTo -> headOfBlock = blockFrom -> headOfBlock;
			blockTo -> blockLength = blockTo -> blockLength + blockFrom -> blockLength;
			blockTo -> blockPrev = blockFrom -> blockPrev;
			if(blockFrom -> blockPrev != nullptr) blockFrom -> blockPrev -> blockNext = blockTo;
			if(dequeHead == blockFrom) dequeHead = blockTo;
			deallocateDequeBlock(blockFrom);
		}

	public:
		class const_iterator;
		class iterator {
				friend class deque;
			private:
				deque *belongDeque;
				dequeBlock *belongBlock;
				dequeNode *belongNode;
				int getPosition() const {
					int res = 0;
					dequeBlock *itBlock = belongBlock;
					dequeNode *itNode = belongNode -> nodePrev;
					while(itNode != nullptr) {
						res ++;
						itNode = itNode -> nodePrev;
					}
					if(itBlock -> blockPrev == nullptr) return res;
					else itBlock = itBlock -> blockPrev;
					while(itBlock != nullptr) {
						res += itBlock -> blockLength;
						itBlock = itBlock -> blockPrev;
					}
					return res;
				}
			public:
				iterator() { belongDeque = nullptr; belongBlock = nullptr; belongNode = nullptr; }
				iterator(deque *_belongDeque, dequeBlock *_belongBlock, dequeNode *_belongNode) {
					belongDeque = _belongDeque;
					belongBlock = _belongBlock;
					belongNode = _belongNode;
				}
				iterator(const iterator &other) {
					belongDeque = other.belongDeque;
					belongBlock = other.belongBlock;
					belongNode = other.belongNode;
				}
				iterator(const const_iterator &other) {
					belongDeque = other.belongDeque;
					belongBlock = other.belongBlock;
					belongNode = other.belongNode;
				}
				iterator operator+(const int &n) const {
					if(n == 0) return *this;
					int restOfDelta = n;
					if(n > 0) {
						dequeBlock *itBlock = this -> belongBlock;
						dequeNode *itNode = this -> belongNode -> nodeNext;
						-- restOfDelta;
						while(itNode != nullptr) {
							if(restOfDelta == 0) return iterator(this -> belongDeque, itBlock, itNode);
							itNode = itNode -> nodeNext;
							-- restOfDelta;
						}
						itBlock = itBlock -> blockNext;
						if(itBlock == nullptr) return iterator(nullptr, nullptr, nullptr);
						else itNode = itBlock -> headOfBlock;
						while(restOfDelta >= itBlock -> blockLength) {
							restOfDelta -= itBlock -> blockLength;
							itBlock = itBlock -> blockNext;
							if(itBlock == nullptr) return iterator(nullptr, nullptr, nullptr);
							else itNode = itBlock -> headOfBlock;
						}
						while(restOfDelta --) itNode = itNode -> nodeNext;
						return iterator(this -> belongDeque, itBlock, itNode);
					} else {
						restOfDelta = -restOfDelta;
						dequeBlock *itBlock = this -> belongBlock;
						dequeNode *itNode = this -> belongNode -> nodePrev;
						-- restOfDelta;
						while(itNode != nullptr) {
							if(restOfDelta == 0) return iterator(this -> belongDeque, itBlock, itNode);
							itNode = itNode -> nodePrev;
							-- restOfDelta;
						}
						itBlock = itBlock -> blockPrev;
						if(itBlock == nullptr) return iterator(nullptr, nullptr, nullptr);
						else itNode = itBlock -> tailOfBlock;
						while(restOfDelta >= itBlock -> blockLength) {
							restOfDelta -= itBlock -> blockLength;
							itBlock = itBlock -> blockPrev;
							if(itBlock == nullptr) return iterator(nullptr, nullptr, nullptr);
							else itNode = itBlock -> tailOfBlock;
						}
						while(restOfDelta --) itNode = itNode -> nodePrev;
						return iterator(this -> belongDeque, itBlock, itNode);
					}
				}
				iterator operator-(const int &n) const { return *this + (-n); }
				// return th distance between two iterator,
				// if these two iterators points to different vectors, throw invaild_iterator.
				int operator-(const iterator &rhs) const {
					if(belongDeque != rhs.belongDeque) throw invalid_iterator();
					return this -> getPosition() - rhs.getPosition();
				}
				iterator operator+=(const int &n) { *this = *this + n; return *this; }
				iterator operator-=(const int &n) { *this = *this - n; return *this; }
				iterator operator++(int x) { iterator tempIt = *this; *this += 1; return tempIt; }
				iterator& operator++() { *this += 1; return *this; }
				iterator operator--(int x) { iterator tempIt = *this; *this -= 1; return tempIt; }
				iterator& operator--() { *this -= 1; return *this; }
				T& operator*() const {
					if(belongDeque == nullptr || belongBlock == nullptr || belongBlock == nullptr) throw invalid_iterator();
					if(belongNode == this -> belongDeque -> dequeEnd) throw invalid_iterator();
					return *(belongNode -> nodeValue);
				}
				T* operator->() const noexcept {
					if(belongDeque == nullptr || belongBlock == nullptr || belongBlock == nullptr) throw invalid_iterator();
					if(belongNode == this -> belongDeque -> dequeEnd) throw invalid_iterator();
					return belongNode -> nodeValue;
				}
				bool operator==(const iterator &rhs) const { return belongDeque == rhs.belongDeque && belongBlock == rhs.belongBlock && belongNode == rhs.belongNode; }
				bool operator==(const const_iterator &rhs) const { return belongDeque == rhs.belongDeque && belongBlock == rhs.belongBlock && belongNode == rhs.belongNode; }
				bool operator!=(const iterator &rhs) const { return belongDeque != rhs.belongDeque || belongBlock != rhs.belongBlock || belongNode != rhs.belongNode; }
				bool operator!=(const const_iterator &rhs) const { return belongDeque != rhs.belongDeque || belongBlock != rhs.belongBlock || belongNode != rhs.belongNode; }
		};
		class const_iterator {
				friend class deque;
			private:
				const deque *belongDeque;
				const dequeBlock *belongBlock;
				const dequeNode *belongNode;
				int getPosition() const {
					int res = 0;
					const dequeBlock *itBlock = belongBlock;
					const dequeNode *itNode = belongNode -> nodePrev;
					while(itNode != nullptr) {
						res ++;
						itNode = itNode -> nodePrev;
					}
					if(itBlock -> blockPrev == nullptr) return res;
					else itBlock = itBlock -> blockPrev;
					while(itBlock != nullptr) {
						res += itBlock -> blockLength;
						itBlock = itBlock -> blockPrev;
					}
					return res;
				}
			public:
				const_iterator() { belongDeque = nullptr; belongBlock = nullptr; belongNode = nullptr; }
				const_iterator(const deque *_belongDeque, const dequeBlock *_belongBlock, const dequeNode *_belongNode) {
					belongDeque = _belongDeque;
					belongBlock = _belongBlock;
					belongNode = _belongNode;
				}
				const_iterator(const iterator &other) {
					belongDeque = other.belongDeque;
					belongBlock = other.belongBlock;
					belongNode = other.belongNode;
				}
				const_iterator(const const_iterator &other) {
					belongDeque = other.belongDeque;
					belongBlock = other.belongBlock;
					belongNode = other.belongNode;
				}
				const_iterator operator+(const int &n) const {
					if(n == 0) return const_iterator(*this);
					int restOfDelta = n;
					if(n > 0) {
						const dequeBlock *itBlock = this -> belongBlock;
						const dequeNode *itNode = this -> belongNode -> nodeNext;
						-- restOfDelta;
						while(itNode != nullptr) {
							if(restOfDelta == 0) return const_iterator(this -> belongDeque, itBlock, itNode);
							itNode = itNode -> nodeNext;
							-- restOfDelta;
						}
						itBlock = itBlock -> blockNext;
						if(itBlock == nullptr) return const_iterator(nullptr, nullptr, nullptr);
						else itNode = itBlock -> headOfBlock;
						while(restOfDelta >= itBlock -> blockLength) {
							restOfDelta -= itBlock -> blockLength;
							itBlock = itBlock -> blockNext;
							if(itBlock == nullptr) return const_iterator(nullptr, nullptr, nullptr);
							else itNode = itBlock -> headOfBlock;
						}
						while(restOfDelta --) itNode = itNode -> nodeNext;
						return const_iterator(this -> belongDeque, itBlock, itNode);
					} else {
						restOfDelta = -restOfDelta;
						const dequeBlock *itBlock = this -> belongBlock;
						const dequeNode *itNode = this -> belongNode -> nodePrev;
						-- restOfDelta;
						while(itNode != nullptr) {
							if(restOfDelta == 0) return const_iterator(this -> belongDeque, itBlock, itNode);
							itNode = itNode -> nodePrev;
							-- restOfDelta;
						}
						itBlock = itBlock -> blockPrev;
						if(itBlock == nullptr) return const_iterator(nullptr, nullptr, nullptr);
						else itNode = itBlock -> tailOfBlock;
						while(restOfDelta >= itBlock -> blockLength) {
							restOfDelta -= itBlock -> blockLength;
							itBlock = itBlock -> blockPrev;
							if(itBlock == nullptr) return const_iterator(nullptr, nullptr, nullptr);
							else itNode = itBlock -> tailOfBlock;
						}
						while(restOfDelta --) itNode = itNode -> nodePrev;
						return const_iterator(this -> belongDeque, itBlock, itNode);
					}
				}
	 			const_iterator operator-(const int &n) const { return *this + (-n); }
				int operator-(const const_iterator &rhs) const {
					if(belongDeque != rhs.belongDeque) throw invalid_iterator();
					return this -> getPosition() - rhs.getPosition();
				}
				const_iterator operator+=(const int &n) { *this = *this + n; return *this; }
				const_iterator operator-=(const int &n) { *this = *this - n; return *this; }
				const_iterator operator++(int x) { iterator tempIt = *this; *this += 1; return tempIt; }
				const_iterator& operator++() { *this += 1; return *this; }
				const_iterator operator--(int x) { iterator tempIt = *this; *this -= 1; return tempIt; }
				const_iterator& operator--() { *this -= 1; return *this; }
				T& operator*() const {
					if(belongDeque == nullptr || belongBlock == nullptr || belongBlock == nullptr) throw invalid_iterator();
					if(belongNode == this -> belongDeque -> dequeEnd) throw invalid_iterator();
					return *(belongNode -> nodeValue);
				}
				T* operator->() const noexcept {
					if(belongDeque == nullptr || belongBlock == nullptr || belongBlock == nullptr) throw invalid_iterator();
					if(belongNode == this -> belongDeque -> dequeEnd) throw invalid_iterator();
					return belongNode -> nodeValue;
				}
				bool operator==(const iterator &rhs) const { return belongDeque == rhs.belongDeque && belongBlock == rhs.belongBlock && belongNode == rhs.belongNode; }
				bool operator==(const const_iterator &rhs) const { return belongDeque == rhs.belongDeque && belongBlock == rhs.belongBlock && belongNode == rhs.belongNode; }
				bool operator!=(const iterator &rhs) const { return belongDeque != rhs.belongDeque || belongBlock != rhs.belongBlock || belongNode != rhs.belongNode; }
				bool operator!=(const const_iterator &rhs) const { return belongDeque != rhs.belongDeque || belongBlock != rhs.belongBlock || belongNode != rhs.belongNode; }
		};

		deque() {
			dequeHead = dequeTail = allocateDequeBlock();
			dequeHead -> headOfBlock = dequeHead -> tailOfBlock = allocateDequeNode();
			dequeBegin = dequeEnd = dequeHead -> headOfBlock;
			dequeHead -> blockLength = 1;
			dequeLength = 1;
		}

		deque(const deque &other) {
			dequeHead = allocateDequeBlock();
			copyDequeBlock(dequeHead, other.dequeHead, other);
			dequeBlock *p = other.dequeHead -> blockNext, *q = dequeHead;
			while(p != nullptr) {
				q -> blockNext = allocateDequeBlock(nullptr, nullptr, q, nullptr);
				q = q -> blockNext;
				copyDequeBlock(q, p, other);
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
				deallocateDequeBlock(p);
				p = q;
			}
			dequeHead = dequeTail = nullptr;
			dequeBegin = dequeEnd = nullptr;
		}

		deque &operator=(const deque &other) {
			if(this == &other) return *this;
			/* A combination of ~deque() and deque(const deque &other)) */
			dequeBlock *p = dequeHead, *q;
			while(p != nullptr) {
				q = p -> blockNext;
				deallocateDequeBlock(p);
				p = q;
			}
			dequeHead = allocateDequeBlock();
			copyDequeBlock(dequeHead, other.dequeHead, other);
			p = other.dequeHead -> blockNext, q = dequeHead;
			while(p != nullptr) {
				q -> blockNext = allocateDequeBlock(nullptr, nullptr, q, nullptr);
				q = q -> blockNext;
				copyDequeBlock(q, p, other);
				p = p -> blockNext;
			}
			dequeTail = q;
			dequeBegin = dequeHead -> headOfBlock;
			dequeEnd = dequeTail -> tailOfBlock;
			dequeLength = other.dequeLength;
		  return *this;
    }

		bool empty() const { return dequeLength == 1; }
		size_t size() const { return dequeLength - 1; }

		T & at(const size_t &pos) {
			if(pos < 0 || pos >= dequeLength - 1) throw index_out_of_bound();
			size_t restOfDelta = pos;
			dequeBlock *itBlock = dequeHead;
			dequeNode *itNode = dequeBegin;
			while(restOfDelta >= itBlock -> blockLength) {
				restOfDelta -= itBlock -> blockLength;
				itBlock = itBlock -> blockNext;
				itNode = itBlock -> headOfBlock;
			}
			while(restOfDelta --) itNode = itNode -> nodeNext;
			return *(itNode -> nodeValue);
		}
		const T & at(const size_t &pos) const {
			if(pos < 0 || pos >= dequeLength - 1) throw index_out_of_bound();
			size_t restOfDelta = pos;
			dequeBlock *itBlock = dequeHead;
			dequeNode *itNode = dequeBegin;
			while(restOfDelta >= itBlock -> blockLength) {
				restOfDelta -= itBlock -> blockLength;
				itBlock = itBlock -> blockNext;
				itNode = itBlock -> headOfBlock;
			}
			while(restOfDelta --) itNode = itNode -> nodeNext;
			return *(itNode -> nodeValue);
		}
		T & operator[](const size_t &pos) {
			if(pos < 0 || pos >= dequeLength - 1) throw index_out_of_bound();
			size_t restOfDelta = pos;
			dequeBlock *itBlock = dequeHead;
			dequeNode *itNode = dequeBegin;
			while(restOfDelta >= itBlock -> blockLength) {
				restOfDelta -= itBlock -> blockLength;
				itBlock = itBlock -> blockNext;
				itNode = itBlock -> headOfBlock;
			}
			while(restOfDelta --) itNode = itNode -> nodeNext;
			return *(itNode -> nodeValue);
		}
		const T & operator[](const size_t &pos) const {
			if(pos < 0 || pos >= dequeLength - 1) throw index_out_of_bound();
			size_t restOfDelta = pos;
			dequeBlock *itBlock = dequeHead;
			dequeNode *itNode = dequeBegin;
			while(restOfDelta >= itBlock -> blockLength) {
				restOfDelta -= itBlock -> blockLength;
				itBlock = itBlock -> blockNext;
				itNode = itBlock -> headOfBlock;
			}
			while(restOfDelta --) itNode = itNode -> nodeNext;
			return *(itNode -> nodeValue);
		}
		/**
		 * access the first element
		 * throw container_is_empty when the container is empty.
		 */
		const T & front() const {
			if(empty()) throw container_is_empty();
			return *(dequeBegin -> nodeValue);
		}
		/**
		 * access the last element
		 * throw container_is_empty when the container is empty.
		 */
		const T & back() const {
			if(empty()) throw container_is_empty();
			if(dequeTail -> blockLength > 1) return *(dequeEnd -> nodePrev -> nodeValue);
			else return *(dequeTail -> blockPrev -> tailOfBlock -> nodeValue);
		}
		/**
		 * returns an iterator to the beginning.
		 */
		iterator begin() {
			return iterator(this, dequeHead, dequeBegin);
		}
		const_iterator cbegin() const {
			return const_iterator(this, dequeHead, dequeBegin);
		}
		/**
		 * returns an iterator to the end.
		 */
		iterator end() {
			return iterator(this, dequeTail, dequeEnd);
		}
		const_iterator cend() const {
			return const_iterator(this, dequeTail, dequeEnd);
		}

		void clear() {
			/* A combination of ~deque() and deque() */
			dequeBlock *p = dequeHead, *q;
			while(p != nullptr) {
				q = p -> blockNext;
				deallocateDequeBlock(p);
				p = q;
			}
			dequeHead = dequeTail = allocateDequeBlock();
			dequeHead -> headOfBlock = dequeHead -> tailOfBlock = allocateDequeNode();
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
		iterator insert(iterator pos, const T &value) {
			if(pos.belongDeque != this) throw invalid_iterator();
			if(pos.belongBlock == nullptr || pos.belongNode == nullptr) throw invalid_iterator();
			if(pos == begin()) { push_front(value); return begin(); }
			if(pos == end()) {
				push_back(value);
				iterator it = end();
				return --it;
			}
			iterator tempIt = pos;
			dequeBlock *itBlock = pos.belongBlock;
			dequeNode *itNode = pos.belongNode, *newNode = allocateDequeNode(value, itNode -> nodePrev, itNode);
			if(itNode -> nodePrev != nullptr) itNode -> nodePrev -> nodeNext = newNode;
			else itBlock -> headOfBlock = newNode;
			itNode -> nodePrev = newNode;
			++ itBlock -> blockLength;
			++ dequeLength;
			tempIt.belongNode = newNode;
			if(itBlock -> blockLength >= 2 * maxBlockLength) {
				splitBlock(itBlock);
				if(dequeTail == itBlock) dequeTail = itBlock -> blockNext;
				itNode = itBlock -> blockNext -> headOfBlock;
				tempIt.belongBlock = itBlock;
				while(itNode != nullptr) {
					if(itNode == newNode) {
						tempIt.belongBlock = itBlock -> blockNext;
						break;
					}
					itNode = itNode -> nodeNext;
				}
			}
			return tempIt;
		}
		/**
		 * removes specified element at pos.
		 * removes the element at pos.
		 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
		 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
		 */
		iterator erase(iterator pos) {
			if(empty()) throw container_is_empty();
			if(pos.belongDeque != this) throw invalid_iterator();
			if(pos.belongBlock == nullptr || pos.belongNode == nullptr) throw invalid_iterator();
			if(pos == begin()) { pop_front(); return begin(); }
			iterator tempIt = end();
			if(pos == tempIt) { pop_back(); return end(); }
			tempIt = pos; ++ tempIt;
			dequeBlock *itBlock = pos.belongBlock, *tempBlock;
			dequeNode *itNode = pos.belongNode;
			if(itNode -> nodePrev != nullptr) itNode -> nodePrev -> nodeNext = itNode -> nodeNext;
			else itBlock -> headOfBlock = itNode -> nodeNext;
			if(itNode -> nodeNext != nullptr) itNode -> nodeNext -> nodePrev = itNode -> nodePrev;
			else itBlock -> tailOfBlock = itNode -> nodePrev;
			-- itBlock -> blockLength;
			-- dequeLength;
			if(itBlock -> blockLength == 0) {
				itBlock -> blockPrev -> blockNext = itBlock -> blockNext;
				itBlock -> blockNext -> blockPrev = itBlock -> blockPrev;
				deallocateDequeBlock(itBlock);
				return tempIt;
			}
			while(checkMerge(itBlock, itBlock -> blockNext)) {
				tempBlock = itBlock -> blockNext;
				if(dequeHead == itBlock) dequeHead = tempBlock;
				mergeRight(itBlock, itBlock -> blockNext);
				if(tempIt.belongBlock == itBlock) tempIt.belongBlock = tempBlock;
				itBlock = tempBlock;
			}
			while(checkMerge(itBlock, itBlock -> blockPrev)) {
				tempBlock = itBlock -> blockPrev;
				if(dequeTail == itBlock) dequeTail = tempBlock;
				mergeLeft(itBlock -> blockPrev, itBlock);
				if(tempIt.belongBlock == itBlock) tempIt.belongBlock = tempBlock;
				itBlock = tempBlock;
			}
			return tempIt;
		}

		void push_back(const T &value) {
			*(dequeEnd -> nodeValue) = value;
			dequeEnd -> nodeNext = allocateDequeNode(dequeEnd, nullptr);
			dequeEnd = dequeEnd -> nodeNext;
			dequeTail -> tailOfBlock = dequeEnd;
			++ dequeTail -> blockLength;
			++ dequeLength;
			if(dequeTail -> blockLength >= 2 * maxBlockLength) {
				splitBlock(dequeTail);
				dequeTail = dequeTail -> blockNext;
			}
		}

		void pop_back() {
			if(empty()) throw container_is_empty();
			-- dequeLength;
			if(dequeEnd -> nodePrev != nullptr) {
				dequeEnd = dequeEnd -> nodePrev;
				dequeTail -> tailOfBlock = dequeEnd;
				deallocateDequeNode(dequeEnd -> nodeNext);
				dequeEnd -> nodeNext = nullptr;
				-- dequeTail -> blockLength;
				while(checkMerge(dequeTail -> blockPrev, dequeTail)) mergeRight(dequeTail -> blockPrev, dequeTail);
			} else {
				dequeTail = dequeTail -> blockPrev;
				deallocateDequeBlock(dequeTail -> blockNext);
				dequeTail -> blockNext = nullptr;
				dequeEnd = dequeTail -> tailOfBlock;
				while(checkMerge(dequeTail -> blockPrev, dequeTail)) mergeRight(dequeTail -> blockPrev, dequeTail);
			}
		}

		void push_front(const T &value) {
			dequeBegin -> nodePrev = allocateDequeNode(value, nullptr, dequeBegin);
			dequeBegin = dequeBegin -> nodePrev;
			dequeHead -> headOfBlock = dequeBegin;
			++ dequeHead -> blockLength;
			++ dequeLength;
			if(dequeHead -> blockLength >= 2 * maxBlockLength) {
				splitBlock(dequeHead);
				if(dequeHead == dequeTail) dequeTail = dequeTail -> blockNext;
			}
		}

		void pop_front() {
			if(empty()) throw container_is_empty();
			-- dequeLength;
			if(dequeBegin -> nodeNext != nullptr) {
				dequeBegin = dequeBegin -> nodeNext;
				dequeHead -> headOfBlock = dequeBegin;
				deallocateDequeNode(dequeBegin -> nodePrev);
				dequeBegin -> nodePrev = nullptr;
				-- dequeHead -> blockLength;
				while(checkMerge(dequeHead, dequeHead -> blockNext)) mergeLeft(dequeHead, dequeHead -> blockNext);
			} else {
				dequeHead = dequeHead -> blockNext;
				deallocateDequeBlock(dequeHead -> blockPrev);
				dequeHead -> blockPrev = nullptr;
				dequeBegin = dequeHead -> headOfBlock;
				while(checkMerge(dequeHead, dequeHead -> blockNext)) mergeLeft(dequeHead, dequeHead -> blockNext);
			}
		}
};

}

#endif
