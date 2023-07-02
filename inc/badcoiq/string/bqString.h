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

#pragma once
#ifndef __BQ_STRING_H__
#define __BQ_STRING_H__

// Функция чтобы получить длинну строки
template<typename other_type>
size_t bqStringLen(const other_type* str)
{
	// Считаем пока не встретится ноль
	unsigned int len = 0u;
	if (str[0] != 0)
	{
		const other_type* p = &str[0u];
		while ((size_t)*p++)
			++len;
	}
	return len;
}

// Традиционный строковый класс
template<typename char_type>
class bqString_base
{
	char_type* m_data = 0;       // массив
	size_t     m_allocated = 10; // количество символов которое влезает в m_data учитывая завершающий ноль
	size_t     m_size = 0;       // текущее количество символов

	// если количество символов m_size равно или больше m_allocated то надо вызвать это
	//  указав новый m_allocated
	void _reallocate(size_t new_allocated)
	{
		// выделение новой памяти, копирование содержимого, освобождение старой, сохранение нового указателя
		char_type* new_data = (char_type*)bqMemory::malloc(new_allocated * sizeof(char_type));
		if (m_data)
		{
			memcpy(new_data, m_data, m_size * sizeof(char_type));
			bqMemory::free(m_data);
		}
		else
		{
			memset(new_data, 0, new_allocated);
		}
		m_data = new_data;
		m_allocated = new_allocated;
	}

public:
	// в конструкторах надо вызвать _reallocate() чтобы инициализировать массив m_data
	bqString_base()
	{
		_reallocate(m_allocated);
	}

	template<typename other_type>
	bqString_base(const other_type* str)
	{
		_reallocate(m_allocated);
		assign(str); // просто присвоение
	}

	bqString_base(const bqString_base<char_type>& str)
	{
		_reallocate(m_allocated);
		assign(str);
	}

	~bqString_base()
	{
		if (m_data)
			bqMemory::free(m_data);
	}

	size_t capacity()
	{
		return m_allocated;
	}

	// будет необходимо сравнивать строки, например расширение файла типа .png с .PNG и
	//  проще перевести символы в нижний регистр.
	//  to_upper нет так как он пока не нужен.
	void to_lower()
	{
		if (!m_size)
			return;
		for (size_t i = 0; i < m_size; ++i)
		{
			auto c = m_data[i];
			if (c <= 'Z' && c >= 'A')
				m_data[i] += 32;
		}
	}

	// перевернуть строку, например abc -> cba
	void flip()
	{
		if (!m_size)
			return;
		bqString_base<char_type> flippedStr;
		for (size_t i = m_size - 1; i >= 0; --i)
		{
			flippedStr += m_data[i];
			if (!i)
				break;
		}
		this->operator=(flippedStr);
	}

	// перевернуть '\\' -> '/'
	void flip_slash()
	{
		if (!m_size)
			return;
		for (size_t i = 0; i < m_size; ++i)
		{
			if (m_data[i] == '\\')
				m_data[i] = '/';
		}
	}

	// перевернуть '/' -> '\\'
	void flip_backslash()
	{
		if (!m_size)
			return;
		for (size_t i = 0; i < m_size; ++i)
		{
			if (m_data[i] == '/')
				m_data[i] = '\\';
		}
	}

	// заранее выделить память
	void reserve(size_t size)
	{
		if (size >= m_allocated)
		{
			_reallocate(size + 1);
			m_data[m_size] = 0;
		}
	}

	// присвоить содержимое другой строки
	template<typename other_type>
	void assign(other_type str)
	{
		clear();
		if (str)
			append(str);
	}

	void assign(const bqString_base<char_type>& str)
	{
		clear();
		append(str);
	}

	// добавить в конец
	template<typename other_type>
	void append(const other_type* str, size_t l)
	{
		for (size_t i = 0; i < l; ++i)
		{
			push_back((char_type)str[i]);
		}
	}

	template<typename other_type>
	void append(const other_type* str)
	{
		append(str, bqStringLen(str));
	}

	void append(const bqString_base<char_type>& str)
	{
		append(str.data(), str.m_size);
	}

	// добавить в конец символ
	void push_back(char_type c)
	{
		size_t new_size = m_size + 1u;
		if ((new_size + 1u) > m_allocated)
			_reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));
		m_data[m_size] = c;
		m_size = new_size;
		m_data[m_size] = 0;
	}

	// добавить число
	void append(float f)
	{
		char buf[32];
		int nob = sprintf_s(buf, 32, "%.4f", f);
		if (nob)
		{
			for (int i = 0; i < nob; ++i)
			{
				push_back((char_type)buf[i]);
			}
		}
	}

	void append_float(float f)
	{
		char buf[32];
		int nob = sprintf_s(buf, 32, "%.7f", f);
		if (nob)
		{
			for (int i = 0; i < nob; ++i)
			{
				push_back((char_type)buf[i]);
			}
		}
	}

	void append(double f)
	{
		char buf[32u];
		int nob = sprintf_s(buf, 32, "%.12f", f);
		if (nob)
		{
			for (int i = 0; i < nob; ++i)
			{
				push_back((char_type)buf[i]);
			}
		}
	}

	void append(uint32_t i)
	{
		char buf[32u];
		int nob = sprintf_s(buf, 32, "%u", i);
		if (nob)
		{
			for (int i = 0; i < nob; ++i)
			{
				push_back((char_type)buf[i]);
			}
		}
	}

	void append(int32_t i)
	{
		char buf[32u];
		int nob = sprintf_s(buf, 32, "%i", i);
		if (nob)
		{
			for (int i = 0; i < nob; ++i)
			{
				push_back((char_type)buf[i]);
			}
		}
	}

	void append(uint64_t i)
	{
		char buf[64u];
		int nob = sprintf_s(buf, 64, "%llu", i);
		if (nob)
		{
			for (int i = 0; i < nob; ++i)
			{
				push_back((char_type)buf[i]);
			}
		}
	}

	void append(int64_t i)
	{
		char buf[64u];
		int nob = sprintf_s(buf, 64, "%ll", i);
		if (nob)
		{
			for (int i = 0; i < nob; ++i)
			{
				push_back((char_type)buf[i]);
			}
		}
	}

	bqString_base<char_type>& operator=(const bqString_base<char_type>& s)
	{
		assign(s);
		return *this;
	}

	template<typename other_type>
	char_type& operator=(other_type* str)
	{
		assign(str);
		return *this;
	}

	const char_type* c_str() const
	{
		return m_data;
	}

	char_type* data() const
	{
		return m_data;
	}

	const size_t size() const
	{
		return m_size;
	}

	char_type* begin() const { return m_data; }
	char_type* end() const { return (m_data + (m_size)); }
	const char_type& operator[](size_t i) const { return m_data[i]; }
	char_type& operator[](size_t i) { return m_data[i]; }
	void operator+=(int32_t i) { append(i); }
	void operator+=(int64_t i) { append(i); }
	void operator+=(uint32_t i) { append(i); }
	void operator+=(uint64_t i) { append(i); }
	void operator+=(float i) { append(i); }
	void operator+=(double i) { append(i); }
	void operator+=(char_type c) { push_back(c); }
	template<typename other_type>
	void operator+=(other_type* str) { append(str); }
	void operator+=(const char_type& str) { append(str); }

	bool operator==(const char_type& other) const
	{
		if (other.m_size != m_size)
			return false;

		const size_t sz = other.m_size;
		const auto* ptr = other.m_data;
		for (size_t i = 0; i < sz; ++i)
		{
			if (ptr[i] != m_data[i])
				return false;
		}

		return true;
	}

	bool operator!=(const char_type& other) const
	{
		if (other.m_size != m_size)
			return true;

		const size_t sz = other.m_size;
		const auto* ptr = other.m_data;
		for (size_t i = 0; i < sz; ++i)
		{
			if (ptr[i] != m_data[i])
				return true;
		}

		return false;
	}

	void clear()
	{
		m_size = 0;
		m_data[m_size] = 0;
	}

	// убрать символ с конца
	void pop_back()
	{
		if (m_size)
		{
			--m_size;
			m_data[m_size] = 0;
		}
	}

	// убрать символ с конца и вернуть его
	char_type pop_back_return()
	{
		char_type r = '?';
		if (m_size)
		{
			--m_size;
			r = m_data[m_size];
			m_data[m_size] = 0;
		}
		return r;
	}

	// заменить символ
	void replace(char_type a, char_type b)
	{
		for (size_t i = 0; i < m_size; ++i)
		{
			if (m_data[i] == a)
				m_data[i] = b;
		}
	}

	// Убирать символ с конца пока не встретится символ.
	// Встреченный символ не уберётся.
	void pop_back_before(char_type before_this)
	{
		if (m_size)
			pop_back();

		if (m_size)
		{
			for (size_t i = m_size - 1; i >= 0; --i)
			{
				if (m_data[i] == (char_type)before_this)
					break;
				else
					pop_back();
				if (!i)
					break;
			}
		}
	}

	// Освободить лишнюю выделенную память
	void shrink_to_fit()
	{
		if (m_size)
		{
			if (m_allocated > (m_size + (1 + (uint32_t)(m_size * 0.5f))))
			{
				_reallocate(m_size + 1);
				m_data[m_size] = static_cast<char_type>(0x0);
			}
		}
	}

	// Удалить символы
	void erase(size_t begin, size_t end)
	{
		size_t numCharsToDelete = end - begin + 1;
		size_t next = end + 1;
		for (size_t i = begin; i < m_size; ++i)
		{
			if (next < m_size)
			{
				m_data[i] = m_data[next];
				++next;
			}
		}
		m_size -= numCharsToDelete;
		m_data[m_size] = static_cast<char_type>(0x0);
	}

	// Удалить символ с начала
	void pop_front()
	{
		erase(0, 0);
	}

	// конвертировать в int
	int32_t to_int()
	{
		size_t len = m_size;
		int result = 0;
		int mul_val = 1;
		bool is_neg = m_data[0] == (char_type)'-';
		for (size_t i = 0, last = len - 1; i < len; ++i)
		{
			int32_t char_value = (int32_t)m_data[last] - 0x30;
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

	// конвертировать в float
	float to_float()
	{
		float result = 0.f;
		auto ptr = m_data;
		bool is_negative = *ptr == (char_type)'-';

		if (is_negative)
			++ptr;

		int i = 0;
		while (*ptr >= (char_type)'0' && *ptr <= (char_type)'9')
		{
			i *= 10;
			i += *ptr - (char_type)'0';
			++ptr;
		}
		result = (float)i;

		i = 0;

		if (*ptr == (char_type)'.')
			++ptr;

		int part_2_count = 0;
		while (*ptr >= (char_type)'0' && *ptr <= (char_type)'9')
		{
			i *= 10;
			i += *ptr - (char_type)'0';
			++ptr;
			++part_2_count;
		}

		const static float string_to_float_table[17] =
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

		result += (float)i * string_to_float_table[part_2_count];

		return is_negative ? -result : result;
	}
};

using bqStringW = bqString_base<wchar_t>;
using bqStringA = bqString_base<char>;

// Unicode строка
class bqString
{
	size_t m_size = 0;
	size_t m_allocated = 0;
	char32_t* m_data = nullptr;

	void reallocate(size_t new_allocated);
public:
	bqString();
	bqString(const char*);
	bqString(const wchar_t*);
	bqString(const char8_t*);
	bqString(const char16_t*);
	bqString(const char32_t*);
	bqString(const bqString&);
	bqString(bqString&&);
	~bqString();

	const char32_t* c_str()const;
	size_t size() const;
	void reserve(size_t size);
	void clear();

	void assign(const char*);
	void assign(const wchar_t*);
	void assign(const char8_t*);
	void assign(const char16_t*);
	void assign(const char32_t*);
	void assign(const bqString&);

	void append(const char*);
	void append(const wchar_t*);
	void append(const char8_t*);
	void append(const char16_t*);
	void append(const char32_t*);
	void append(const bqString&);
	void append(int8_t);
	void append(int16_t);
	void append(int32_t);
	void append(int64_t);
	void append(uint8_t);
	void append(uint16_t);
	void append(uint32_t);
	void append(uint64_t);
	void append_hex(uint64_t);
	void append(float);
	void append(double);

	void push_back(char);
	void push_back(wchar_t);
	void push_back(char8_t);
	void push_back(char16_t);
	void push_back(char32_t);

	void pop_back();
	char32_t pop_back_return();
	void pop_back_before(char32_t before_this);
	void pop_front();
	void replace(char32_t a, char32_t b);
	void shrink_to_fit();

	void erase(size_t begin, size_t end);
	void insert(const char32_t* str, size_t where);
	void trim_spaces();

	// Only for ASCII
	void to_lower();
	void to_upper();
	void flip();
	void flip_slash();
	void flip_backslash();
	int32_t to_int();
	int64_t to_intll();
	uint32_t to_uint();
	uint64_t to_uintll();
	float to_float();
	double to_double();

	bqString& operator=(const char*);
	bqString& operator=(const wchar_t*);
	bqString& operator=(const char8_t*);
	bqString& operator=(const char16_t*);
	bqString& operator=(const char32_t*);
	bqString& operator=(const bqString&);
	bqString& operator=(bqString&&);

	const char32_t& operator[](size_t i) const;
	char32_t& operator[](size_t i);
	const char32_t& at(size_t i) const;
	char32_t& at(size_t i);

	void operator+=(const char*);
	void operator+=(const wchar_t*);
	void operator+=(const char8_t*);
	void operator+=(const char16_t*);
	void operator+=(const char32_t*);
	void operator+=(int8_t);
	void operator+=(int16_t);
	void operator+=(int32_t);
	void operator+=(int64_t);
	void operator+=(uint8_t);
	void operator+=(uint16_t);
	void operator+=(uint32_t);
	void operator+=(uint64_t);
	void operator+=(float);
	void operator+=(double);

	bqString operator+(const char*);
	bqString operator+(const wchar_t*);
	bqString operator+(const char8_t*);
	bqString operator+(const char16_t*);
	bqString operator+(const char32_t*);
	bqString operator+(int8_t);
	bqString operator+(int16_t);
	bqString operator+(int32_t);
	bqString operator+(int64_t);
	bqString operator+(uint8_t);
	bqString operator+(uint16_t);
	bqString operator+(uint32_t);
	bqString operator+(uint64_t);
	bqString operator+(float);
	bqString operator+(double);

	bool operator==(const bqString& other) const;
	bool operator!=(const bqString& other) const;

	void to_utf8(bqStringA&) const;
	void to_utf16(bqStringW&) const;
};

#endif

