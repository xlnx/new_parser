#include <iostream>
#include "longint.h"
using namespace std;
longint fact(int n)
{
	longint result = 1;
	while (n > 0)
		{ result *= n--; }
	return result;
}

int main()
{
	longint n = -123123;
	n <<= 100;
	//n /= n * 2;
	cout << n << endl;
}
