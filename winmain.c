#include <windows.h>

#include "resource.h"

// 变量声明
HINSTANCE hinstance = 0;

// 位图和设备上下文声明
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0;

// 关卡数据声明
int levelSelection = 1;
struct LEVEL
{
	unsigned char personaX, personaY;   // 人物坐标
	unsigned char pointsX, pointsY;     // 积分点坐标
	unsigned char pointsX_2, pointsY_2; // 第二个积分点坐标
	unsigned char boxX, boxY;           // 箱子坐标
	unsigned char boxX_2, boxY_2;       // 第二个箱子坐标
	unsigned char scene[10][10];        // 场景数组
}
template[] =// 关卡模板数组
{   // 关卡1
    5, 4,// 人物坐标
    2, 2,// 积分点坐标
    7, 7,// 第二个积分点坐标
	5, 5,// 箱子坐标
	4, 6,// 第二个箱子坐标
	{// 数组内数字代表场景元素，1为墙壁，0为空气，3为箱子，2为积分点，4为人物（一个不会存在的值）
        1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,2,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,3,0,0,0,1,
        1,0,0,0,3,0,0,0,0,1,
        1,0,0,0,0,0,0,2,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1
    },
	// 关卡2
    1, 1,// 人物坐标
    8, 8,// 积分点坐标
	1, 8,// 第二个积分点坐标
	2, 2,// 箱子坐标
    6, 7,// 第二个箱子坐标
    {// 数组内数字代表场景元素，1为墙壁，0为空气，3为箱子，2为积分点，4为人物（一个不会存在的值）
        1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,2,1,
        1,0,3,0,1,1,1,1,0,1,
        1,0,0,0,1,0,0,1,0,1,
        1,1,1,0,1,0,0,1,0,1,
        1,0,0,0,1,0,0,1,0,1,
        1,0,1,1,1,0,0,1,0,1,
        1,0,0,0,0,0,3,0,0,1,
        1,0,0,0,0,0,0,0,2,1,
        1,1,1,1,1,1,1,1,1,1
	}
};
enum { AIR, WALL, POINTS_, BOX, PERSONA };// 场景元素枚举

// 窗口过程函数声明
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void LoadResources(HWND);
void DrawScene(HWND, HDC);
void ReleaseResources(void);

// 程序入口点
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    hinstance = hInstance;// 保存实例句柄
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
    wcex.lpszClassName = L"Sokoban";      			                // 窗口类名
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // 自定义小图标
	// 注册窗口类
    if (!RegisterClassEx(&wcex))
        return 0;
    // 获取屏幕宽高
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // 设置窗口宽高
    int winWidth = 976;
    int winHeight = 999;
    // 计算居中位置
    int x = (screenWidth - winWidth) / 2;
    int y = (screenHeight - winHeight) / 2;
	// 创建窗口
	HWND hWnd = CreateWindowEx(
            0,                              // 扩展样式
            L"Sokoban",                     // 窗口类名
            L"推箱子",                      // 窗口标题
            WS_OVERLAPPEDWINDOW,            // 窗口风格
            x, y,                           // 初始位置
            winWidth, winHeight,            // 初始大小
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
        
		LoadResources(hWnd);
        return 0;

    case WM_PAINT:// 绘制窗口内容
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // 在此处添加绘图代码
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        DrawScene(hWnd, hdc);

		EndPaint(hWnd, &ps);
    }
        return 0;

    case WM_SIZE:// 处理窗口大小改变
        
        return 0;

    case WM_KEYDOWN:// 处理键盘按下事件
    {
        int wmId = LOWORD(wParam);
		int x = template[levelSelection].personaX;
		int y = template[levelSelection].personaY;
        switch (wmId)
        {
        case VK_ESCAPE:// 按下Esc键时关闭窗口
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
		case VK_UP:// 角色移动控制
            if (y > 0 && template[levelSelection].scene[y - 1][x] != WALL)
			    template[levelSelection].personaY--;

            break;
		case VK_DOWN:
			if (y < 9 && template[levelSelection].scene[y + 1][x] != WALL)
			template[levelSelection].personaY++;

			break;
		case VK_LEFT:
			if (x > 0 && template[levelSelection].scene[y][x - 1] != WALL)
			template[levelSelection].personaX--;

			break;
		case VK_RIGHT:
			if (x < 9 && template[levelSelection].scene[y][x + 1] != WALL)
			template[levelSelection].personaX++;

			break;
        }
		HDC hdc = GetDC(hWnd);
		DrawScene(hWnd, hdc);
		ReleaseDC(hWnd, hdc);
    }
	return 0;

    case WM_COMMAND:// 处理菜单和控件命令
  
        return 0;

    case WM_DESTROY:// 窗口销毁时的清理代码
		ReleaseResources();
        PostQuitMessage(0);

        return 0;
    }
    // 其它未处理的消息交给默认窗口过程
    return DefWindowProc(hWnd, mSgId, wParam, lParam);
}

// 资源加载函数
void LoadResources(HWND hWnd)
{
    // 加载位图资源
	airBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_AIR));
	wallBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WALLS));
	boxBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BOX));
	pointBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_POINTS_));
    personaBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_PERSONA));
    // 创建内存设备上下文
    HDC hdc = GetDC(hWnd); // 获取屏幕设备上下文
    airDc = CreateCompatibleDC(hdc);
    wallDc = CreateCompatibleDC(hdc);
    boxDc = CreateCompatibleDC(hdc);
	pointDc = CreateCompatibleDC(hdc);
    personaDc = CreateCompatibleDC(hdc);
    // 将位图选入设备上下文
    SelectObject(airDc, airBmp);
    SelectObject(wallDc, wallBmp);
    SelectObject(boxDc, boxBmp);
    SelectObject(pointDc, pointBmp);
    SelectObject(personaDc, personaBmp);

    ReleaseDC(hWnd, hdc); // 释放屏幕设备上下文
	return;
}
// 绘制函数（示例）
void DrawScene(HWND hWnd, HDC hdc)
{
    // 兼容性DC
	HDC memDc = CreateCompatibleDC(hdc);
	HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);
	// 绘制场景
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			switch (template[levelSelection].scene[y][x])
			{
			case AIR:
				BitBlt(memDc, x * 96, y * 96, 96, 96, airDc, 0, 0, SRCCOPY);
				break;
			case WALL:
				BitBlt(memDc, x * 96, y * 96, 96, 96, wallDc, 0, 0, SRCCOPY);
				break;
			case POINTS_:
				BitBlt(memDc, x * 96, y * 96, 96, 96, pointDc, 0, 0, SRCCOPY);
				break;
			}
		}
	}
    // 绘制人物
    BitBlt(memDc, template[levelSelection].personaX * 96, template[levelSelection].personaY * 96, 96, 96, personaDc, 0, 0, SRCCOPY);
    // 绘制箱子
	BitBlt(memDc, template[levelSelection].boxX * 96, template[levelSelection].boxY * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
	BitBlt(memDc, template[levelSelection].boxX_2 * 96, template[levelSelection].boxY_2 * 96, 96, 96, boxDc, 0, 0, SRCCOPY);

    // 将内存DC内容复制到窗口DC
	BitBlt(hdc, 0, 0, 960, 960, memDc, 0, 0, SRCCOPY);
    // 清理
    DeleteObject(memBmp);
	DeleteDC(memDc);
	return;
}
// 资源释放函数
void ReleaseResources(void)
{
    // 释放位图资源
    DeleteObject(airBmp);
    airBmp = NULL;
	DeleteObject(wallBmp);
	wallBmp = NULL;
	DeleteObject(boxBmp);
	boxBmp = NULL;
	DeleteObject(pointBmp);
	pointBmp = NULL;
	DeleteObject(personaBmp);
	personaBmp = NULL;
    // 删除设备上下文
    DeleteDC(airDc);
    airDc = NULL;
    DeleteDC(wallDc);
    wallDc = NULL;
    DeleteDC(boxDc);
    boxDc = NULL;
    DeleteDC(pointDc);
    pointDc = NULL;
    DeleteDC(personaDc);
	personaDc = NULL;
	return;
}