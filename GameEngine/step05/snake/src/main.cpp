#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include <Engine.h>
#include <Mesh.h>

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int main() {
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc  = {};
    wc.cbSize       = sizeof(wc);
    wc.style        = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc  = WndProc;
    wc.hInstance    = hInstance;
    wc.hCursor      = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"SnakeWindow";
    if (!RegisterClassExW(&wc)) {
        std::cerr << "[Snake] Failed to register window class\n";
        return 1;
    }

    HWND hwnd = CreateWindowExW(
        0, L"SnakeWindow", L"Snake",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    if (!hwnd) {
        std::cerr << "[Snake] Failed to create window\n";
        return 1;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    Engine engine;
    if (!engine.Init(hwnd)) {
        std::cerr << "[Snake] Engine init failed\n";
        return 1;
    }

    Vertex triangle[] = {
        {{ 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    };
    engine.GetScene()->AddMesh(new Mesh(triangle, 3));

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT) break;
        }
        engine.Update();
    }

    engine.Shutdown();
    return 0;
}
