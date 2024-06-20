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

#ifndef _BQ_SOUNDSTREAMFILEWAV_H_
#define _BQ_SOUNDSTREAMFILEWAV_H_

class bqSoundStreamFileWAV : public bqSoundStreamFile
{
	FILE* m_file = 0;
	uint32_t m_dataSize = 0;
	uint32_t m_firstDataBlock = 0;
	uint32_t m_lastDataBlock = 0;
	uint32_t m_currentDataBlock = 0;
public:
	bqSoundStreamFileWAV();
	virtual ~bqSoundStreamFileWAV();

	virtual size_t Read(void* buffer, size_t size) override;
	virtual void MoveToFirstDataBlock() override;
	virtual long Tell() override;
	virtual void Seek(long) override;
	virtual bool eof() override;

	bool Open(const char*);
	void Close();
};

#endif
