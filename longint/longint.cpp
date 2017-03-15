// constructor

longint::longint(const char* src)
{
	unsigned tmp;
	vector<unsigned> dig_arr;
	if (get_num(tmp, src))
	{
		do { dig_arr.push_back(tmp); }
		while (get_num(tmp, nullptr));
	}
	else
		{ *this = 0; return; }
	while (dig_arr.back() == 0)
		{ dig_arr.pop_back(); }
	unsigned base = 0, top = 0;
	while (!dig_arr.empty())
	{	
		base |= (dig_arr.front() & 1) << top++;
		auto iter = dig_arr.rbegin(), end_iter = dig_arr.rend();
		for (unsigned d = 0; iter < end_iter; ++iter)
		{
			unsigned tmp = d * src_max + *iter;
			*iter = tmp >> 1;
			d = tmp & 1;
		}
		while (!dig_arr.back())
			{ dig_arr.pop_back(); }
		if (top >= 32)
		{
			push_back(base);
			top = base = 0;
		}
	}
	if (empty() || base)
		{ push_back(base); }
}

// operator overload

// double ref

longint& longint::operator += (const longint& n)
{
	int sz = n.size();
	bool flag = size() < sz;
	unsigned tmp = 0;				// overflow flag
	if (flag)
		{ safe_resize(sz); }
	for (int i = 0; i < sz; ++i)
	{
		unsigned src = n[i];
		tmp = (operator[](i) += src + tmp) < src;	// judge overflow if a + b < b
	}
	if (!flag)
	{
		int _sz = size();
		for (int i = sz; tmp && i < _sz; ++i)
		{
			unsigned src = n.filter;
			tmp = (operator[](i) += src + tmp) < src + tmp;
		}
	}
	safe_push_back(filter + n.filter + tmp);		// assume an extra value for sign
	pop_top_zeroes();
	return *this;
}

longint& longint::operator -= (const longint& n)
{ return *this += -n; }

// optimized due to cutting down if branches
longint& longint::operator >>= (int n)
{
	if (n >= 0)
	{
		int low_pos = n >> 5, diff = n & 31, sz = size();
		if (low_pos < sz)
		{
			front() = operator[](low_pos) >> diff;
			for (int i = low_pos + 1; i < sz; ++i)
			{
				unsigned src = operator[](i);
				operator[](i - low_pos) = src >> diff;
				operator[](i - low_pos - 1) |= src << 32 - diff;
			}
			resize(sz - low_pos);
			back() |= filter << 32 - diff;
			pop_top_zeroes();
		}
		else
		{ resize(1); front() = filter; }
	}
	else
	{ return operator<<=(-n); }
	return *this;
}

longint& longint::operator <<= (int n)
{
	if (n >= 0)
	{
		int low_pos = n >> 5, diff = n & 31;
		int sz = size();
		resize(low_pos + sz + 1);
		back() = filter << diff;
		for (int i = sz - 1; i >= 0; --i)
		{
			unsigned src = operator[](i);
			operator[](i + low_pos) = src << diff;
			if (diff > 0)
				operator[](i + low_pos + 1) |= src >> 32 - diff;
		}
		for (int i = 0; i < low_pos; ++i)
			{ operator[](i) = 0; }
		pop_top_zeroes();
	}
	else
	{ return operator>>=(-n); }
	return *this;
}

longint& longint::operator |= (const longint& n)
{
	if (this != &n)
	{
		int sz = n.size(), _sz = size();
		filter |= n.filter;
		int low_pos = std::min(sz, _sz);
		for (int i = 0; i < low_pos; ++i)
			{ operator[](i) |= n[i]; }
		if (filter)
			{ resize(low_pos); }
		else if (_sz < sz)
			{ for (int i = _sz; i < sz; ++i)
			 	{ push_back(n[i]); }}
		pop_top_zeroes();
	}
	return *this;
}

longint& longint::operator &= (const longint& n)
{
	if (this != &n)
	{
		int sz = n.size(), _sz = size();
		filter &= n.filter;
		int low_pos = std::min(sz, _sz);
		for (int i = 0; i < low_pos; ++i)
			{ operator[](i) &= n[i]; }
		if (!filter)
			{ resize(low_pos); }
		else if (_sz < sz)
			{ for (int i = _sz; i < sz; ++i)
			 	{ push_back(n[i]); }}
		pop_top_zeroes();
	}
	return *this;
}

longint& longint::operator ^= (const longint& n)
{
	if (this != &n)
	{
		int sz = n.size(), _sz = size();
		filter ^= n.filter;
		int low_pos = std::min(sz, _sz);
		for (int i = 0; i < low_pos; ++i)
			{ operator[](i) ^= n[i]; }
		if (_sz < sz)
		{
			safe_resize(sz);
			for (int i = _sz; i < sz; ++i)
			 	{ operator[](i) ^= n[i]; }
		}
		else
		{
			for (int i = sz; i < _sz; ++i)
			 	{ operator[](i) ^= n.filter; }
		}
		pop_top_zeroes();
	}
	return *this = 0;
}

longint& longint::operator *= (int n)
{
	long long tmp = 0;
	for (auto &var : *this)
	{
		var = tmp += (long long)var * n;
		tmp >>= 32;
	}
	safe_push_back(tmp + filter * n);
	pop_top_zeroes();
	return *this;
}

longint& longint::operator *= (unsigned n)
{
	unsigned long long tmp = 0;
	for (auto &var : *this)
	{
		var = tmp += (unsigned long long)var * n;
		tmp >>= 32;
	}
	safe_push_back(tmp + filter * n);
	pop_top_zeroes();
	return *this;
}
// bugs
longint& longint::operator /= (int n)
{
	long long tmp = (long long)filter << 32;
	filter ^= - (n < 0);
	auto iter = rbegin(), iend = rend() - 1;
	for (; iter < iend; ++iter)
	{
		*iter = (tmp |= *iter) / n - bool(filter);
		tmp = tmp % n << 32;
	}
	front() = (front() | tmp) / n;
	pop_top_zeroes();
	return *this; 
}

longint& longint::operator *= (const longint& n)
{
	if (this != &n)
	{
		longint val = *this;//(filter ? -*this : *this);
		*this = 0;
		for (auto var : n)
		{	
			operator+=(val * var);
			val <<= 32;
		}
		if (n.filter) { operator-=(val); }		// set sign to right state
		filter = val.filter ^ n.filter;
		pop_top_zeroes();
		return *this;
	}
	return *this *= longint(n);
}

// double const
// TODO: improve this
longint longint::operator / (const longint& n) const
{
	//bool m_rev = *this < 0, n_rev = n < 0;
	longint _m(filter ? -*this : *this), _n(n.filter ? -n : n);
	int low_pos = _m.size() - _n.size() + 1;
	if (low_pos < 0) return 0;
	longint tmp(_m >> (low_pos << 5)), result(0);
	auto iter = _m.rbegin(), iend = _m.rend();
	for (; iter < iend; ++iter)
	{
		for (int j = 32 - 1; j >= 0; --j)
		{
			tmp <<= 1;
			if (*iter & 1 << j) { tmp |= 1; }
			result <<= 1;
			if (_n <= tmp) { result |= 1; tmp -= _n; }
		}
	}
	result.pop_top_zeroes();
	if (filter ^ n.filter) result = -result;
	return result;
}

// output method

std::string longint::as_string() const
{
	vector<unsigned> dig_arr;		// < src_max
	dig_arr.push_back(0);
	longint tmp = !filter ? *this : -*this;
	for (int iter = tmp.size() - 1; iter >= 0; --iter)
	{
		for (int i = 31; i >= 0; --i)
		{
			int d = (tmp[iter] & 1 << i) != 0;
			for (auto &p: dig_arr)
			{
				p <<= 1;
				p |= d;
				d = p / src_max;
				p %= src_max;
			}
			if (d) { dig_arr.push_back(d); }
		}
	}
	//printf("%d %d %d\n", dig_arr.size(), dig_arr[0], dig_arr[1]);
	//printf("%d %d %d\n", tmp.size(), tmp[0], tmp[1]);
	char buff[src_deccount + 1] = {};
	vector<unsigned>::reverse_iterator iter(dig_arr.end()), end_iter(dig_arr.begin());
	sprintf(buff, "%u", *iter);
	std::string result = buff;
	if (filter) { result = "-" + result; }
	for (++iter; iter < end_iter; ++iter)
	{
		sprintf(buff, "%09u", *iter);
		result += buff;
	}
	return result;
}
