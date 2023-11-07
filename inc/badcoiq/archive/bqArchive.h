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
#ifndef __BQ_ARCHIVE_H__
#define __BQ_ARCHIVE_H__

#include "badcoiq/string/bqString.h"
#include "badcoiq/containers/bqArray.h"

// Имя библиотеки_алгоритм, или просто имя если в ней один алгоритм
//     megalib_deflate,
//     megalib_lz77,
//     megalib_lzma,
// Имеется:
//     -   fastlz (LZ77)
enum class bqCompressorType : uint32_t
{
    // m_level:
    //  1 скорость
    //  2 сжатие
    fastlz
};

// Информация для сжатия и расжатия
struct bqCompressionInfo
{
    bqCompressorType m_compressorType = bqCompressorType::fastlz;

    // Когда надо сжать данные
    // Установи размер данных и указатель на данные.
    // после вызова Compress m_sizeCompressed и m_dataCompressed
    // будут иметь значения
    uint32_t m_sizeUncompressed = 0;
    uint8_t* m_dataUncompressed = 0;

    // Когда надо расжать
    // Устанавливаем размер сжатых данных, указатель на сжатые данные.
    // Так-же должен знать m_sizeUncompressed и надо выделить память для распакованных
    // данных в m_dataUncompressed
    // После вызова Decompress m_dataUncompressed будет иметь расжатые данные
    uint32_t m_sizeCompressed = 0;
    uint8_t* m_dataCompressed = 0;

    // Смотри CompressorType. Если значение не верно
    // будет использовано значение по умолчанию.
    uint32_t m_level = 2;
};

// Структура с описанием файла архива.
// Путь к файлу и специфичные данные
struct bqArchiveZipFile
{
    bqStringA m_fileName;
    void* m_implementation = 0;
};

class bqArchiveSystem
{
public:
    // Добавить файл из которого будет происходить распаковка
    static bqArchiveZipFile* ZipAdd(const char* zipFile);
    static void ZipRemove(bqArchiveZipFile*);

    // Получить список файлов в указанном архиве
    static void GetFileList(bqArchiveZipFile*, bqArray<bqStringA>&);

    // Распаковать файл. Если `a` NULL первый найденный файл будет распакован.
    // Установи `a` если используешь много zip файлов которые содержат файлы с одинаковым именем.
    // Функция выделит память используя bqMemory::malloc, и запишет размер в size
    // Если всё OK то вернётся указатель.
    // Используй bqMemory::free для освобождения памяти
    static uint8_t* ZipUnzip(const char* fileInZip, uint32_t* size, bqArchiveZipFile* a = NULL);

    // Установи все данные в `info`. Если сжатие было OK, вернётся true
    // и m_dataCompressed будет иметь адрес
    // Надо вызвать bqMemory::free(m_dataCompressed) для освобождения памяти
    static bool Compress(bqCompressionInfo* info);

    // Подобно методу `Compress`
    // Выделяй память для m_dataUncompressed самостоятельно.
    // Ты должен знать m_sizeUncompressed.
    static bool Decompress(bqCompressionInfo* info);

    // Расжать данные и сунуть всё в массив
    // Если info.m_dataCompressed NULL (или !info.m_sizeCompressed)
    // тогда функция попробует копировать данные из info.m_dataUncompressed
    // Функция выделит память, вставит все данные в массив и освободит память.
    // info.m_dataUncompressed и info.m_sizeUncompressed будут проигнорированы
    static bool Decompress(bqCompressionInfo* info, bqArray<uint8_t>& toArray);
};

#endif

