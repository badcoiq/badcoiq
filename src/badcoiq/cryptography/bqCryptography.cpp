/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
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
#include "badcoiq/cryptography/bqCryptography.h"

#include "md5.h"

bqMD5 bqCryptography::MD5(const void* b, uint32_t sz)
{
	bqMD5 md5;

	MD5Context ctx;
	md5Init(&ctx);
	md5Update(&ctx, (uint8_t*)b, sz);
	md5Finalize(&ctx);

	memcpy(&md5.m_a, ctx.digest, 16);
    
	return md5;
}

bool bqCryptography::Compare(const bqMD5& a, const bqMD5& b)
{
	if (a.m_a != b.m_a)
		return false;
	if (a.m_b != b.m_b)
		return false;
	if (a.m_c != b.m_c)
		return false;
	if (a.m_d != b.m_d)
		return false;
	return true;
}

bool bqCryptography::Valid(const bqMD5& md5)
{
	if (md5.m_a) return true;
	if (md5.m_b) return true;
	if (md5.m_c) return true;
	if (md5.m_d) return true;
	return false;
}
