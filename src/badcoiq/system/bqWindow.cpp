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

#include "badcoiq/system/bqWindow.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/input/bqInput.h"

#ifdef BQ_PLATFORM_WINDOWS
#include "badcoiq/system/bqWindowWin32.h"
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// Нахождение текущего размера окна (клиентской области)
void bqWindowWin32_findCurrentSize(bqWindow* w, bqWindowWin32* w32)
{
    RECT rc;
    GetClientRect(w32->m_hWnd, &rc);
    w->GetData()->m_sizeCurrent.x = rc.right - rc.left;
    w->GetData()->m_sizeCurrent.y = rc.bottom - rc.top;
}
#endif


bqWindow::bqWindow(bqWindowCallback* cb)
{
#ifdef BQ_PLATFORM_WINDOWS
    // Выделю память для ОС данных
    bqWindowWin32* w32 = (bqWindowWin32*)bqMemory::malloc(sizeof(bqWindowWin32));
    // Сохраню адрес
    m_data.m_implementation = w32;

    // Обычные WinAPI действия
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(0);
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;

    // нужен уникальный lpszClassName
    static int windowCount = 0;
    wsprintf(w32->m_className, L"w%i", windowCount++);
    wcex.lpszClassName = w32->m_className;
    wcex.hIconSm = 0;
    RegisterClassExW(&wcex);

    w32->m_hWnd = CreateWindowExW(
        0,
        w32->m_className,
        L"bqWindow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        0,
        0,
        wcex.hInstance,
        0
    );

    // сохраню стиль окна
    w32->m_style = GetWindowLongPtr(w32->m_hWnd, GWL_STYLE);

    // получаю размер рамки
    int padding = GetSystemMetrics(SM_CXPADDEDBORDER);
    m_data.m_borderSize.x = GetSystemMetrics(SM_CXFRAME) + padding;
    m_data.m_borderSize.y = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + padding);

    bqWindowWin32_findCurrentSize(this, w32);

    SetWindowLongPtr(w32->m_hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));
#endif

    m_data.m_cb = cb;
}

bqWindow::~bqWindow()
{
    if (m_data.m_implementation)
    {
#ifdef BQ_PLATFORM_WINDOWS
        bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
        if (w32->m_hWnd)
        {
            DestroyWindow((HWND)w32->m_hWnd);
            UnregisterClass(w32->m_className, GetModuleHandle(0));
        }
#endif

        bqMemory::free(m_data.m_implementation);
    }
}

void bqWindow::SetTitle(const char* s)
{
    BQ_ASSERT_ST(s);
    BQ_ASSERT_ST(m_data.m_implementation);
#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    if (w32->m_hWnd)
        SetWindowTextA(w32->m_hWnd, s);
#endif
}

void bqWindow::SetVisible(bool v)
{
    BQ_ASSERT_ST(m_data.m_implementation);
    m_data.m_isVisible = v;
#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    if (w32->m_hWnd)
        ShowWindow(w32->m_hWnd, v ? SW_SHOW : SW_HIDE);
#endif
}


void bqWindow::Maximize()
{
    BQ_ASSERT_ST(m_data.m_implementation);
    m_data.m_isVisible = true;
#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    ShowWindow(w32->m_hWnd, SW_MAXIMIZE);
#endif
}

void bqWindow::Minimize()
{
    BQ_ASSERT_ST(m_data.m_implementation);
    m_data.m_isVisible = false;
#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    ShowWindow(w32->m_hWnd, SW_MINIMIZE);
#endif
}

void bqWindow::Restore()
{
    BQ_ASSERT_ST(m_data.m_implementation);
    m_data.m_isVisible = true;
#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    ShowWindow(w32->m_hWnd, SW_RESTORE);
#endif
}

void bqWindow::SetBorderless(bool v)
{
    BQ_ASSERT_ST(m_data.m_implementation);

#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    if (v)
        SetWindowLongPtr(w32->m_hWnd, GWL_STYLE, WS_POPUP);
    else
        SetWindowLongPtr(w32->m_hWnd, GWL_STYLE, w32->m_style);
    SetWindowPos(w32->m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    ShowWindow(w32->m_hWnd, SW_NORMAL);
#endif
}

void bqWindow::SetNoResize(bool v)
{
    BQ_ASSERT_ST(m_data.m_implementation);

#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    if (v)
    {
        w32->m_style &= ~WS_THICKFRAME;
        w32->m_style &= ~WS_MAXIMIZEBOX;
    }
    else
        w32->m_style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    SetWindowLongPtr(w32->m_hWnd, GWL_STYLE, w32->m_style);
    SetWindowPos(w32->m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    ShowWindow(w32->m_hWnd, SW_NORMAL);
#endif
}

void bqWindow::SetNoMinimize(bool v)
{
    BQ_ASSERT_ST(m_data.m_implementation);

#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    if (v)
    {
        w32->m_style &= ~WS_MINIMIZEBOX;
    }
    else
        w32->m_style |= WS_MINIMIZEBOX;
    SetWindowLongPtr(w32->m_hWnd, GWL_STYLE, w32->m_style);
    SetWindowPos(w32->m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    ShowWindow(w32->m_hWnd, SW_NORMAL);
#endif
}

void bqWindow::SetPositionAndSize(int x, int y, int sx, int sy)
{
    BQ_ASSERT_ST(m_data.m_implementation);
#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;

    SetWindowPos(w32->m_hWnd, 0, x, y, sx, sy, 0);
    bqWindowWin32_findCurrentSize(this, w32);
#endif
}

void bqWindow::ToFullscreenMode()
{
    BQ_ASSERT_ST(m_data.m_implementation);
    if (m_data.m_isFullscreen)
        return;

#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    w32->m_stylePreFullscreen = GetWindowLong(w32->m_hWnd, GWL_STYLE);
    MONITORINFO mi = { sizeof(mi) };

    if (GetWindowPlacement(w32->m_hWnd, &w32->m_wndPlcmnt) &&
        GetMonitorInfo(MonitorFromWindow(w32->m_hWnd, MONITOR_DEFAULTTOPRIMARY), &mi))
    {
        SetWindowLong(w32->m_hWnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(w32->m_hWnd, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        m_data.m_isFullscreen = true;
        ShowWindow(w32->m_hWnd, SW_NORMAL);

        bqWindowWin32_findCurrentSize(this, w32);
    }
#endif
}

void bqWindow::ToWindowMode()
{
    BQ_ASSERT_ST(m_data.m_implementation);
    if (!m_data.m_isFullscreen)
        return;
#ifdef BQ_PLATFORM_WINDOWS
    bqWindowWin32* w32 = (bqWindowWin32*)m_data.m_implementation;
    SetWindowLong(w32->m_hWnd, GWL_STYLE, w32->m_stylePreFullscreen);

    SetWindowPlacement(w32->m_hWnd, &w32->m_wndPlcmnt);
    SetWindowPos(w32->m_hWnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    m_data.m_isFullscreen = false;
    ShowWindow(w32->m_hWnd, SW_NORMAL);

    bqWindowWin32_findCurrentSize(this, w32);
#endif
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    bqWindow* pW = reinterpret_cast<bqWindow*>(GetWindowLongPtr(hWnd, GWL_USERDATA));

    switch (message)
    {
    case WM_ACTIVATE:
    {
        switch (wmId)
        {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
        {
            if (pW)
                pW->GetData()->m_cb->OnActivate(pW);
            return DefWindowProc(hWnd, message, wParam, lParam);
        }break;
        case WA_INACTIVE:
            if (pW)
                pW->GetData()->m_cb->OnDeactivate(pW);
            break;
        }
        break;
    }
    case WM_GETMINMAXINFO:
    {
        if (pW)
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = pW->GetSizeMinimum()->x;
            lpMMI->ptMinTrackSize.y = pW->GetSizeMinimum()->y;
        }
    }
    break;
    case WM_SIZE:
    {
        if (pW)
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case SIZE_MINIMIZED:
                pW->GetData()->m_cb->OnMinimize(pW);
                break;
            case SIZE_RESTORED:
                pW->GetData()->m_cb->OnRestore(pW);
                break;
            case SIZE_MAXIMIZED:
                pW->GetData()->m_cb->OnMaximize(pW);
                break;
            }

            bqWindowWin32_findCurrentSize(pW, (bqWindowWin32*)pW->GetData()->m_implementation);

            pW->GetData()->m_cb->OnSize(pW);
        }

    }return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SIZING:
    {
        if (pW)
        {
            bqWindowWin32_findCurrentSize(pW, (bqWindowWin32*)pW->GetData()->m_implementation);

            pW->GetData()->m_cb->OnSizing(pW);
        }
    }break;

    case WM_ERASEBKGND:
    case WM_PAINT:
    {
        if (pW)
            pW->GetData()->m_cb->OnDraw(pW);
    }break;
    case WM_MOVE:
    {
        if (pW)
            pW->GetData()->m_cb->OnMove(pW);
    }break;
   
    case WM_CLOSE:
        if (pW)
            pW->GetData()->m_cb->OnClose(pW);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

