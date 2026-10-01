#include <windows.h>

#include "Engine.hpp"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEXW wc = {.cbSize = sizeof(WNDCLASSEXW),
                      .style = (CS_HREDRAW | CS_VREDRAW),
                      .lpfnWndProc = &WndProc,
                      .hInstance = hInstance,
                      .hIcon = LoadIconW(0, IDI_APPLICATION),
                      .hCursor = LoadCursorW(0, IDC_ARROW),
                      .lpszClassName = L"App",
                      .hIconSm = LoadIconW(0, IDI_APPLICATION)};
    if (!RegisterClassExW(&wc))
        return GetLastError();
    RECT rect = {0, 0, WIDTH, HEIGHT};
    if (!AdjustWindowRectEx(&rect, (WS_OVERLAPPEDWINDOW | WS_VISIBLE), FALSE, WS_EX_OVERLAPPEDWINDOW))
        return GetLastError();

    HWND hwnd = nullptr;
    if (!(hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, wc.lpszClassName, L"Snake", (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
                                 CW_USEDEFAULT, CW_USEDEFAULT, (rect.right - rect.left), (rect.bottom - rect.top), 0, 0,
                                 hInstance, 0)))
        return GetLastError();

    Graphics2DEngine engine;
    if (FAILED(engine.EngineInit(hwnd)))
    {
        MessageBoxA(nullptr, "Failed to initialize Graphics2DEngine", "Fatal Error", MB_OK);
        return EXIT_FAILURE;
    }

    MSG msg = {0};
    for (;;)
    {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                goto exit;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        // logic
    }

exit:
    return static_cast<int>(msg.wParam);
}
