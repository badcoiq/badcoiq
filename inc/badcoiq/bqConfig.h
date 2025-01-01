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

/// \file bqConfig.h
/// \brief Настройка компиляции.
/// 
/// Можно выбрать что нужно или ненужно компилировать.
///

#pragma once
#ifndef __BQ_BADCOIQCONFIG_H__
/// \cond
#define __BQ_BADCOIQCONFIG_H__
/// \endcond

/// Компилировать с физикой
#define BQ_WITH_PHYSICS

/// Компилировать со звуком
#define BQ_WITH_SOUND

#if defined(BQ_WITH_SOUND)
/// Использовать opus (если используется BQ_WITH_SOUND)
#define BQ_WITH_OGGVORBIS_OPUS
#endif

/// Использовать bqImage и текстуры
#define BQ_WITH_IMAGE

#ifdef BQ_WITH_IMAGE
/// Компилировать с GUI  (если используется BQ_WITH_IMAGE)
#define BQ_WITH_GUI
/// Чтение .bmp  (если используется BQ_WITH_IMAGE)
#define BQ_WITH_IMAGE_BMP

/// Чтение .png. Для GUI нужен PNG. (если используется BQ_WITH_IMAGE)
#define BQ_WITH_IMAGE_PNG

/// Чтение .jpg  (если используется BQ_WITH_IMAGE)
/// Если используется, то разработчик обязан соблюсти условия
/// описанных в src\jpeglib\README
/// 
/// Особое внимание этому пункту:
/// 
/// (2) If only executable code is distributed, then the accompanying
/// documentation must state that "this software is based in part on the work of
/// the Independent JPEG Group".
//#define BQ_WITH_IMAGE_JPG

/// Чтение .tga  (если используется BQ_WITH_IMAGE)
#define BQ_WITH_IMAGE_TGA
#endif

/// Поддержка сжатия и расжатия данных и прочих связанных вещей
#define BQ_WITH_ARCHIVE

/// Поддержка 3Д моделей
#define BQ_WITH_MESH

/// Поддержка создания обычного системного окна.
/// Может быть такое что приложение нужно без GUI и окна,
/// тогда можно убрать окно и всё что с ним связано совсем.
#define BQ_WITH_WINDOW

#if defined(BQ_WITH_IMAGE) && defined(BQ_WITH_WINDOW)

/// Поддержка GPU для рисования. Нужно BQ_WITH_WINDOW.
#define BQ_WITH_GS
#endif

#if defined(BQ_WITH_IMAGE) && defined(BQ_WITH_GS)

/// Поддержка спрайтов. Нужно BQ_WITH_IMAGE (потому 
/// что текстуру без картинки не создать) и BQ_WITH_GS (чтобы рисовать спрайт)
#define BQ_WITH_SPRITE
#endif

/// Поддержка объектов сцены. Пока сцена не проработана.
#define BQ_WITH_SCENE

#endif

