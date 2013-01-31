#include "Window.h"


Window::Window(HINSTANCE hInstance) : wndClassName(L"myWindowClass"),
									  hInstance(hInstance)
{
    this->wc.cbSize        = sizeof(WNDCLASSEX);
    this->wc.style         = CS_OWNDC;
    this->wc.lpfnWndProc   = Window::WndProc;
    this->wc.cbClsExtra    = 0;
    this->wc.cbWndExtra    = 0;
    this->wc.hInstance     = this->hInstance;
    this->wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    this->wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    this->wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    this->wc.lpszMenuName  = NULL;
    this->wc.lpszClassName = this->wndClassName.c_str();
    this->wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    }

    this->hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        this->wndClassName.c_str(),
        L"Angine",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, this->hInstance, NULL
	);

    if (this->hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    }

    SetWindowLongPtr(this->hwnd, GWL_USERDATA, (__int3264)this);

    ShowWindow(this->hwnd, SW_SHOWDEFAULT);
    UpdateWindow(this->hwnd);

}


Window::~Window(void)
{
}


bool Window::start(void)
{

	return true;
}


void Window::update(void)
{
	if (GetMessage(&this->Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&this->Msg);
		DispatchMessage(&this->Msg);
	} else {
		this->kill();
	}
}


void Window::stop(void)
{
	CloseWindow(Window::hwnd);
}


HWND Window::getHWND()
{
    return this->hwnd;
}


void Window::keyDown(int key)
{
    this->bufferKeys.insert(key);
}


void Window::keyUp(int key)
{
    this->bufferKeys.erase(key);
}


std::set<int> Window::keys()
{
    return this->bufferKeys;
}


LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window *window = (Window*)GetWindowLongPtr(hwnd, GWL_USERDATA);

    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            window->keyDown(27);
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN:
            window->keyDown((int)wParam);
            break;

        case WM_KEYUP:
            window->keyUp((int)wParam);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}