#include <windows.h>
#include <imm.h>  // 添加输入法支持头文件
#pragma comment(lib, "imm32.lib")  // 链接输入法库

#include "resource.h"

// 变量声明
HINSTANCE hinstance = 0;

// 位图和设备上下文声明
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0;

// 关卡数据声明
int levelSelection = 0;
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
void MoveUp(void);
void MoveDown(void);
void MoveLeft(void);
void MoveRight(void);
BOOL CheckLevelComplete(void);
void UpdateGameArea(HWND hWnd);

// 程序入口点
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    hinstance = hInstance;

    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"Sokoban";
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    if (!RegisterClassEx(&wcex))
        return 0;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int winWidth = 976;
    int winHeight = 999;
    int x = (screenWidth - winWidth) / 2;
    int y = (screenHeight - winHeight) / 2;

    HWND hWnd = CreateWindowEx(
        0, L"Sokoban", L"推箱子",
        WS_OVERLAPPEDWINDOW,
        x, y, winWidth, winHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!hWnd)
        return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT mSgId, WPARAM wParam, LPARAM lParam)
{
    switch (mSgId)
    {
    case WM_CREATE:
        LoadResources(hWnd);
        // 禁用IME输入法，这是关键步骤！
        ImmAssociateContext(hWnd, NULL);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawScene(hWnd, hdc);
        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_KEYDOWN:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
        }
    }
    return 0;

    case WM_CHAR:  // 使用WM_CHAR消息，这个在中文输入法下也能正常工作
    {
        switch (wParam)
        {
        case 'w':
        case 'W':
        case 0x1A:  // 中文输入法下的W
            MoveUp();
            break;
        case 's':
        case 'S':
        case 0x13:  // 中文输入法下的S
            MoveDown();
            break;
        case 'a':
        case 'A':
        case 0x01:  // 中文输入法下的A
            MoveLeft();
            break;
        case 'd':
        case 'D':
        case 0x04:  // 中文输入法下的D
            MoveRight();
            break;
        case 'r':
        case 'R':
            // 重置关卡逻辑可以在这里添加
            break;
        }

        if (CheckLevelComplete())
        {
            MessageBox(hWnd, L"恭喜！关卡完成！", L"过关", MB_OK);
        }

        UpdateGameArea(hWnd);
    }
    return 0;

    case WM_DESTROY:
        ReleaseResources();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, mSgId, wParam, lParam);
}

// 资源加载函数
void LoadResources(HWND hWnd)
{
    airBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_AIR));
    wallBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WALLS));
    boxBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BOX));
    pointBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_POINTS_));
    personaBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_PERSONA));

    HDC hdc = GetDC(hWnd);
    airDc = CreateCompatibleDC(hdc);
    wallDc = CreateCompatibleDC(hdc);
    boxDc = CreateCompatibleDC(hdc);
    pointDc = CreateCompatibleDC(hdc);
    personaDc = CreateCompatibleDC(hdc);

    SelectObject(airDc, airBmp);
    SelectObject(wallDc, wallBmp);
    SelectObject(boxDc, boxBmp);
    SelectObject(pointDc, pointBmp);
    SelectObject(personaDc, personaBmp);

    ReleaseDC(hWnd, hdc);
}

// 绘制场景
void DrawScene(HWND hWnd, HDC hdc)
{
    HDC memDc = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);

    // 绘制白色背景
    RECT rect = { 0, 0, 960, 960 };
    HBRUSH whiteBrush = GetStockObject(WHITE_BRUSH);
    FillRect(memDc, &rect, whiteBrush);

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

    // 绘制箱子
    BitBlt(memDc, template[levelSelection].boxX * 96, template[levelSelection].boxY * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
    BitBlt(memDc, template[levelSelection].boxX_2 * 96, template[levelSelection].boxY_2 * 96, 96, 96, boxDc, 0, 0, SRCCOPY);

    // 绘制人物
    BitBlt(memDc, template[levelSelection].personaX * 96, template[levelSelection].personaY * 96, 96, 96, personaDc, 0, 0, SRCCOPY);

    // 将内存DC内容复制到窗口DC
    BitBlt(hdc, 0, 0, 960, 960, memDc, 0, 0, SRCCOPY);

    // 清理
    DeleteObject(memBmp);
    DeleteDC(memDc);
}

// 资源释放函数
void ReleaseResources(void)
{
    if (wallBmp) DeleteObject(wallBmp);
    if (boxBmp) DeleteObject(boxBmp);
    if (pointBmp) DeleteObject(pointBmp);
    if (airBmp) DeleteObject(airBmp);
    if (personaBmp) DeleteObject(personaBmp);
    personaBmp = airBmp = wallBmp = boxBmp = pointBmp = NULL;

    if (airDc) DeleteDC(airDc);
    if (wallDc) DeleteDC(wallDc);
    if (boxDc) DeleteDC(boxDc);
    if (pointDc) DeleteDC(pointDc);
    if (personaDc) DeleteDC(personaDc);
    personaDc = airDc = wallDc = boxDc = pointDc = NULL;
}

// 检查关卡是否完成
BOOL CheckLevelComplete(void)
{
    BOOL box1OnPoint = (template[levelSelection].boxX == template[levelSelection].pointsX &&
        template[levelSelection].boxY == template[levelSelection].pointsY) ||
        (template[levelSelection].boxX == template[levelSelection].pointsX_2 &&
            template[levelSelection].boxY == template[levelSelection].pointsY_2);

    BOOL box2OnPoint = (template[levelSelection].boxX_2 == template[levelSelection].pointsX &&
        template[levelSelection].boxY_2 == template[levelSelection].pointsY) ||
        (template[levelSelection].boxX_2 == template[levelSelection].pointsX_2 &&
            template[levelSelection].boxY_2 == template[levelSelection].pointsY_2);

    return box1OnPoint && box2OnPoint;
}

// 只更新游戏区域，避免闪烁
void UpdateGameArea(HWND hWnd)
{
    RECT gameRect = { 0, 0, 960, 960 };
    InvalidateRect(hWnd, &gameRect, FALSE);
    UpdateWindow(hWnd);
}

// 移动函数保持不变
void MoveUp(void)
{
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    if (template[levelSelection].scene[personaY - 1][personaX] == WALL)
        return;

    if ((boxX == personaX) && (boxY == personaY - 1))
    {
        if (template[levelSelection].scene[boxY - 1][boxX] == WALL)
            return;
        template[levelSelection].boxY--;
        template[levelSelection].personaY--;
    }
    else if ((boxX_2 == personaX) && (boxY_2 == personaY - 1))
    {
        if (template[levelSelection].scene[boxY_2 - 1][boxX_2] == WALL)
            return;
        template[levelSelection].boxY_2--;
        template[levelSelection].personaY--;
    }
    else
    {
        template[levelSelection].personaY--;
    }
}

void MoveDown(void)
{
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    if (template[levelSelection].scene[personaY + 1][personaX] == WALL)
        return;

    if ((boxX == personaX) && (boxY == personaY + 1))
    {
        if (template[levelSelection].scene[boxY + 1][boxX] == WALL)
            return;
        template[levelSelection].boxY++;
        template[levelSelection].personaY++;
    }
    else if ((boxX_2 == personaX) && (boxY_2 == personaY + 1))
    {
        if (template[levelSelection].scene[boxY_2 + 1][boxX_2] == WALL)
            return;
        template[levelSelection].boxY_2++;
        template[levelSelection].personaY++;
    }
    else
    {
        template[levelSelection].personaY++;
    }
}

void MoveLeft(void)
{
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    if (template[levelSelection].scene[personaY][personaX - 1] == WALL)
        return;

    if ((boxX == personaX - 1) && (boxY == personaY))
    {
        if (template[levelSelection].scene[boxY][boxX - 1] == WALL)
            return;
        template[levelSelection].boxX--;
        template[levelSelection].personaX--;
    }
    else if ((boxX_2 == personaX - 1) && (boxY_2 == personaY))
    {
        if (template[levelSelection].scene[boxY_2][boxX_2 - 1] == WALL)
            return;
        template[levelSelection].boxX_2--;
        template[levelSelection].personaX--;
    }
    else
    {
        template[levelSelection].personaX--;
    }
}

void MoveRight(void)
{
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    if (template[levelSelection].scene[personaY][personaX + 1] == WALL)
        return;

    if ((boxX == personaX + 1) && (boxY == personaY))
    {
        if (template[levelSelection].scene[boxY][boxX + 1] == WALL)
            return;
        template[levelSelection].boxX++;
        template[levelSelection].personaX++;
    }
    else if ((boxX_2 == personaX + 1) && (boxY_2 == personaY))
    {
        if (template[levelSelection].scene[boxY_2][boxX_2 + 1] == WALL)
            return;
        template[levelSelection].boxX_2++;
        template[levelSelection].personaX++;
    }
    else
    {
        template[levelSelection].personaX++;
    }
}