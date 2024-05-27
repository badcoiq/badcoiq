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

#pragma once
#ifndef __BQ_BADCOIQCONFIG_H__
#define __BQ_BADCOIQCONFIG_H__

// Компилировать со звуком
#define BQ_WITH_SOUND

// Использовать bqImage и текстуры
#define BQ_WITH_IMAGE

#ifdef BQ_WITH_IMAGE

// Компилировать с GUI
#define BQ_WITH_GUI

#define BQ_WITH_IMAGE_BMP

// Для GUI нужен PNG
#define BQ_WITH_IMAGE_PNG

#define BQ_WITH_IMAGE_JPG
#define BQ_WITH_IMAGE_TGA
#endif

#define BQ_WITH_ARCHIVE
#define BQ_WITH_MESH

#define BQ_WITH_WINDOW
#if defined(BQ_WITH_IMAGE) && defined(BQ_WITH_WINDOW)
#define BQ_WITH_GS
#endif

#if defined(BQ_WITH_IMAGE) && defined(BQ_WITH_GS)
#define BQ_WITH_SPRITE
#endif

#define BQ_WITH_SCENE

#endif

