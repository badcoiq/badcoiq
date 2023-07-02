/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "badcoiq.h"

static uint8_t g_stringWordSize = 16;
const static float g_string_to_float_table[17] =
{
	0.f,
	0.1f,
	0.01f,
	0.001f,
	0.0001f,
	0.00001f,
	0.000001f,
	0.0000001f,
	0.00000001f,
	0.000000001f,
	0.0000000001f,
	0.00000000001f,
	0.000000000001f,
	0.0000000000001f,
	0.00000000000001f,
	0.000000000000001f,
	0.0000000000000001f,
};

// для to_utf8 to_utf16.
union UC
{
	uint8_t bytes[4];
	uint16_t shorts[2];
	uint32_t integer;
};

struct slUnicodeCharNode
{
	uint32_t m_utf8; // код для UTF-8
	uint32_t m_utf16;// код для UTF-16
};
static slUnicodeCharNode g_UnicodeChars[0x32000] =
{
#include "UnicodeChars.inl" // коды здесь
};

void bqString::reallocate(size_t new_allocated)
{
	char32_t* new_data = (char32_t*)bqMemory::malloc(new_allocated * sizeof(char32_t));
	if (m_data)
	{
		memcpy(new_data, m_data, m_size * sizeof(char32_t));
		bqMemory::free(m_data);
	}
	else
	{
		memset(new_data, 0, new_allocated);
	}
	m_data = new_data;
	m_allocated = new_allocated;
}

bqString::bqString()
{
	m_allocated = g_stringWordSize;
	reallocate(m_allocated);
}

bqString::bqString(const char* s) : bqString()
{
	assign(s);
}

bqString::bqString(const wchar_t* s) : bqString()
{
	assign(s);
}

bqString::bqString(const char8_t* s) : bqString()
{
	assign(s);
}

bqString::bqString(const char16_t* s) : bqString()
{
	assign(s);
}

bqString::bqString(const char32_t* s) : bqString()
{
	assign(s);
}

bqString::bqString(const bqString& s) : bqString()
{
	assign(s);
}

bqString::bqString(bqString&& s) : bqString()
{
	this->operator=(std::move(s));
}

bqString::~bqString()
{
	if (m_data)
		bqMemory::free(m_data);
}

const char32_t* bqString::c_str() const
{
	return m_data;
}

size_t bqString::size() const
{
	return m_size;
}

void bqString::reserve(size_t size)
{
	if (size > m_allocated)
	{
		reallocate(size);
		m_data[m_size] = 0;
	}
}

void bqString::clear()
{
	m_data[0] = 0;
	m_size = 0;
}

void bqString::assign(const char* s)
{
	clear();
	append(s);
}

void bqString::assign(const wchar_t* s)
{
	clear();
	append(s);
}

void bqString::assign(const char8_t* s)
{
	clear();
	append(s);
}

void bqString::assign(const char16_t* s)
{
	clear();
	append(s);
}

void bqString::assign(const char32_t* s)
{
	clear();
	append(s);
}

void bqString::assign(const bqString& s)
{
	clear();
	append(s);
}

// UTF-8 to UTF-32
void bqString::append(const char* str)
{
	size_t len = bqStringLen(str);

	unsigned char c1 = 0;
	unsigned char c2 = 0;
	unsigned char c3 = 0;
	unsigned char c4 = 0;
	for (size_t i = 0; i < len; ++i)
	{
		c1 = str[i];

		if (c1 <= 0x7F)
		{
			push_back((char32_t)c1);
		}
		else
		{
			if ((c1 & 0xE0) == 0xC0) //2 bytes
			{
				++i;
				if (i < len)
				{
					c2 = str[i];
					if ((c2 & 0xC0) == 0x80)
					{
						char16_t wch = (c1 & 0x1F) << 6;
						wch |= (c2 & 0x3F);
						push_back((char32_t)wch);
					}
				}
			}
			else if ((c1 & 0xF0) == 0xE0) //3
			{
				++i;
				if (i < len)
				{
					c2 = str[i];
					if ((c2 & 0xC0) == 0x80)
					{
						++i;
						if (i < len)
						{
							c3 = str[i];
							if ((c3 & 0xC0) == 0x80)
							{
								char16_t wch = (c1 & 0xF) << 12;
								wch |= (c2 & 0x3F) << 6;
								wch |= (c3 & 0x3F);
								push_back((char32_t)wch);
							}
						}
					}
				}
			}
			else if ((c1 & 0xF8) == 0xF0) //4
			{
				++i;
				if (i < len)
				{
					c2 = str[i];
					if ((c2 & 0xC0) == 0x80)
					{
						++i;
						if (i < len)
						{
							c3 = str[i];
							if ((c3 & 0xC0) == 0x80)
							{
								++i;
								if (i < len)
								{
									c4 = str[i];
									if ((c4 & 0xC0) == 0x80)
									{
										uint32_t u = (c1 & 0x7) << 18;
										u |= (c2 & 0x3F) << 12;
										u |= (c3 & 0x3F) << 6;
										u |= (c4 & 0x3F);

										push_back((char32_t)u);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

// UTF-16 to UTF-32
void bqString::append(const wchar_t* str)
{
	size_t len = bqStringLen(str);
	for (size_t i = 0u; i < len; ++i)
	{
		char16_t ch16 = str[i];
		if (ch16 < 0x80)
		{
			push_back((char32_t)ch16);
		}
		else if (ch16 < 0x800)
		{
			push_back((char32_t)ch16); //????
		}
		else if ((ch16 & 0xFC00) == 0xD800)
		{
			++i;
			if (i < len)
			{
				char16_t ch16_2 = str[i];
				if ((ch16_2 & 0xFC00) == 0xDC00)
				{
					uint32_t u = (ch16 - 0xD800) * 0x400;
					u += (ch16_2 - 0xDC00) + 0x10000;

					push_back((char32_t)u);
				}
			}
		}
		else
		{
			push_back((char32_t)ch16);
		}
	}
}

void bqString::append(const char8_t* s)
{
	append((const char*)s);
}

void bqString::append(const char16_t* s)
{
	append((const wchar_t*)s);
}

void bqString::append(const char32_t* s)
{
	size_t len = bqStringLen(s);

	for (size_t i = 0; i < len; ++i)
	{
		push_back(s[i]);
	}
}

void bqString::append(const bqString& s)
{
	for (size_t i = 0; i < s.m_size; ++i)
	{
		push_back(s.m_data[i]);
	}
}

void bqString::append(int8_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%i", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append(int16_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%i", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append(int32_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%i", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append(int64_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%lli", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append(uint8_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%u", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append(uint16_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%u", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}
void bqString::append(uint32_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%u", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}
void bqString::append(uint64_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%llu", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append_hex(uint64_t i)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%jx", i);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append(float f)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%.7f", f);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::append(double f)
{
	char buf[32u];
	int nob = sprintf_s(buf, 32, "%.12f", f);
	if (nob)
	{
		for (int i = 0; i < nob; ++i)
		{
			push_back((char32_t)buf[i]);
		}
	}
}

void bqString::push_back(char c)
{
	push_back((char32_t)c);
}

void bqString::push_back(wchar_t c)
{
	push_back((char32_t)c);
}

void bqString::push_back(char8_t c)
{
	push_back((char32_t)c);
}

void bqString::push_back(char16_t c)
{
	push_back((char32_t)c);
}

void bqString::push_back(char32_t c)
{
	size_t new_size = m_size + 1u;
	if ((new_size + 1u) > m_allocated)
		reallocate((new_size + 1u) + (1 + (size_t)(m_size * 0.5f)));
	m_data[m_size] = c;
	m_size = new_size;
	m_data[m_size] = 0;
}

void bqString::pop_back()
{
	if (m_size)
	{
		--m_size;
		m_data[m_size] = 0;
	}
}

char32_t bqString::pop_back_return()
{
	char32_t r = '?';
	if (m_size)
	{
		--m_size;
		r = m_data[m_size];
		m_data[m_size] = 0;
	}
	return r;
}

void bqString::pop_back_before(char32_t before_this)
{
	if (size())
		pop_back();

	if (size())
	{
		for (size_t i = size() - 1u; i >= 0u; --i)
		{
			if (m_data[i] == before_this)
				break;
			else
				pop_back();
			if (!i)
				break;
		}
	}
}

void bqString::pop_front()
{
	erase(0u, 0u);
}

void bqString::replace(char32_t a, char32_t b)
{
	for (size_t i = 0; i < m_size; ++i)
	{
		if (m_data[i] == a)
			m_data[i] = b;
	}
}

void bqString::shrink_to_fit()
{
	if (m_size)
	{
		if (m_allocated > (m_size + (1 + (uint32_t)(m_size * 0.5f))))
		{
			reallocate(m_size + 1u);
			m_data[m_size] = 0;
		}
	}
}

void bqString::erase(size_t begin, size_t end)
{
	size_t numCharsToDelete = end - begin + 1u; // delete first char: 0 - 0 + 1
	size_t next = end + 1u;
	for (size_t i = begin; i < m_size; ++i)
	{
		if (next < m_size)
		{
			m_data[i] = m_data[next];
			++next;
		}
	}
	m_size -= numCharsToDelete;
	m_data[m_size] = 0;
}

void bqString::insert(const char32_t* str, size_t where)
{
	size_t str_len = bqStringLen(str);
	size_t new_size = str_len + m_size;
	if ((new_size + 1u) > m_allocated)
		reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));

	size_t i = m_size;
	while (i >= where)
	{
		m_data[i + str_len] = m_data[i];
		if (i == 0)
			break;
		--i;
	}
	for (size_t i = 0; i < str_len; ++i)
	{
		m_data[i + where] = str[i];
	}
	m_size = new_size;
	m_data[m_size] = 0;
}

// Only for ASCII
void bqString::trim_spaces()
{
	while (true)
	{
		if (isspace(m_data[0]))
			pop_front();
		else
			break;
	}

	while (true)
	{
		if (isspace(m_data[m_size - 1]))
			pop_back();
		else
			break;
	}
}

void bqString::to_lower()
{
	if (!m_size)
		return;
	for (size_t i = 0u; i < m_size; ++i)
	{
		auto c = m_data[i];
		if (c <= 'Z' && c >= 'A')
			m_data[i] += 32;
	}
}

void bqString::to_upper()
{
	if (!m_size)
		return;
	for (size_t i = 0u; i < m_size; ++i)
	{
		auto c = m_data[i];
		if (c <= 'z' && c >= 'a')
			m_data[i] -= 32;
	}
}

void bqString::flip()
{
	if (!m_size)
		return;
	bqString flippedStr;
	for (size_t i = m_size - 1u; i >= 0u; --i)
	{
		flippedStr += m_data[i];
		if (!i)
			break;
	}
	this->operator=(flippedStr);
}

void bqString::flip_slash()
{
	if (!m_size)
		return;
	for (size_t i = 0u; i < m_size; ++i)
	{
		if (m_data[i] == '\\')
			m_data[i] = '/';
	}
}

void bqString::flip_backslash()
{
	if (!m_size)
		return;
	for (size_t i = 0u; i < m_size; ++i)
	{
		if (m_data[i] == '/')
			m_data[i] = '\\';
	}
}

int32_t bqString::to_int()
{
	size_t len = m_size;
	int32_t result = 0;
	int mul_val = 1;
	bool is_neg = m_data[0] == U'-';
	for (size_t i = 0, last = len - 1; i < len; ++i)
	{
		int char_value = (int)m_data[last] - 0x30;
		if (char_value >= 0 && char_value <= 9)
		{
			result += char_value * mul_val;
			mul_val *= 10;
		}
		--last;
	}
	if (is_neg)
		result = -result;
	return result;
}

int64_t bqString::to_intll()
{
	size_t len = m_size;
	int64_t result = 0;
	int mul_val = 1;
	bool is_neg = m_data[0] == U'-';
	for (size_t i = 0, last = len - 1; i < len; ++i)
	{
		int char_value = (int)m_data[last] - 0x30;
		if (char_value >= 0 && char_value <= 9)
		{
			result += char_value * mul_val;
			mul_val *= 10;
		}
		--last;
	}
	if (is_neg)
		result = -result;
	return result;
}

uint32_t bqString::to_uint()
{
	size_t len = m_size;
	uint32_t result = 0;
	int mul_val = 1;
	bool is_neg = m_data[0] == U'-';
	for (size_t i = 0, last = len - 1; i < len; ++i)
	{
		int char_value = (int)m_data[last] - 0x30;
		if (char_value >= 0 && char_value <= 9)
		{
			result += char_value * mul_val;
			mul_val *= 10;
		}
		--last;
	}
	return result;
}

uint64_t bqString::to_uintll()
{
	size_t len = m_size;
	uint64_t result = 0;
	int mul_val = 1;
	bool is_neg = m_data[0] == U'-';
	for (size_t i = 0, last = len - 1; i < len; ++i)
	{
		int char_value = (int)m_data[last] - 0x30;
		if (char_value >= 0 && char_value <= 9)
		{
			result += char_value * mul_val;
			mul_val *= 10;
		}
		--last;
	}
	return result;
}

float bqString::to_float()
{
	float result = 0.f;
	auto ptr = m_data;
	bool is_negative = *ptr == U'-';

	if (is_negative)
		++ptr;

	int i = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
	}
	result = (float)i;

	i = 0;

	if (*ptr == U'.')
		++ptr;

	int part_2_count = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
		++part_2_count;
	}

	result += (float)i * (float)g_string_to_float_table[part_2_count];

	return is_negative ? -result : result;
}

double bqString::to_double()
{
	double result = 0.f;
	auto ptr = m_data;
	bool is_negative = *ptr == U'-';

	if (is_negative)
		++ptr;

	int i = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
	}
	result = (double)i;

	i = 0;

	if (*ptr == U'.')
		++ptr;

	int part_2_count = 0;
	while (*ptr >= U'0' && *ptr <= U'9')
	{
		i *= 10;
		i += *ptr - U'0';
		++ptr;
		++part_2_count;
	}
	result += (double)i * g_string_to_float_table[part_2_count];

	return is_negative ? -result : result;
}

bqString& bqString::operator=(const char* s)
{
	clear();
	append(s);
	return *this;
}

bqString& bqString::operator=(const wchar_t* s)
{
	clear();
	append(s);
	return *this;
}

bqString& bqString::operator=(const char8_t* s)
{
	clear();
	append(s);
	return *this;
}

bqString& bqString::operator=(const char16_t* s)
{
	clear();
	append(s);
	return *this;
}

bqString& bqString::operator=(const char32_t* s)
{
	clear();
	append(s);
	return *this;
}

bqString& bqString::operator=(const bqString& s)
{
	clear();
	append(s);
	return *this;
}

bqString& bqString::operator=(bqString&& s)
{
	if (m_data)
		bqMemory::free(m_data);
	m_data = s.m_data;
	m_allocated = s.m_allocated;
	m_size = s.m_size;

	s.m_data = 0;
	s.m_allocated = 0;
	s.m_size = 0;
	return *this;
}

const char32_t& bqString::operator[](size_t i) const
{
	return m_data[i];
}

char32_t& bqString::operator[](size_t i)
{
	return m_data[i];
}

const char32_t& bqString::at(size_t i) const
{
	return m_data[i];
}

char32_t& bqString::at(size_t i)
{
	return m_data[i];
}

void bqString::operator+=(const char* s)
{
	append(s);
}

void bqString::operator+=(const wchar_t* s)
{
	append(s);
}

void bqString::operator+=(const char8_t* s)
{
	append(s);
}

void bqString::operator+=(const char16_t* s)
{
	append(s);
}

void bqString::operator+=(const char32_t* s)
{
	append(s);
}

void bqString::operator+=(int8_t i)
{
	append(i);
}

void bqString::operator+=(int16_t i)
{
	append(i);
}

void bqString::operator+=(int32_t i)
{
	append(i);
}

void bqString::operator+=(int64_t i)
{
	append(i);
}

void bqString::operator+=(uint8_t i)
{
	append(i);
}

void bqString::operator+=(uint16_t i)
{
	append(i);
}

void bqString::operator+=(uint32_t i)
{
	append(i);
}

void bqString::operator+=(uint64_t i)
{
	append(i);
}

void bqString::operator+=(float f)
{
	append(f);
}

void bqString::operator+=(double f)
{
	append(f);
}

bool bqString::operator==(const bqString& other) const
{
	if (other.size() != m_size)
		return false;

	const size_t sz = other.size();
	const auto* ptr = other.m_data;
	for (size_t i = 0u; i < sz; ++i)
	{
		if (ptr[i] != m_data[i])
			return false;
	}

	return true;
}

bool bqString::operator!=(const bqString& other) const
{
	if (other.size() != m_size)
		return true;

	const size_t sz = other.size();
	const auto* ptr = other.m_data;
	for (size_t i = 0u; i < sz; ++i)
	{
		if (ptr[i] != m_data[i])
			return true;
	}

	return false;
}

void bqString::to_utf8(bqStringA& stra) const
{
	stra.clear();
	UC uc;
	auto ut = &g_UnicodeChars[0];
	for (size_t i = 0; i < m_size; ++i)
	{
		char32_t c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf8;

		if (uc.bytes[3]) stra.push_back(uc.bytes[3]);
		if (uc.bytes[2]) stra.push_back(uc.bytes[2]);
		if (uc.bytes[1]) stra.push_back(uc.bytes[1]);
		if (uc.bytes[0]) stra.push_back(uc.bytes[0]);
	}
}

void bqString::to_utf16(bqStringW& strw) const
{
	strw.clear();
	UC uc;
	auto ut = &g_UnicodeChars[0];
	for (size_t i = 0; i < m_size; ++i)
	{
		char32_t c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf16;

		if (uc.shorts[1]) strw.push_back(uc.shorts[1]);
		if (uc.shorts[0]) strw.push_back(uc.shorts[0]);
	}
}

bqString bqString::operator+(const char* v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(const wchar_t* v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(const char8_t* v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(const char16_t* v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(const char32_t* v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(int8_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(int16_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(int32_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(int64_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(uint8_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(uint16_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(uint32_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(uint64_t v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(float v)
{
	bqString s = *this;
	s += v;
	return s;
}

bqString bqString::operator+(double v)
{
	bqString s = *this;
	s += v;
	return s;
}



