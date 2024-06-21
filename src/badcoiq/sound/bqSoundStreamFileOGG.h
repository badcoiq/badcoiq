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
#ifndef _BQ_SOUNDSTREAMFILEOGG_H_
#define _BQ_SOUNDSTREAMFILEOGG_H_

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <opus.h>
#include <opusfile.h>

size_t _oggvorbis_fread(void* buffer, size_t es, size_t ec, void* _f);
int _oggvorbis_fseek(void* _f, ogg_int64_t o, int s);
long _oggvorbis_ftell(void* _f);

int _oggopus_fread(void* _stream, unsigned char* _ptr, int _nbytes);
int _oggopus_fseek(void* _stream, opus_int64 _offset, int _whence);
opus_int64 _oggopus_ftell(void* _stream);

class bqSoundStreamFileOGG : public bqSoundStreamFile
{
	OggVorbis_File m_vorbisFile;
	ov_callbacks m_vorbisCallbacks =
	{
		_oggvorbis_fread,
		_oggvorbis_fseek,
		0,
		_oggvorbis_ftell
	};
	int m_bitstreamCurrentSection = 0;
	uint8_t m_ovReadBuffer[4096];
	uint32_t m_ovReadBufferSize = 0;

	OggOpusFile* m_opusFile = 0;
	OpusFileCallbacks m_opusCallbacks =
	{
		_oggopus_fread,
		_oggopus_fseek,
		_oggopus_ftell,
		0,
	};
	size_t _readOpus(void* buffer, size_t size);
	float m_opReadBuffer[0x5000];

	bool m_eof = false;
	uint8_t m_outBuffer[4096];
	uint32_t m_outBufferSize = 0;

	bool _OpenVorbis(const char*);
	bool _OpenOpus(const char*);
public:
	bqSoundStreamFileOGG();
	virtual ~bqSoundStreamFileOGG();

	virtual size_t Read(void* buffer, size_t size) override;
	virtual void MoveToFirstDataBlock() override;
	virtual long Tell() override;
	virtual void Seek(long) override;
	virtual bool eof() override;

	bool Open(const char*);
	void Close();

	FILE* m_file = 0;
};

#endif
