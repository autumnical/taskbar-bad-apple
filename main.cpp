#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>

#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    HRSRC hRes = FindResourceA(hInst, MAKEINTRESOURCEA(101), (LPCSTR)RT_RCDATA);
    if (!hRes) return 1;
    
    HGLOBAL hData = LoadResource(hInst, hRes);
    unsigned char* frames = (unsigned char*)LockResource(hData);
    DWORD fsize = SizeofResource(hInst, hRes);
    
    int w = 32, h = 32;
    DWORD frameBytes = w * h;
    DWORD totalFrames = fsize / frameBytes;
    
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = "BadApple";
    wc.hIcon = LoadIcon(hInst, "MAINICON");
    RegisterClassA(&wc);
    
    HWND hwnd = CreateWindowA("BadApple", "Bad Apple", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, hInst, NULL);
    
    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HDC hdcIcon = CreateCompatibleDC(hdc);
    
    BITMAPINFOHEADER bmi = {0};
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biWidth = w;
    bmi.biHeight = -h;
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biCompression = BI_RGB;
    
    HBITMAP hBmp = CreateDIBSection(hdc, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, NULL, NULL, 0);
    HBITMAP hIconBmp = CreateCompatibleBitmap(hdc, 32, 32);
    HBITMAP hMask = CreateBitmap(32, 32, 1, 1, NULL);
    
    LARGE_INTEGER freq, t0, t1;
    QueryPerformanceFrequency(&freq);
    double frameTime = (double)freq.QuadPart / 30.0;
    DWORD frame = 0;
    HICON hIcon = NULL;
    
    timeBeginPeriod(1);

    HRSRC hResA = FindResourceA(hInst, MAKEINTRESOURCEA(102), (LPCSTR)RT_RCDATA);
    if (hResA) {
        HGLOBAL hDataA = LoadResource(hInst, hResA);
        void* pAudio = LockResource(hDataA);
        PlaySoundA((LPCSTR)pAudio, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
    }

    QueryPerformanceCounter(&t0);
    
    BOOL running = TRUE;
    while (running) {
        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = FALSE;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        if (!running) break;
        
        QueryPerformanceCounter(&t1);
        if ((double)(t1.QuadPart - t0.QuadPart) >= frameTime) {
            if (frame >= totalFrames) {
                DestroyWindow(hwnd);
                break;
            }
            
            unsigned char* src = frames + (frame * frameBytes);
            BITMAP bm;
            GetObject(hBmp, sizeof(bm), &bm);
            DWORD* dst = (DWORD*)bm.bmBits;
            
            for (int i = 0; i < w*h; i++) {
                DWORD gray = src[i];
                dst[i] = (gray << 16) | (gray << 8) | gray | 0xFF000000;
            }
            
            SelectObject(hdcMem, hBmp);
            RECT rc;
            GetClientRect(hwnd, &rc);
            StretchBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, w, h, SRCCOPY);
            
            if (frame % 2 == 0) {
                SelectObject(hdcIcon, hIconBmp);
                StretchBlt(hdcIcon, 0, 0, 32, 32, hdcMem, 0, 0, w, h, SRCCOPY);
                ICONINFO ii = {TRUE, 0, 0, hMask, hIconBmp};
                HICON hNew = CreateIconIndirect(&ii);
                if (hNew) {
                    SendMessageA(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hNew);
                    SendMessageA(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hNew);
                    if (hIcon) DestroyIcon(hIcon);
                    hIcon = hNew;
                }
            }
            
            frame++;
            t0.QuadPart += (LONGLONG)frameTime;
        }
        Sleep(1);
    }
    
    timeEndPeriod(1);
    PlaySoundA(NULL, NULL, 0);
    if (hIcon) DestroyIcon(hIcon);
    DeleteObject(hBmp);
    DeleteObject(hIconBmp);
    DeleteObject(hMask);
    DeleteDC(hdcMem);
    DeleteDC(hdcIcon);
    ReleaseDC(hwnd, hdc);
    
    return 0;
}