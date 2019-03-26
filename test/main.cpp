//Provided by desire2020/Steve Lu

/***********************************************************************
Hint: This test case almost completely tests the correctness of your deque.
So if all tests are passed, feel free to enhance your performance! :)
Yours Sincerely. Rabbit.
***********************************************************************/
#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "class-bint.hpp"
#include <iostream>
#include <vector>
#include <deque>
#include "deque.hpp"


long long randNum(long long x,long long maxNum)
{
	x = (x * 10007) % maxNum;
	return x + 1;
}
const size_t N = 10005LL;

void error()
{
	std::cout << "Error, mismatch found." << std::endl;
	exit(0);
}

int main() {
	sjtu :: deque<int> deq;
	std :: deque<int> ans;
	for (int i=1; i<=10000; ++i) {
		deq.push_back(i);
		std :: cout << "push " << i << " success!\n";
//		ans.push_back(i);
	}
	for (int i=1; i<=10000; ++i) {
		deq.pop_back();
		std :: cout << "pop " << i << " success!\n";
//		ans.pop_back();
	}

	std :: cout << "Success!\n";
	return 0;
}