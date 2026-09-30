#include <windows.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  return DefWindowProcW(hwnd, msg, wparam, lparam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine,
                    int nCmdShow) {
  WNDCLASSEXW wc = {.cbSize = sizeof(WNDCLASSEXW),
                    .style = (CS_HREDRAW | CS_VREDRAW),
                    .lpfnWndProc = &WndProc,
                    .hInstance = hInstance,
                    .hIcon = LoadIcon(0, IDI_APPLICATION),
                    .hCursor = LoadCursor(0, IDC_ARROW),
                    .lpszClassName = L"App",
                    .hIconSm = LoadIcon(0, IDI_APPLICATION)};
  if (!RegisterClassExW(&wc))
    return GetLastError();
  RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  if (!AdjustWindowRectEx(&rect, (WS_OVERLAPPEDWINDOW | WS_VISIBLE), FALSE,
                          WS_EX_OVERLAPPEDWINDOW))
    return GetLastError();
  HWND hwnd = nullptr;
  if (!(hwnd =
            CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, wc.lpszClassName, L"Snake",
                            (WS_OVERLAPPEDWINDOW | WS_VISIBLE), CW_USEDEFAULT,
                            CW_USEDEFAULT, (rect.right - rect.left),
                            (rect.bottom - rect.top), 0, 0, hInstance, 0)))
    return GetLastError();

  MSG msg = {0};
  for (;;) {
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        goto exit;
      }
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }

exit:
  return static_cast<int>(msg.wParam);
}
