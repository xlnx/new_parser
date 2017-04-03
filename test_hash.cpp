#include <iostream>

inline constexpr long long str_hash_64(const char *str)
{
	long long seed = 131;
	long long hash = 0;
	while (*str)
		hash = hash * seed + (*str++);
	return hash & ((unsigned long long)(-1LL) >> 1);
}
using namespace std;

int main()
{
	string s;
	while (1) {
		getline(cin, s);
		cout << str_hash_64(s.c_str()) << endl;
	}
}
