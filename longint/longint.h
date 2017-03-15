#ifndef __LONGINT_H
#define __LONGINT_H
#include <vector>
#include <cstring>
#include <cstdio>
#include <string>

class longint: protected std::vector<unsigned>
{
	friend struct __get_num;
	friend std::ostream& operator << (std::ostream &os, const longint &n)
	{ return os << n.as_string(); }
public:
	longint(int x = 0)		// default constructor
	{
		set_filter(x >= 0);		// ensure the sign is right
		push_back(x);
	}
	longint(long long x)
	{
		set_filter(x >= 0);
		push_back(x);
		if (unsigned(x >>= 32) != filter)
		{ push_back(x); }
	}
	longint(const char* src);
	/* use the default copy/move constructor for vector<unsigned> */
	
	explicit operator bool() const { return size() > 1 || front(); }
	int as_int() const { return front(); } 
	std::string as_string () const;
	
	// unary operators
	bool operator ! () const { return size() == 1 && !front(); }
	longint operator - () const		// -x = ~x + 1
	{
		longint result = ~*this;
		result += 1;
		return result;
	}
	longint operator ~ () const		// filp each bit and filter
	{
		longint result = *this;
		for (auto &var : result)
			{ var = ~var; }
		result.filter = ~result.filter;
		return result;
	}
	// TODO: improve these operators
	longint& operator ++ () { return *this += 1; }
	longint operator ++ (int)
	{ longint t(*this); *this += 1; return t; }
	longint& operator -- () { return *this -= 1; }
	longint operator -- (int)
	{ longint t(*this); *this -= 1; return t; }
	
	//inline longint &operator = (const longint& n);
	
	longint operator + (const longint& n) const
	{ longint result(*this); return result += n; }
	longint operator - (const longint& n) const
	{ longint result(*this); return result -= n; }
	longint operator >> (int n) const
	{ longint result(*this); return result >>= n; }
	longint operator << (int n) const
	{ longint result(*this); return result <<= n; }
	longint operator | (const longint& n) const
	{ longint result(*this); return result |= n; }
	longint operator & (const longint& n) const
	{ longint result(*this); return result &= n; }
	longint operator ^ (const longint& n) const
	{ longint result(*this); return result ^= n; }
	longint operator * (int n) const
	{ longint result(*this); return result *= n; }
	longint operator * (unsigned n) const
	{ longint result(*this); return result *= n; }
	friend longint operator * (int n, const longint& m)
	{ longint result(m); return result *= n; }
	longint operator * (const longint& n) const
	{ longint result(*this); return result *= n; }
	longint operator / (int n) const
	{ longint result(*this); return result /= n; }
	inline longint operator / (const longint& n) const;
	
	longint operator % (int n) const
	{ longint result(*this); return result %= n; }
	longint operator % (const longint& n) const
	{ longint result(*this); return result %= n; }
	
	bool operator == (const longint& n) const
	{ return size() == n.size() && n.filter == filter 
		&& std::equal(begin(), end(), n.begin()); }
	bool operator != (const longint& n) const
	{ return size() != n.size() || n.filter != filter
		|| !std::equal(begin(), end(), n.begin()); }
	bool operator > (const longint& n) const
	{ return (n - *this).filter; }
	bool operator < (const longint& n) const
	{ return (*this - n).filter; }
	bool operator >= (const longint& n) const
	{ return !(*this < n); }
	bool operator <= (const longint& n) const
	{ return !(*this > n); }
	
	longint& operator += (const longint& n);
	longint& operator -= (const longint& n);
	longint& operator >>= (int n);
	longint& operator <<= (int n);
	longint& operator |= (const longint& n);
	longint& operator &= (const longint& n);
	longint& operator ^= (const longint& n);
	longint& operator *= (int n);
	longint& operator *= (unsigned n);
	longint& operator *= (const longint& n);
	longint& operator /= (int n);
	longint& operator /= (const longint& n)
	{ return *this = *this / n; }
	longint& operator %= (int n)
	{ return (*this) -= (*this) / n * n; }
	longint& operator %= (const longint& n)
	{ return (*this) -= (*this) / n * n; }
protected:
	void pop_top_zeroes()
	{ while (back() == filter && size() > 1) { pop_back(); } }
	void safe_resize(int n)
	{ resize(n, filter); }
	void safe_push_back(int top)
	{ if (top != filter)
		{ push_back(top); set_filter(top >= 0); } }
	void set_filter(bool positive)
	{ filter = positive ? 0 : unsigned(-1); }
	unsigned filter = 0;
	static const unsigned src_deccount = 9;
	static const unsigned src_max = 1000000000;		// 30 bit
	//static const unsigned max_val = unsigned(-1);
};

struct __get_num
{
	bool operator ()(unsigned &dest, const char *src)
	{
		if (!src)
		{
			if ((src = save_ptr) < end_ptr)
				{ return false; }
		}
		else
		{
			end_ptr = src;
			src += strlen(src);
		}
		src -= longint::src_deccount;
		if (src >= end_ptr) {
			if (sscanf(src, "%9u", &dest) <= 0) 
				{ return false; }
		}
		else
		{
			char buff[longint::src_deccount + 1] = {};
			strncpy(buff, end_ptr, longint::src_deccount - (end_ptr - src));
			if (sscanf(buff, "%u", &dest) <= 0)
				{ return false; }
		}
		save_ptr = src;
		return true;
	}
private:
	const char *save_ptr;
	const char *end_ptr;
	//const unsigned max = unsigned(-1); 
} get_num;

#include "longint.cpp"
#endif
