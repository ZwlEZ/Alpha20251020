/*
询问：
deep seek
鼠标悬停在窗口上5秒后不显示再次移动鼠标才会显示怎么实现（鼠标箭头）
*/
#include <windows.h>
#include <stdbool.h>

#include "resource.h"

// 变量声明
HINSTANCE hinstance = 0;

// 位图和设备上下文声明
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0, scoreBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0, scoreDc = 0;

// 关卡数据声明
int levelSelection = 0;
int totalLevels = 10; // 总关卡数
struct LEVEL
{
    unsigned char personaX, personaY;   // 人物坐标
    unsigned char scene[10][10];        // 场景数组
}
template[] = {
    // 关卡数据保持不变...
    // 关卡1到10的数据
};
enum { AIR, WALL, POINTS_, BOX, PERSONA, SCORE };// 场景元素枚举
unsigned char boxCounts = 0, scoreCounts = 0;

// 鼠标隐藏相关变量
bool cursorHidden = false;
UINT_PTR hideCursorTimer = 0;
POINT lastMousePos = { 0, 0 };

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
void NextLevel(HWND hWnd);
void UpdateWindowTitle(HWND hWnd);
void StartHideCursorTimer(HWND hWnd);
void StopHideCursorTimer(HWND hWnd);
void HideCursor(void);
void ShowCursor(void);

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
        UpdateWindowTitle(hWnd);
        // 获取初始鼠标位置
        GetCursorPos(&lastMousePos);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        DrawScene(hWnd, hdc);
        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_SIZE:
        return 0;

    case WM_MOUSEMOVE:
    {
        // 鼠标移动时显示光标
        if (cursorHidden)
        {
            ShowCursor();
            cursorHidden = false;
        }

        // 重置隐藏计时器
        StopHideCursorTimer(hWnd);
        StartHideCursorTimer(hWnd);

        // 更新鼠标位置
        lastMousePos.x = LOWORD(lParam);
        lastMousePos.y = HIWORD(lParam);
    }
    return 0;

    case WM_TIMER:
        if (wParam == 1) // 隐藏光标的定时器
        {
            // 检查鼠标是否真的没有移动
            POINT currentPos;
            GetCursorPos(&currentPos);

            // 将屏幕坐标转换为窗口客户区坐标
            ScreenToClient(hWnd, &currentPos);

            if (currentPos.x == lastMousePos.x && currentPos.y == lastMousePos.y)
            {
                // 鼠标确实没有移动，隐藏光标
                HideCursor();
                cursorHidden = true;
            }
            else
            {
                // 鼠标移动了，更新位置并重新开始计时
                lastMousePos = currentPos;
                StopHideCursorTimer(hWnd);
                StartHideCursorTimer(hWnd);
            }
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
        case VK_UP:
        case 'W':
        case 'w':
            MoveUp();
            break;
        case VK_DOWN:
        case 'S':
        case 's':
            MoveDown();
            break;
        case VK_LEFT:
        case 'A':
        case 'a':
            MoveLeft();
            break;
        case VK_RIGHT:
        case 'D':
        case 'd':
            MoveRight();
            break;
        case 'R':
        case 'r': // 重置当前关卡
            // 可以在这里添加重置逻辑
            break;
        }

        HDC hdc = GetDC(hWnd);
        DrawScene(hWnd, hdc);
        ReleaseDC(hWnd, hdc);

        // 检查关卡是否完成
        if (CheckLevelComplete())
        {
            NextLevel(hWnd);
        }
    }
    return 0;

    case WM_SETCURSOR:
        // 当设置光标时，如果光标是隐藏状态，则隐藏光标
        if (cursorHidden && LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(NULL);
            return TRUE;
        }
        break;

    case WM_ACTIVATE:
        // 窗口激活状态改变时显示光标
        if (wParam != WA_INACTIVE && cursorHidden)
        {
            ShowCursor();
            cursorHidden = false;
        }
        break;

    case WM_DESTROY:
        // 清理资源
        StopHideCursorTimer(hWnd);
        if (cursorHidden)
        {
            ShowCursor();
        }
        ReleaseResources();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, mSgId, wParam, lParam);
}

// 开始隐藏光标的计时器
void StartHideCursorTimer(HWND hWnd)
{
    if (hideCursorTimer == 0)
    {
        // 设置5秒后触发的定时器
        hideCursorTimer = SetTimer(hWnd, 1, 5000, NULL);
    }
}

// 停止隐藏光标的计时器
void StopHideCursorTimer(HWND hWnd)
{
    if (hideCursorTimer != 0)
    {
        KillTimer(hWnd, hideCursorTimer);
        hideCursorTimer = 0;
    }
}

// 隐藏光标
void HideCursor(void)
{
    // 隐藏系统光标
    while (ShowCursor(FALSE) >= 0)
    {
        // 持续调用直到光标隐藏
    }
}

// 显示光标
void ShowCursor(void)
{
    // 显示系统光标
    while (ShowCursor(TRUE) < 0)
    {
        // 持续调用直到光标显示
    }
}

// 检查关卡是否完成
BOOL CheckLevelComplete(void)
{
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            if (template[levelSelection].scene[y][x] == POINTS_)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

// 切换到下一关
void NextLevel(HWND hWnd)
{
    MessageBox(hWnd, L"恭喜！关卡完成！", L"过关", MB_OK | MB_ICONINFORMATION);

    levelSelection++;

    if (levelSelection >= totalLevels)
    {
        MessageBox(hWnd, L"恭喜！你已通关所有关卡！", L"游戏通关", MB_OK | MB_ICONINFORMATION);
        levelSelection = 0;
    }

    UpdateWindowTitle(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
}

// 更新窗口标题显示当前关卡信息
void UpdateWindowTitle(HWND hWnd)
{
    WCHAR title[100];
    swprintf(title, 100, L"推箱子 - 第%d关/共%d关", levelSelection + 1, totalLevels);
    SetWindowText(hWnd, title);
}

// 资源加载函数
void LoadResources(HWND hWnd)
{
    airBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_AIR));
    wallBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WALLS));
    boxBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BOX));
    pointBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_POINTS_));
    personaBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_PERSONA));
    scoreBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_SCORE));

    HDC hdc = GetDC(hWnd);
    airDc = CreateCompatibleDC(hdc);
    wallDc = CreateCompatibleDC(hdc);
    boxDc = CreateCompatibleDC(hdc);
    pointDc = CreateCompatibleDC(hdc);
    personaDc = CreateCompatibleDC(hdc);
    scoreDc = CreateCompatibleDC(hdc);

    SelectObject(airDc, airBmp);
    SelectObject(wallDc, wallBmp);
    SelectObject(boxDc, boxBmp);
    SelectObject(pointDc, pointBmp);
    SelectObject(personaDc, personaBmp);
    SelectObject(scoreDc, scoreBmp);

    ReleaseDC(hWnd, hdc);
}

// 绘制函数
void DrawScene(HWND hWnd, HDC hdc)
{
    HDC memDc = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);

    // 绘制白色背景
    RECT rect = { 0, 0, 960, 960 };
    FillRect(memDc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    // 绘制场景
    boxCounts = 0, scoreCounts = 0;
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
            case BOX:
                BitBlt(memDc, x * 96, y * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
                boxCounts += 1;
                break;
            case SCORE:
                BitBlt(memDc, x * 96, y * 96, 96, 96, scoreDc, 0, 0, SRCCOPY);
                scoreCounts += 1;
                break;
            }
        }
    }

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
    if (scoreBmp) DeleteObject(scoreBmp);
    personaBmp = airBmp = wallBmp = boxBmp = pointBmp = scoreBmp = NULL;

    if (airDc) DeleteDC(airDc);
    if (wallDc) DeleteDC(wallDc);
    if (boxDc) DeleteDC(boxDc);
    if (pointDc) DeleteDC(pointDc);
    if (personaDc) DeleteDC(personaDc);
    if (scoreDc) DeleteDC(scoreDc);
    personaDc = airDc = wallDc = boxDc = pointDc = scoreDc = NULL;
}

// 移动函数保持不变...
void MoveUp(void)
{
    // 原有的MoveUp代码保持不变
}

void MoveDown(void)
{
    // 原有的MoveDown代码保持不变
}

void MoveLeft(void)
{
    // 原有的MoveLeft代码保持不变
}

void MoveRight(void)
{
    // 原有的MoveRight代码保持不变
}