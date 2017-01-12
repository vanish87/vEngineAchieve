#ifndef COMMON_TOOLS_H_
#define COMMON_TOOLS_H_

#pragma once

//compile time string hash
//from http://www.klayge.org/?p=3361
constexpr size_t _Hash(char const * str, size_t seed)
{
	return 0 == *str ? seed : _Hash(str + 1, seed ^ (*str + 0x9e3779b9 + (seed << 6) + (seed >> 2)));
}

template <size_t N>
struct EnsureConst
{
	static const size_t value = N;
};

#define CT_HASH(x) (EnsureConst<_Hash(x, 0)>::value)

#endif