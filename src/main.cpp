#include <windows.h>

#include "constants.hpp"
#include "engine.hpp"
#include "logic.hpp"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    auto *gameContextPtr =
        reinterpret_cast<GameContext<std::uint16_t, TILES> *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_NCCREATE: {
        auto *createStruct = reinterpret_cast<CREATESTRUCTW *>(lparam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
        return TRUE;
    }
    case WM_CLOSE: {
        if (MessageBoxA(hwnd, "Are you sure you want to quit?", "Confirm Exit", MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            DestroyWindow(hwnd);
        }
        else
        {
            return 0;
        }
        break;
    }
    case WM_KEYUP:
        if (gameContextPtr)
        {
            gameContextPtr->keys.reset(wparam);
            return 0;
        }
        break;
    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            return 0;
        }
        if (gameContextPtr)
        {
            gameContextPtr->keys.set(wparam);
            return 0;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

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
    RECT rect{0, 0, static_cast<LONG>(WINDOW_WIDTH), static_cast<LONG>(WINDOW_HEIGHT)};
    if (!AdjustWindowRectEx(&rect, (WS_OVERLAPPEDWINDOW | WS_VISIBLE), FALSE, WS_EX_OVERLAPPEDWINDOW))
        return GetLastError();

    GameContext<std::uint16_t, TILES> gameContext;

    HWND hwnd = nullptr;
    if (!(hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, wc.lpszClassName, L"Snake", (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
                                 CW_USEDEFAULT, CW_USEDEFAULT, (rect.right - rect.left), (rect.bottom - rect.top), 0, 0,
                                 hInstance, &gameContext)))
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
