#include <windows.h>

#include "resource.h"

// 变量声明
HINSTANCE hIns;
HBITMAP hBmp = NULL;
HDC hdc, bmpdc;
int score = 0;
HWND hScoreLabel = NULL;

// 窗口过程函数声明
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void OnButtonClick(HWND, LPARAM);

// 程序入口点
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	hIns = hInstance;// 保存实例句柄
	// 定义并初始化窗口类
    WNDCLASSEX wcex = { 0 };
    // 对结构体各成员初始化
    wcex.cbSize = sizeof(WNDCLASSEX);          		                // 结构大小
    wcex.style = CS_HREDRAW | CS_VREDRAW;      		                // 窗口样式：当水平或垂直方向变化时重绘
    wcex.lpfnWndProc = WindowProc;             			            // 窗口过程函数（回调函数名）
    wcex.hInstance = hInstance;                		                // 实例句柄
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); 	// 自定义图标
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   	                // 光标：使用标准箭头光标
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); 	            // 背景：使用系统窗口颜色
    wcex.lpszClassName = L"MyWindow";      			                // 窗口类名
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // 自定义小图标
	// 注册窗口类
    if (!RegisterClassEx(&wcex))
        return 0;
	// 创建窗口
	HWND hWnd = CreateWindowEx(
            0,                              // 扩展样式
            L"MyWindow",                    // 窗口类名
            L"你好！Windows",               // 窗口标题
            WS_OVERLAPPEDWINDOW,            // 窗口风格
            CW_USEDEFAULT, CW_USEDEFAULT,   // 初始位置
            800, 600,                       // 初始大小
            NULL,                           // 父窗口句柄
            NULL,                           // 菜单句柄
            hInstance,                      // 实例句柄
            NULL                            // 附加参数
		);
    if (!hWnd)
		return 0;
	// 显示和更新窗口
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// 消息循环
	MSG msg;// 队列消息结构体变量
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
    return 0;
}
// 窗口过程函数：处理窗口消息（回调函数）
LRESULT CALLBACK WindowProc(HWND hWnd, UINT mSgId, WPARAM wParam, LPARAM lParam)
{
    switch (mSgId)//函数的第二个参数
    {
    case WM_CREATE:// 窗口创建时的初始化代码
    {
        OnButtonClick(hWnd, lParam);
    }
        return 0;

    case WM_PAINT:// 绘制窗口内容
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // 在此处添加绘图代码

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hWnd, &ps);
    }
        return 0;

    case WM_SIZE:// 处理窗口大小改变
        
        return 0;

    case WM_KEYDOWN:// 处理键盘按下事件
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case VK_ESCAPE:// 按下Esc键时关闭窗口
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
        case VK_UP:

			return 0;
		case VK_DOWN:

			return 0;
		case VK_LEFT:

			return 0;
		case VK_RIGHT:

			return 0;
        }
    }
	return 0;

    case WM_COMMAND:// 处理菜单和控件命令
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case 1: // 按钮ID
            score++;
            // 更新分数显示
            wchar_t buf[32];
            wsprintf(buf, L"分数: %d", score);
            SetWindowText(hScoreLabel, buf);
            return 0;
        default:
            return DefWindowProc(hWnd, mSgId, wParam, lParam);
        }
    }
        return 0;

    case WM_DESTROY:// 窗口销毁时的清理代码
        
        PostQuitMessage(0);
        return 0;
    }
    // 其它未处理的消息交给默认窗口过程
    return DefWindowProc(hWnd, mSgId, wParam, lParam);
}

// 按钮函数
void OnButtonClick(HWND hWnd, LPARAM lParam)
{
    static HWND hButton = NULL;
    if (!hButton) {
        hButton = CreateWindow(
            L"BUTTON", L"点击我",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30,
            hWnd, (HMENU)1, hIns, NULL
        );
        // 创建分数显示标签
        hScoreLabel = CreateWindow(
            L"STATIC", L"分数: 0",
            WS_VISIBLE | WS_CHILD,
            120, 10, 100, 30,
            hWnd, NULL, hIns, NULL
        );
    }
}