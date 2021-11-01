#include <iostream>
#include "MySubset.h"
#include <vector>

using namespace std;

struct A
{
	A(int v) : v(v) {}
	A(A&&) = delete;

	int v;

	friend bool operator==(const A& l, const A& r)
	{
		return l.v == r.v;
	}
	friend bool operator!=(const A & l, const A & r)
	{
		return !(l == r);
	}
};

int main()
{
	bitfield_arithmetic<uint8_t>{};

	return 0;
}