/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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
#include "badcoiq/common/bqStream.h"
#include "badcoiq/CRT/bqCRT.h"

#include "stdarg.h"

char __CRT_itoa_buffer[0x50];
char __CRT_dtoa_buffer[0x50];
char __CRT_itoa_internalBuffer[0x50];
const double __stdlib_string_to_double_table[17] =
{
	0.0,
	0.1,
	0.01,
	0.001,
	0.0001,
	0.00001,
	0.000001,
	0.0000001,
	0.00000001,
	0.000000001,
	0.0000000001,
	0.00000000001,
	0.000000000001,
	0.0000000000001,
	0.00000000000001,
	0.000000000000001,
	0.0000000000000001,
};

const long long __stdlib_string_to_double_table_hex[17] =
{
	1,
	16,
	256,
	4096,
	65536,
	1048576,
	16777216,
	268435456,
	4294967296,
	68719476736,
	1099511627776,
	17592186044416,
	281474976710656,
	4503599627370496,
	72057594037927936,
	1152921504606846976,
	-1,
};

const int __stdlib_string_to_int_table_hex[6] =
{
	10, 11, 12, 13, 14, 15
};
const char __stdlib_int_to_hex_table[16] =
{
	'0', '1', '2', '3','4','5','6','7','8','9','a','b','c','d','e','f',
};


namespace bq
{
	
	uint64_t __CRT_dectooctll(uint64_t dec)
	{
		uint64_t oct = 0;
		if (dec)
		{
			uint64_t m = 0;
			uint64_t m2 = 0;
			while (1)
			{
				uint64_t rem = dec % 8;
				dec /= 8;

				m2 = m * 10;
				if (!m2)
					oct += rem;
				else
					oct += rem * m2;
				if (!m)
					m = 1;
				else
					m *= 10;

				if (!dec)
					break;
			}
		}
		return oct;
	}
	template<typename char_type>
	void __CRT_dectohex(uint64_t dec, char_type* buf, int* sz)
	{
		if (dec)
		{
			double decInDouble = (double)dec;
			int bufInd = 0;
			while (1)
			{
				decInDouble /= 16.0;

				double ipart = 0.0;
				int i = (int)(modf(decInDouble, &ipart) * 16.0);//(dec % 16) * 16;
				dec /= 16; // for exit from loop

				__CRT_itoa_internalBuffer[bufInd++] = __stdlib_int_to_hex_table[i];

				if (!dec)
					break;
			}
			*sz = bufInd;
			if (bufInd)
			{
				for (int i = 0; i < bufInd; ++i)
				{
					buf[i] = __CRT_itoa_internalBuffer[bufInd - i - 1];
				}
				buf[bufInd] = 0;
			}
		}
		else
		{
			buf[0] = '0';
			buf[1] = 0;
		}
	}
	template<typename char_type>
	void __CRT_lltoa(long long val, /*not const*/ char_type* buf, size_t bufSz, int mode)
	{
		if (val)
		{
			long long val2 = val;

			switch (mode)
			{
			case 0:
			case 3:
			default:
			{
				if (mode == 3)
					val2 = __CRT_dectooctll(val);
				int bufferIndex = 0;
				while (1)
				{
					long long mod = val2 % 10;
					if (mod < 0) mod *= -1;
					__CRT_itoa_internalBuffer[bufferIndex++] = (char)mod;
					val2 = val2 / 10;

					if (!val2)
						break;
				}
				if (bufferIndex)
				{
					size_t dstBufInd = 0;
					if (val < 0)
						buf[dstBufInd++] = '-';

					for (int i = 0; i < bufferIndex; ++i)
					{
						buf[dstBufInd++] = __CRT_itoa_internalBuffer[bufferIndex - i - 1] + '0';
						if (dstBufInd == bufSz)
							break;
					}
					buf[dstBufInd] = 0;
				}
			}break;
			case 1:
			case 2:
			{
				int bsz = 0;
				char hexbuf[10];
				bq::__CRT_dectohex(val, hexbuf, &bsz);
				if (bsz)
				{
					for (int i = 0; i < bsz; ++i)
					{
						buf[i] = hexbuf[i];
						buf[i + 1] = 0;
					}
				}

				if (mode == 2)
				{
					for (size_t i2 = 0; i2 < bufSz; ++i2)
					{
						if (bqCRT::islower(buf[i2]))
							buf[i2] = bqCRT::toupper(buf[i2]);

						if (!buf[i2])
							break;
					}
				}
			}break;
			}
		}
		else
		{
			buf[0] = '0';
			buf[1] = 0;
		}
	}	

	template<typename char_type>
	void __CRT_ulltoa(uint64_t val, /*not const*/ char_type* buf, size_t bufSz, int mode)
	{
		if (val)
		{
			uint64_t val2 = val;

			switch (mode)
			{
			case 0:
			case 3:
			default:
			{
				if (mode == 3)
					val2 = __CRT_dectooctll(val);
				int bufferIndex = 0;
				while (1)
				{
					unsigned long long mod = val2 % 10;
					__CRT_itoa_internalBuffer[bufferIndex++] = (char)mod;

					val2 = val2 / 10;

					if (!val2)
						break;
				}
				if (bufferIndex)
				{
					size_t dstBufInd = 0;
					for (int i = 0; i < bufferIndex; ++i)
					{
						buf[dstBufInd++] = __CRT_itoa_internalBuffer[bufferIndex - i - 1] + '0';
						if (dstBufInd == bufSz)
							break;
					}
					buf[dstBufInd] = 0;
				}
			}
			break;
			case 1:
			case 2:
			{
				int bsz = 0;
				char hexbuf[10];
				__CRT_dectohex(val, hexbuf, &bsz);
				if (bsz)
				{
					for (int i = 0; i < bsz; ++i)
					{
						buf[i] = hexbuf[i];
						buf[i + 1] = 0;
					}
				}
				if (mode == 2)
				{
					for (size_t i2 = 0; i2 < bufSz; ++i2)
					{
						if (islower(buf[i2]))
							buf[i2] = toupper(buf[i2]);

						if (!buf[i2])
							break;
					}
				}
			}break;
			}
		}
		else
		{
			buf[0] = '0';
			buf[1] = 0;
		}
	}
	// mode: 
//  0 - normal,  decimal
//  1 - hex 7fa
//  2 - hex 7FA
//  3 - octal 610
	template<typename char_type>
	void __CRT_itoa(int val, char_type* buf, size_t bufSz, int mode)
	{
		__CRT_lltoa(val, buf, bufSz, mode);
	}
	template<typename char_type>
	void __CRT_uitoa(uint32_t val, /*not const*/ char_type* buf, size_t bufSz, int mode)
	{
		__CRT_ulltoa(val, buf, bufSz, mode);
	}


	template<typename char_type>
	const char_type* __string_skip_spaces(const char_type* str)
	{
		while (1)
		{
			if (*str > 0 && *str < 33)
				++str;
			else
				break;

			if (!*str)
				break;
		}
		return str;
	}

	template<typename char_type>
	int _atoi(const char_type* nptr)
	{
		assert(nptr);
		int result = 0;
		const char_type* str_ptr = bq::__string_skip_spaces(nptr);
		int isNeg = 0;
		if (*str_ptr == '-')
		{
			isNeg = 1;
			++str_ptr;
		}

		if (!*str_ptr)
			goto end;

		while (*str_ptr >= '0' && *str_ptr <= '9')
		{
			result *= 10;
			result += *str_ptr - '0';
			++str_ptr;

			if (!*str_ptr)
				break;
		}

	end:;
		return isNeg ? -result : result;
	}

	template<typename char_type>
	size_t strlen(const char_type* s)
	{
		if (!s)
			return 0;
		const char_type* b = s;

		while (*s)
			++s;
		return (size_t)(s - b);
	}

	template<typename char_type>
	double strtod(const char_type* nptr, char_type** endptr)
	{
		assert(nptr);
		double result = 0.0;
		long leftPart = 0;
		long rightPart = 0;
		int e_right = 0;
		int flags = 0; /* 1(-) 2(hex) 3(e) 4(e-)*/

		const char_type* str_ptr = __string_skip_spaces(nptr);
		const struct lconv* lc = localeconv();
		char decimal_point = *lc->decimal_point;

		size_t str_len = strlen(nptr);
		if (str_len >= 3)
		{
			char bf[4] = { 0,0,0,0 };
			for (int i = 0; i < 3; ++i)
			{
				if (isupper(nptr[i]))
					bf[i] = tolower(nptr[i]);
				else
					bf[i] = nptr[i];
			}
			if (strcmp(bf, "inf"))
				return INFINITY;

			if (strcmp(bf, "nan"))
			{
				result = NAN;
			}
		}

		/*with - or not*/
		if (*str_ptr == '-')
		{
			++str_ptr;
			flags |= 0x1;
		}

		if (!*str_ptr)
			goto end;

		/*hex or not, skip 00*/
		if (*str_ptr == '0')
		{
			++str_ptr;
			if (!*str_ptr)
				goto end;

			if (*str_ptr == 'x' || *str_ptr == 'X')
			{
				flags |= 0x2;
				++str_ptr;
			}
			else if (*str_ptr == decimal_point)
				--str_ptr;
			else if (*str_ptr < '1' || *str_ptr > '9')
				goto end;
		}

		if (!*str_ptr)
			goto end;

		long part_2_count = 0;

		if (flags & 0x2)/*hex*/
		{
			int charNum = 0;
			const unsigned char* save_str_ptr = str_ptr;
			while (*str_ptr)
			{
				if ((*str_ptr >= 'a' && *str_ptr <= 'f')
					|| (*str_ptr >= 'A' && *str_ptr <= 'F'))
					++charNum;
				else
					break;

				++str_ptr;

				if (!*str_ptr)
					break;
			}

			if (charNum)
			{
				str_ptr = save_str_ptr;
				int charNum2 = charNum;
				for (int i = 0; i < charNum; ++i)
				{
					int curr_char = str_ptr[i];
					if (curr_char < 'a')
						curr_char += 32;

					int curr_int = __stdlib_string_to_int_table_hex[curr_char - 97];

					leftPart += curr_int * (long)__stdlib_string_to_double_table_hex[charNum2 - 1];
					--charNum2;
				}
			}
		}
		else
		{
			/* left part 1234. */
			while (*str_ptr >= '0' && *str_ptr <= '9')
			{
				leftPart *= 10;
				leftPart += *str_ptr - '0';
				++str_ptr;

				if (!*str_ptr)
					break;
			}

			if (*str_ptr == 'e' || *str_ptr == 'E')
			{
				flags |= 0x4;
				++str_ptr;

				goto do_e;
			}

			/* right part .5678 */
			if (*str_ptr == decimal_point)
			{
				++str_ptr;

				if (!*str_ptr)
					goto finish;

				while (*str_ptr >= '0' && *str_ptr <= '9')
				{
					rightPart *= 10;
					rightPart += *str_ptr - '0';
					++str_ptr;
					++part_2_count;
				}

				if (*str_ptr == 'e' || *str_ptr == 'E')
				{
					flags |= 0x4;
					++str_ptr;

					goto do_e;
				}
			}
		}

	finish:;
		result = leftPart + ((float)rightPart * __stdlib_string_to_double_table[part_2_count]);

		if (flags & 0x4)
		{
			double em = 1.0;

			for (int i = 0; i < e_right; ++i)
			{
				if (flags & 0x8) /* - */
					em *= 0.1;
				else
					em *= 10.0;
			}

			result = result * em;
		}

		result = (flags & 0x1) ? -result : result;

	end:;
		if (endptr)
			*endptr = (char_type*)str_ptr;

		return result;

	do_e:;

		if (!*str_ptr)
			goto finish;

		if (*str_ptr == '-')
		{
			flags |= 0x8;

			++str_ptr;
			if (!*str_ptr)
				goto finish;
		}
		else if (*str_ptr == '+')
		{
			++str_ptr;
			if (!*str_ptr)
				goto finish;
		}

		while (*str_ptr >= '0' && *str_ptr <= '9')
		{
			e_right *= 10;
			e_right += *str_ptr - '0';
			++str_ptr;
		}
		goto finish;
	}

	template<typename char_type>
	float strtof(const char_type* nptr, char_type** endptr)
	{
		assert(nptr);

		const struct lconv* lc = localeconv();
		char decimal_point = *lc->decimal_point;

		float result = 0.0;
		long leftPart = 0;
		long rightPart = 0;
		int e_right = 0;
		int flags = 0; /* 1(-) 2(hex) 3(e) 4(e-)*/
		const char_type* str_ptr = __string_skip_spaces(nptr);
		if (*str_ptr == '-') {
			++str_ptr;
			flags |= 0x1;
		}
		if (!*str_ptr) goto end;
		if (*str_ptr == '0') {
			++str_ptr;
			if (!*str_ptr) goto end;
			if (*str_ptr == 'x' || *str_ptr == 'X') {
				flags |= 0x2;
				++str_ptr;
			}
			else if (*str_ptr == decimal_point) --str_ptr;
			else if (*str_ptr < '1' || *str_ptr > '9') goto end;
		}
		if (!*str_ptr) goto end;
		long part_2_count = 0;
		if (flags & 0x2) {
			int charNum = 0;
			const unsigned char* save_str_ptr = str_ptr;
			while (*str_ptr) {
				if ((*str_ptr >= 'a' && *str_ptr <= 'f') || (*str_ptr >= 'A' && *str_ptr <= 'F'))
					++charNum;
				else break;
				++str_ptr;
				if (!*str_ptr) break;
			}
			if (charNum) {
				str_ptr = save_str_ptr;
				int charNum2 = charNum;
				for (int i = 0; i < charNum; ++i) {
					int curr_char = str_ptr[i];
					if (curr_char < 'a') curr_char += 32;
					int curr_int = __stdlib_string_to_int_table_hex[curr_char - 97];
					leftPart += curr_int * (long)__stdlib_string_to_double_table_hex[charNum2 - 1];
					--charNum2;
				}
			}
		}
		else {
			while (*str_ptr >= '0' && *str_ptr <= '9') {
				leftPart *= 10;
				leftPart += *str_ptr - '0';
				++str_ptr;
				if (!*str_ptr) break;
			}
			if (*str_ptr == 'e' || *str_ptr == 'E') {
				flags |= 0x4;
				++str_ptr;
				goto do_e;
			}
			if (*str_ptr == decimal_point) {
				++str_ptr;
				if (!*str_ptr) goto finish;
				while (*str_ptr >= '0' && *str_ptr <= '9') {
					rightPart *= 10;
					rightPart += *str_ptr - '0';
					++str_ptr;
					++part_2_count;
				}
				if (*str_ptr == 'e' || *str_ptr == 'E') {
					flags |= 0x4;
					++str_ptr;
					goto do_e;
				}
			}
		}
	finish:;
		result = leftPart + ((float)rightPart * (float)__stdlib_string_to_double_table[part_2_count]);
		if (flags & 0x4) {
			float em = 1.f;
			for (int i = 0; i < e_right; ++i) {
				if (flags & 0x8) em *= 0.1f;
				else em *= 10.0f;
			}
			result = result * em;
		}
		result = (flags & 0x1) ? -result : result;
	end:;
		if (endptr) *endptr = (char_type*)str_ptr;
		return result;
	do_e:;
		if (!*str_ptr) goto finish;
		if (*str_ptr == '-') {
			flags |= 0x8;
			++str_ptr;
			if (!*str_ptr) goto finish;
		}
		else if (*str_ptr == '+') {
			++str_ptr;
			if (!*str_ptr) goto finish;
		}
		while (*str_ptr >= '0' && *str_ptr <= '9') {
			e_right *= 10;
			e_right += *str_ptr - '0';
			++str_ptr;
		}
		goto finish;
	}

#pragma warning(push)
#pragma warning(disable: 4244)
	// `buf` is a __CRT_dtoa_buffer if it called by __CRT_vsnprintf
	// I can use __CRT_itoa_buffer here for i1 and i2
	// mode:
	//  0 - normal lowercase, nan
	//  1 - normal uppercase, NAN
	//  2 - scientific lowercase
	//  3 - scientific uppercase
	template<typename char_type>
	void __CRT_dtoa(double val, char_type* buf, size_t bufSz, int mode)
	{
		if (isnan(val))
		{
			switch (mode)
			{
			default:
			case 0:
			case 2:
				buf[0] = 'n';
				buf[1] = 'a';
				buf[2] = 'n';
				break;
			case 3:
			case 1:
				buf[0] = 'N';
				buf[1] = 'A';
				buf[2] = 'N';
				break;
			}
			buf[3] = 0;
			return;
		}

		if (isinf(val))
		{
			switch (mode)
			{
			default:
			case 0:
			case 2:
				buf[0] = 'i';
				buf[1] = 'n';
				buf[2] = 'f';
				break;
			case 3:
			case 1:
				buf[0] = 'I';
				buf[1] = 'N';
				buf[2] = 'F';
				break;
			}
			buf[3] = 0;
			return;
		}

		int isMinus = 0;
		if (val < 0.0)
		{
			isMinus = 1;
			val *= -1.0; // now we need positive number
		}


		size_t bufInd = 0;
		if (isMinus)
		{
			buf[bufInd++] = '-';
			buf[bufInd] = 0;
		}



		double ipart = 0.0;
		double in = modf(val, &ipart);

		int numOfZeros = 0; // I don't khow how to do dtoa()
		// so, if I have (val=0.003), ipart will be 0
		// in will be 0.003
		// but down here I just make 2 integers, then I call ulltoa
		// and then combine 2 integers together.
		// In `in *= 10000000000000000;` I `move` right part (right from .) to
		//  the left. This is for i2. It's ok if val=123.5678, but it's bad
		//   when val have zeros after point, like 0.003. So result will be
		//   not "0.003", it will be "0.3". I need to put zeros after point.
		double in2 = in;
		if (in2 != 0.0)
		{
			for (int i = 0; i < 17; ++i)
			{
				in2 /= 0.1;
				if ((long long)in2 > 0.0)
				{
					break;
				}
				else
				{
					++numOfZeros;
				}
			}
		}

		in *= 10000000000000000;

		uint64_t i1 = (uint64_t)ipart;
		uint64_t i2 = (uint64_t)in;

		__CRT_ulltoa(i1, __CRT_dtoa_buffer, 30, 0);
		if (__CRT_dtoa_buffer[0])
		{
			for (size_t i = 0; i < 30; ++i)
			{
				if (!__CRT_dtoa_buffer[i])
					break;

				buf[bufInd++] = __CRT_dtoa_buffer[i];
				buf[bufInd] = 0;

				if (bufInd == bufSz)
					return;
			}
		}

		struct lconv* loc = localeconv();

		buf[bufInd++] = loc->decimal_point[0];
		buf[bufInd] = 0;

		if (numOfZeros)
		{
			for (int i = 0; i < numOfZeros; ++i)
			{
				buf[bufInd++] = '0';
				buf[bufInd] = 0;
				if (bufInd == bufSz)
					return;
			}
		}

		if (bufInd == bufSz)
			return;

		__CRT_ulltoa(i2, __CRT_dtoa_buffer, 30, 0);
		if (__CRT_dtoa_buffer[0])
		{
			for (int i = 0; i < 30; ++i)
			{
				if (!__CRT_dtoa_buffer[i])
					break;

				buf[bufInd++] = __CRT_dtoa_buffer[i];
				buf[bufInd] = 0;

				if (bufInd == bufSz)
					return;
			}
		}
		buf[bufInd] = 0;
	}
#pragma warning(pop)

	template<typename char_type>
	void __CRT_ftoa(float val, char_type* buf, size_t bufSz, int mode)
	{
		__CRT_dtoa(val, buf, bufSz, mode);
	}


	template<typename char_type>
	const char_type* __CRT_vprintf_getNumber(const char_type* fmt, int* number)
	{
		static char_type buffer[30];
		int i = 0;
		while (*fmt)
		{
			switch (*fmt)
			{
			case '0':case '1':case '2':case '3':case '4':
			case '5':case '6':case '7':case '8':case '9':
			{
				buffer[i] = *fmt;
				buffer[i + 1] = 0;
				++fmt;
				++i;
			}break;
			default:
				goto out;
			}
		}
	out:;

		if (buffer[0])
			*number = bq::_atoi(buffer);

		return fmt;
	}
	// Get valid specifier. That thing after %.
	// If not valid, return next char (skip %).
	// type:
	enum __CRT_vprintf_type {
		__CRT_vprintf_type_nothing = 0,
		__CRT_vprintf_type_c,
		__CRT_vprintf_type_lc,
		__CRT_vprintf_type_d_i,
		__CRT_vprintf_type_ld_li,
		__CRT_vprintf_type_lld_lli,
		__CRT_vprintf_type_x,
		__CRT_vprintf_type_X,
		__CRT_vprintf_type_o,
		__CRT_vprintf_type_s,
		__CRT_vprintf_type_ls,
		__CRT_vprintf_type_lls,
		__CRT_vprintf_type_f,
	};
	//  0 - nothing, skip
//  1 - c
//  2 - lc
//  3 - d\i
//  4 - ld\li
//  5 - lld\lli
//  6 - x
//  7 - X
//  8 - o
//  9 - s
//  10 - ls
//  11 - lls
//  12 - f
// leftPads: number of ' ' or '0' to write before writing the variable
// flags:
//   0x1 - pads with ' '
//   0x2 - pads with '0'
//   0x4 - '0x'
//   0x8 - '0X'
//   0x10 - '0' for ctal
	template<typename char_type>
	const char_type* __CRT_vprintf_(const char_type* fmt, int* type, int* leftPads, int* flags)
	{
		// *fmt is '%' so next char
		++fmt;

		const char_type* saveFmt = fmt;
		*type = __CRT_vprintf_type_nothing;
		*leftPads = 0;
		*flags = 0;

	begin:;
		switch (*fmt)
		{
		case '#':
		{
			++fmt;
			// #x #X #o
			switch (*fmt)
			{
			case 'x':
				*type = __CRT_vprintf_type_x;
				*flags |= 0x4;
				++fmt;
				break;
			case 'X':
				*type = __CRT_vprintf_type_X;
				*flags |= 0x8;
				++fmt;
				break;
			case 'o':
				*type = __CRT_vprintf_type_o;
				*flags |= 0x10;
				++fmt;
				break;
			}
		}break;
		case 'o':
			++fmt;
			*type = __CRT_vprintf_type_o;
			break;
		case 'x':
			++fmt;
			*type = __CRT_vprintf_type_x;
			break;
		case 'X':
			++fmt;
			*type = __CRT_vprintf_type_X;
			break;
		case '0':
		{
			++fmt;
			*flags |= 0x2;

			// now how much leftPads
			switch (*fmt)
			{
			case '1':case '2':case '3':case '4':
			case '5':case '6':case '7':case '8':case '9':
			{
				fmt = __CRT_vprintf_getNumber(fmt, leftPads);
				// now *fmt must be d\i\li\lli\ld\lld or other...
				// easy to use goto
				goto begin;
			}break;
			default:
				break;
			}
		}break;
		case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
		{
			// need to get number
			fmt = __CRT_vprintf_getNumber(fmt, leftPads);

			// it can be pads with ' '
			*flags |= 0x1;

			// next fmt can be '.'
			switch (*fmt)
			{
			case '.':
				break;
			default: // no, probably *fmt is c d i or other
				goto begin;
				break;
			}

		}break;
		case 's':
		{
			++fmt;
			*type = __CRT_vprintf_type_s;
		}break;
		case 'c':
		{
			*type = __CRT_vprintf_type_c;
			++fmt;
		}break;
		case 'i':
		case 'd':
		{
			*type = __CRT_vprintf_type_d_i;
			++fmt;
		}break;
		case 'l':
		{
			// li\ld lli\lld lc ls

			++fmt;
			if (*fmt)
			{
				switch (*fmt)
				{
				case 's':
					*type = __CRT_vprintf_type_ls;
					++fmt;
					break;
				case 'c':
					*type = __CRT_vprintf_type_lc;
					++fmt;
					break;
				case 'd':
				case 'i':
					*type = __CRT_vprintf_type_ld_li;
					++fmt;
					break;
				case 'l':
				{
					++fmt;
					if (*fmt)
					{
						switch (*fmt)
						{
						case 's':
							*type = __CRT_vprintf_type_lls;
							++fmt;
							break;
						case 'd':
						case 'i':
							*type = __CRT_vprintf_type_lld_lli;
							++fmt;
							break;
						}
					}
				}break;
				default:
					break;
				}
			}
		}break;
		case 'f':
		{
			*type = __CRT_vprintf_type_f;
			++fmt;
		}break;
		default:
			break;
		}

		if (*type == 0)
			fmt = saveFmt;

		return fmt;
	}

	template<typename char_base, typename char_target>
	int __vfprintf_put_string(const char_base* arg_constChar, int leftPad, int flags, bqStream* stream)
	{
		int written_num = 0;
		size_t argLen = bqCRT::strlen(arg_constChar);
		if (argLen)
		{
			if (leftPad)
			{
				if ((flags & 0x1) || (flags & 0x2))
				{
					int leftPadNum = leftPad - argLen;
					if (leftPadNum > 0)
					{
						char leftPadChar = ' ';
						if (flags & 0x2)
							leftPadChar = '0';
						// copy of code below
						for (int i = 0; i < leftPadNum; ++i)
						{
							written_num += bqCRT::fputc(leftPadChar, stream);
							{
								auto _pos = stream->Tell();
								bqCRT::fputc((char)0, stream);
								stream->Seek(_pos, SEEK_SET);
							}
						}
					}
				}
			}

			for (size_t i2 = 0; i2 < argLen; ++i2)
			{
				char_target __c = (char_target)arg_constChar[i2];
				written_num += bqCRT::fputc(__c, stream);
				{
					auto _pos = stream->Tell();
					bqCRT::fputc((char)0, stream);
					stream->Seek(_pos, SEEK_SET);
				}
			}
		}
		return written_num;
	}


	template<typename char_type>
	int vfprintf(bqStream* stream, const char_type* format, va_list ap)
	{
		int written_num = 0;
		if (format)
		{
			size_t format_len = bq::strlen(format);
			if (format_len)
			{
				//int buffer_current_size = 0;
				const char* arg_constChar = 0;
				const wchar_t* arg_constWchar = 0;
				const char32_t* arg_constU32char = 0;

				int type = __CRT_vprintf_type_nothing;
				int flags = 0;
				int leftPad = 0;

				const char_type* fmt = format;
				while (*fmt)
				{
					type = 0;
					flags = 0;
					leftPad = 0;

					switch (*fmt)
					{
					case '%':
						fmt = __CRT_vprintf_(fmt, &type, &leftPad, &flags);
						if (!type)
						{
							//	s[buffer_current_size++] = *fmt; // must be '%'
							//	s[buffer_current_size] = 0;
							written_num += bqCRT::fputc('%', stream);
							
							auto _pos = stream->Tell();
							bqCRT::fputc((char)0, stream);
							stream->Seek(_pos, SEEK_SET);

							++fmt;
						}
						break;
					default:
						written_num += bqCRT::fputc(*fmt, stream);
						{
							auto _pos = stream->Tell();
							bqCRT::fputc((char_type)0, stream);
							stream->Seek(_pos, SEEK_SET);
						}

						++fmt;
						break;
					}

					if (type)
					{
						arg_constU32char = 0;
						arg_constChar = 0;
						arg_constWchar = 0;

						switch (type)
						{
						case __CRT_vprintf_type_c:
						case __CRT_vprintf_type_lc:
						{
							int c = va_arg(ap, int);
							__CRT_itoa_buffer[0] = (char)c;
							__CRT_itoa_buffer[1] = 0;
							if (leftPad && (flags & 0x1))
							{
								flags = 0;
								int leftPadNum = leftPad - 1;
								if (leftPadNum)
								{
									//__CRT_itoa_buffer[leftPadNum] = (char)c;
									//__CRT_itoa_buffer[leftPadNum + 1] = 0;
									for (int i = 0; i < leftPadNum; ++i)
									{
										//__CRT_itoa_buffer[i] = ' ';
										written_num += bqCRT::fputc(' ', stream);
										{
											auto _pos = stream->Tell();
											bqCRT::fputc((char)0, stream);
											stream->Seek(_pos, SEEK_SET);
										}
									}
								}
							}

							arg_constChar = __CRT_itoa_buffer;
						}break;
						case __CRT_vprintf_type_d_i:
						{
							int arg = va_arg(ap, int);
							__CRT_itoa(arg, __CRT_itoa_buffer, 30, 0);
							arg_constChar = __CRT_itoa_buffer;
						}break;
						case __CRT_vprintf_type_ld_li:
						{
							long int arg = va_arg(ap, long int);
							__CRT_itoa(arg, __CRT_itoa_buffer, 30, 0);
							arg_constChar = __CRT_itoa_buffer;
						}break;
						case __CRT_vprintf_type_lld_lli:
						{
							long long int arg = va_arg(ap, long long int);
							__CRT_lltoa(arg, __CRT_itoa_buffer, 30, 0);
							arg_constChar = __CRT_itoa_buffer;
						}break;
						case __CRT_vprintf_type_x: //x
						{
							unsigned int arg = va_arg(ap, unsigned int);
							__CRT_uitoa(arg, __CRT_itoa_buffer, 30, 1);
							arg_constChar = __CRT_itoa_buffer;
							if (flags & 0x4)
							{
								bqCRT::fputc('0', stream);
								{
									auto _pos = stream->Tell();
									bqCRT::fputc((char)0, stream);
									stream->Seek(_pos, SEEK_SET);
								}

								bqCRT::fputc('x', stream);
								{
									auto _pos = stream->Tell();
									bqCRT::fputc((char)0, stream);
									stream->Seek(_pos, SEEK_SET);
								}
							}
						}break;
						case __CRT_vprintf_type_X: //X
						{
							unsigned int arg = va_arg(ap, unsigned int);
							__CRT_uitoa(arg, __CRT_itoa_buffer, 30, 2);
							arg_constChar = __CRT_itoa_buffer;
							if (flags & 0x8)
							{
								bqCRT::fputc('0', stream);
								{
									auto _pos = stream->Tell();
									bqCRT::fputc((char)0, stream);
									stream->Seek(_pos, SEEK_SET);
								}

								bqCRT::fputc('X', stream);
								{
									auto _pos = stream->Tell();
									bqCRT::fputc((char)0, stream);
									stream->Seek(_pos, SEEK_SET);
								}
							}
						}break;
						case __CRT_vprintf_type_o: //o
						{
							unsigned int arg = va_arg(ap, unsigned int);
							__CRT_uitoa(arg, __CRT_itoa_buffer, 30, 3);
							arg_constChar = __CRT_itoa_buffer;
							if (flags & 0x10)
							{
								bqCRT::fputc('0', stream);
								{
									auto _pos = stream->Tell();
									bqCRT::fputc((char)0, stream);
									stream->Seek(_pos, SEEK_SET);
								}
							}
						}break;
						case __CRT_vprintf_type_f:
						{
							double arg = va_arg(ap, double);
							__CRT_dtoa(arg, __CRT_itoa_buffer, 30, 0);
							arg_constChar = __CRT_itoa_buffer;
						}break;
						case __CRT_vprintf_type_s: //s
						{
							arg_constChar = va_arg(ap, char*);
						}break;
						case __CRT_vprintf_type_ls:
						{
							arg_constWchar = va_arg(ap, wchar_t*);
							//if (arg_constWchar)
							//	xxfwprintf(stream, "%s", arg_constWchar);
						}break;
						case __CRT_vprintf_type_lls:
						{
							arg_constU32char = va_arg(ap, char32_t*);
							//if (arg_constU32char)
							//	xxfwprintf(stream, "%s", arg_constWchar);
						}break;
						}


						if (arg_constChar)
						{
							written_num += __vfprintf_put_string<char, char_type>(arg_constChar, leftPad, flags, stream);
							//size_t argLen = bqCRT::strlen(arg_constChar);
							//if (argLen)
							//{
							//	if (leftPad)
							//	{
							//		if ((flags & 0x1) || (flags & 0x2))
							//		{
							//			int leftPadNum = leftPad - argLen;
							//			if (leftPadNum > 0)
							//			{
							//				char leftPadChar = ' ';
							//				if (flags & 0x2)
							//					leftPadChar = '0';
							//				// copy of code below
							//				for (int i = 0; i < leftPadNum; ++i)
							//				{
							//					written_num += bqCRT::fputc(leftPadChar, stream);
							//					{
							//						auto _pos = stream->Tell();
							//						bqCRT::fputc((char)0, stream);
							//						stream->Seek(_pos, SEEK_SET);
							//					}
							//				}
							//			}
							//		}
							//	}

							//	for (size_t i2 = 0; i2 < argLen; ++i2)
							//	{
							//		char_type __c = (char_type)arg_constChar[i2];
							//		written_num += bqCRT::fputc(__c, stream);
							//		{
							//			auto _pos = stream->Tell();
							//			bqCRT::fputc((char)0, stream);
							//			stream->Seek(_pos, SEEK_SET);
							//		}
							//	}
							//}
						}
						if (arg_constWchar)
							written_num += __vfprintf_put_string<wchar_t, char_type>(arg_constWchar, leftPad, flags, stream);
						if (arg_constU32char)
							written_num += __vfprintf_put_string<char32_t, char_type>(arg_constU32char, leftPad, flags, stream);

					}
				}
				return written_num ? written_num : EDOM;
			}
		}
		return written_num;
	}

	template<typename char_type>
	int strcmp(const char_type* s1, const char_type* s2, int limit)
	{
		int limit_counter = 0;
		int result = 1;
		while (1)
		{
			if (*s1 && *s2)
			{
				if (*s1 < *s2)
					return -1;

				if (*s1 > *s2)
					return 1;
			}
			++s1;
			++s2;
			result = 0;

			if (limit)
			{
				++limit_counter;
				if (limit_counter == limit)
				{
					return result;
				}
			}

			if (!*s1 && !*s2)
				break;
			else if (!*s1 && *s2)
				return -1;
			else if (*s1 && !*s2)
				return 1;
		}

		return result;
	}
}
int bqCRT::atoi(const char* nptr) { return bq::_atoi(nptr); }
int bqCRT::atoi(const wchar_t* nptr) { return bq::_atoi(nptr); }
int bqCRT::atoi(const char32_t* nptr) { return bq::_atoi(nptr); }
size_t bqCRT::strlen(const char* s) { return bq::strlen(s); }
size_t bqCRT::strlen(const wchar_t* s) { return bq::strlen(s); }
size_t bqCRT::strlen(const char32_t* s) { return bq::strlen(s); }

int bqCRT::fprintf(bqStream* stream, const char* format, ...) 
{
	va_list ap;
	va_start(ap, format);
	int r = bqCRT::vfprintf(stream, format, ap);
	va_end(ap);
	return r;
}
int bqCRT::fprintf(bqStream* stream, const wchar_t* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int r = bqCRT::vfprintf(stream, format, ap);
	va_end(ap);
	return r;
}
int bqCRT::fprintf(bqStream* stream, const char32_t* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int r = bqCRT::vfprintf(stream, format, ap);
	va_end(ap);
	return r;
}

int bqCRT::vfprintf(bqStream* stream, const char* format, va_list arg) 
{
	return bq::vfprintf(stream, format, arg);
}
int bqCRT::vfprintf(bqStream* stream, const wchar_t* format, va_list arg)
{
	return bq::vfprintf(stream, format, arg);
}
int bqCRT::vfprintf(bqStream* stream, const char32_t* format, va_list arg)
{
	return bq::vfprintf(stream, format, arg);
}

int bqCRT::fputc(char character, bqStream* stream)
{
	int r = EOF;
	if (stream->m_position < stream->m_size)
	{
		stream->m_buffer[stream->m_position] = character;
		r = character;
		++stream->m_position;
	}
	return r;
}
int bqCRT::fputc(wchar_t character, bqStream* stream)
{
	int r = EOF;
	size_t sz = sizeof(character);

	if ((stream->m_size - stream->m_position) < sz)
		return r;

	uint8_t* b = (uint8_t*)&character;
	for (size_t i = 0; i < sz; ++i)
	{
		if (stream->m_position < stream->m_size)
		{
			stream->m_buffer[stream->m_position] = b[i];
			++stream->m_position;
		}
		else
		{
			// до данного места не должно доходить
			break;
		}
	}
	r = character;
	return r;
}
int bqCRT::fputc(char32_t character, bqStream* stream)
{
	int r = EOF;

	if ((stream->m_size - stream->m_position) < 4)
		return r;

	uint8_t* b = (uint8_t*)&character;
	
	stream->m_buffer[stream->m_position] = b[0];
	stream->m_buffer[stream->m_position+1] = b[1];
	stream->m_buffer[stream->m_position+2] = b[2];
	stream->m_buffer[stream->m_position+3] = b[3];
	stream->m_position += 4;

	r = character;
	return r;
}

int bqCRT::islower(char c)
{
	if (c >= 'a' && c <= 'z')
		return 1;
	return 0;
}
int bqCRT::islower(wchar_t c)
{
	if (c >= 'a' && c <= 'z')
		return 1;
	return 0;
}
int bqCRT::islower(char32_t c)
{
	if (c >= 'a' && c <= 'z')
		return 1;
	return 0;
}
int bqCRT::isupper(char c)
{
	if (c >= 'A' && c <= 'Z')
		return 1;
	return 0;
}
int bqCRT::isupper(wchar_t c)
{
	if (c >= 'A' && c <= 'Z')
		return 1;
	return 0;
}
int bqCRT::isupper(char32_t c)
{
	if (c >= 'A' && c <= 'Z')
		return 1;
	return 0;
}
char bqCRT::tolower(char c)
{
	return 0;
}
wchar_t bqCRT::tolower(wchar_t c)
{
	return 0;
}
char32_t bqCRT::tolower(char32_t c)
{
	return 0;
}
char bqCRT::toupper(char c)
{
	return 0;
}
wchar_t bqCRT::toupper(wchar_t c)
{
	return 0;
}
char32_t bqCRT::toupper(char32_t c)
{
	return 0;
}
int bqCRT::strcmp(const char* str, int limit, const char* other)
{
	return bq::strcmp(str, other, limit);
}
int bqCRT::strcmp(const wchar_t* str, int limit, const wchar_t* other)
{
	return bq::strcmp(str, other, limit);
}
int bqCRT::strcmp(const char32_t* str, int limit, const char32_t* other)
{
	return bq::strcmp(str, other, limit);
}
