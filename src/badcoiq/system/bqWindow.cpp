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

#include "badcoiq/gs/bqGS.h"

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
    case WM_INPUT:
    {
        if (pW)
        {
            HRAWINPUT hRawInput = (HRAWINPUT)lParam;
            UINT dataSize;
            GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

            if (dataSize == 0 || dataSize > 0xff)
                break;

            bqWindowWin32* w32 = (bqWindowWin32*)pW->GetData()->m_implementation;

            void* dataBuf = &w32->m_rawInputData[0];
            GetRawInputData(hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));

            const RAWINPUT* raw = (const RAWINPUT*)dataBuf;
            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                HANDLE deviceHandle = raw->header.hDevice;

                const RAWMOUSE& mouseData = raw->data.mouse;

                USHORT flags = mouseData.usButtonFlags;
                short wheelDelta = (short)mouseData.usButtonData;
                LONG x = mouseData.lLastX, y = mouseData.lLastY;

                /*wprintf(
                    L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
                    deviceHandle, flags, wheelDelta, x, y);*/

                bqInputData* id = bqInput::GetData();

                if (wheelDelta)
                    id->m_mouseWheelDelta = (float)wheelDelta / (float)WHEEL_DELTA;

                RECT rct;
                GetWindowRect(hWnd, &rct);

                POINT cursorPoint;
                GetCursorPos(&cursorPoint);
                //ScreenToClient(hWnd, &cursorPoint);
                id->m_mousePosition.x = (float)(cursorPoint.x - rct.left - pW->GetBorderSize()->x);
                id->m_mousePosition.y = (float)(cursorPoint.y - rct.top - pW->GetBorderSize()->y);

                // printf("%i %i\n", cursorPoint.x, cursorPoint.y);

                if (flags)
                {
                    if ((flags & 0x1) == 0x1)
                    {
                        bqInput::EnableLMBDown();
                        bqInput::EnableLMBHold();
                    }

                    if ((flags & 0x2) == 0x2)
                    {
                        bqInput::EnableLMBUp();
                        bqInput::DisableLMBHold();
                    }

                    if ((flags & 0x4) == 0x4)
                    {
                        bqInput::EnableRMBDown();
                        bqInput::EnableRMBHold();
                    }
                    if ((flags & 0x8) == 0x8)
                    {
                        bqInput::EnableRMBUp();
                        bqInput::DisableRMBHold();
                    }

                    if ((flags & 0x10) == 0x10)
                    {
                        bqInput::EnableMMBDown();
                        bqInput::EnableMMBHold();
                    }
                    if ((flags & 0x20) == 0x20)
                    {
                        bqInput::EnableMMBUp();
                        bqInput::DisableMMBHold();
                    }

                    if ((flags & 0x100) == 0x100)
                    {
                        bqInput::EnableX1MBDown();
                        bqInput::EnableX1MBHold();
                    }
                    if ((flags & 0x200) == 0x200)
                    {
                        bqInput::EnableX1MBUp();
                        bqInput::DisableX1MBHold();
                    }

                    if ((flags & 0x40) == 0x40)
                    {
                        bqInput::EnableX2MBDown();
                        bqInput::EnableX2MBHold();
                    }
                    if ((flags & 0x80) == 0x80)
                    {
                        bqInput::EnableX2MBUp();
                        bqInput::DisableX2MBHold();
                    }

                }

            }
        }
    }break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        bool isPress = true;

        uint32_t key = (uint32_t)wParam;

        if (message == WM_SYSKEYUP) isPress = false;
        if (message == WM_KEYUP) isPress = false;

        if (key == bqInput::KEY_SHIFT)
        { // shift -> lshift rshift
            key = MapVirtualKey(((lParam >> 16) & 255u), 3);
        }
        if (key == bqInput::KEY_CTRL)
        { // ctrl -> lctrl rctrl
            key = MapVirtualKey(((lParam >> 16) & 255), 3);
            if (lParam & 0x1000000)
                key = 163;
        }

        if (key == bqInput::KEY_ALT)
        { // alt -> lalt ralt
            key = MapVirtualKey(((lParam >> 16) & 255), 3);
            if (lParam & 0x1000000)
                key = 165;
            //printf("alt = %i\n",(int)ev.keyboardEvent.key);
        }

        if (isPress)
        {
            if (key < 256)
            {
                bqInput::SetKeyHold(key, true);
                bqInput::SetKeyHit(key, true);
            }
        }
        else
        {
            if (key < 256)
            {
                bqInput::SetKeyHold(key, false);
                bqInput::SetKeyRelease(key, true);
            }
        }

        if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        else
        {
            return 0;
        }
    }break;
    case WM_CLOSE:
        if (pW)
            pW->GetData()->m_cb->OnClose(pW);
        return 0;
    // Получение вводимого символа
    case WM_CHAR:
    {
        bqInputData* id = bqInput::GetData();
        id->m_character = wParam;
    }break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
            (wParam & 0xFFF0) == SC_MONITORPOWER ||
            (wParam & 0xFFF0) == SC_KEYMENU
            )
        {
            return 0;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

