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

#include <filesystem>

void to_inl(const char* inFile, const char* outFile)
{
	FILE* inF = 0;
	fopen_s(&inF, inFile, "rb");
	if (inF)
	{
		FILE* outF = 0;
		fopen_s(&outF, outFile, "wb");
		if (outF)
		{
			std::uintmax_t inFileSz = std::filesystem::file_size(inFile);
			if (inFileSz)
			{
				uint8_t* fileData = (uint8_t*)malloc((size_t)inFileSz);
				if (fileData)
				{
					fread(fileData, (size_t)inFileSz, 1, inF);

					int byte_counter = 0;
					for (uint32_t i = 0; i < inFileSz; ++i)
					{
						if (!byte_counter)
							fprintf(outF, "\t");

						fprintf(outF, "0x%02X, ", ((uint8_t*)fileData)[i]);

						++byte_counter;
						if (byte_counter == 8)
						{
							fprintf(outF, "\r\n");
							byte_counter = 0;
						}
					}

					free(fileData);
				}
			}
			fclose(outF);
		}

		fclose(inF);
	}
}

int main()
{
	std::filesystem::create_directory("../src/_data/");
	to_inl("../data/fonts/font.png", "../src/_data/font.inl");
	to_inl("../data/fonts/defaultIcons.png", "../src/_data/defaultIcons.inl");
	return 0;
}