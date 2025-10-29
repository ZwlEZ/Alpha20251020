#include <windows.h>
#include "resource.h"

// 变量声明
HINSTANCE hinstance = 0;

// 位图和设备上下文声明
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0, boxOnPointBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0, boxOnPointDc = 0;

// 场景元素枚举
enum { AIR, WALL, POINTS_, BOX, PERSONA, BOX_ON_POINT };

// 关卡数据声明
int levelSelection = 0;
int totalLevels = 2;
struct LEVEL
{
    unsigned char personaX, personaY;   // 人物坐标
    unsigned char scene[10][10];        // 场景数组
}
template[] = {// 关卡模板数组
    // 关卡1
    {
        5, 4, // 人物坐标
        {// 数组内数字代表场景元素，1为墙壁，0为空气，3为箱子，2为积分点
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
        }
    },
    // 关卡2
    {
        1, 1, // 人物坐标
        {// 数组内数字代表场景元素，1为墙壁，0为空气，3为箱子，2为积分点
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
    }
};

// 窗口过程函数声明
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void LoadResources(HWND);
void DrawScene(HWND, HDC);
void ReleaseResources(void);
BOOL MoveBox(int fromX, int fromY, int toX, int toY);
BOOL CheckLevelComplete(void);
void ResetLevel(void);
void LoadLevel(int level);

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
        LoadLevel(levelSelection);
        return 0;

    case WM_PAINT:// 绘制窗口内容
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 绘制游戏场景
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
        int newX = x, newY = y;
        int boxX, boxY;

        switch (wmId)
        {
        case VK_ESCAPE:// 按下Esc键时关闭窗口
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;

        case 'R':// 重置当前关卡
        case 'r':
            ResetLevel();
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;

        case VK_UP:// 角色移动控制
            newY = y - 1;
            break;

        case VK_DOWN:
            newY = y + 1;
            break;

        case VK_LEFT:
            newX = x - 1;
            break;

        case VK_RIGHT:
            newX = x + 1;
            break;
        }

        // 检查移动是否有效
        if (newX >= 0 && newX < 10 && newY >= 0 && newY < 10)
        {
            int targetCell = template[levelSelection].scene[newY][newX];

            if (targetCell == WALL)
            {
                // 碰到墙壁，不能移动
                break;
            }
            else if (targetCell == BOX || targetCell == BOX_ON_POINT)
            {
                // 尝试推动箱子
                boxX = newX + (newX - x);
                boxY = newY + (newY - y);

                // 检查箱子能否被推动
                if (boxX >= 0 && boxX < 10 && boxY >= 0 && boxY < 10)
                {
                    int boxTargetCell = template[levelSelection].scene[boxY][boxX];

                    if (boxTargetCell == AIR || boxTargetCell == POINTS_)
                    {
                        // 推动箱子
                        if (MoveBox(newX, newY, boxX, boxY))
                        {
                            // 箱子推动成功，人物移动到箱子的位置
                            template[levelSelection].personaX = newX;
                            template[levelSelection].personaY = newY;
                        }
                    }
                }
            }
            else
            {
                // 移动到空地或目标点
                template[levelSelection].personaX = newX;
                template[levelSelection].personaY = newY;
            }

            // 检查关卡是否完成
            if (CheckLevelComplete())
            {
                MessageBox(hWnd, L"恭喜！关卡完成！", L"过关", MB_OK | MB_ICONINFORMATION);
                // 加载下一关
                levelSelection = (levelSelection + 1) % totalLevels;
                LoadLevel(levelSelection);
            }

            // 重绘场景
            InvalidateRect(hWnd, NULL, TRUE);
        }
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

// 加载关卡
void LoadLevel(int level)
{
    if (level >= 0 && level < totalLevels)
    {
        levelSelection = level;
        // 这里可以添加更多关卡初始化逻辑
    }
}

// 重置当前关卡
void ResetLevel(void)
{
    // 重新加载当前关卡
    LoadLevel(levelSelection);
}

// 移动箱子
BOOL MoveBox(int fromX, int fromY, int toX, int toY)
{
    if (fromX < 0 || fromX >= 10 || fromY < 0 || fromY >= 10 ||
        toX < 0 || toX >= 10 || toY < 0 || toY >= 10)
    {
        return FALSE;
    }

    int fromCell = template[levelSelection].scene[fromY][fromX];
    int toCell = template[levelSelection].scene[toY][toX];

    // 检查是否能移动箱子
    if ((fromCell == BOX || fromCell == BOX_ON_POINT) &&
        (toCell == AIR || toCell == POINTS_))
    {
        // 移动箱子
        if (toCell == AIR)
        {
            template[levelSelection].scene[toY][toX] = BOX;
        }
        else if (toCell == POINTS_)
        {
            template[levelSelection].scene[toY][toX] = BOX_ON_POINT;
        }

        // 恢复原位置
        if (fromCell == BOX_ON_POINT)
        {
            template[levelSelection].scene[fromY][fromX] = POINTS_;
        }
        else
        {
            template[levelSelection].scene[fromY][fromX] = AIR;
        }

        return TRUE;
    }

    return FALSE;
}

// 检查关卡是否完成
BOOL CheckLevelComplete(void)
{
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            // 如果还有目标点没有被箱子覆盖，则关卡未完成
            if (template[levelSelection].scene[y][x] == POINTS_)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
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
    boxOnPointBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BOX)); // 可以使用同一个箱子图片，或加载专用图片

    // 创建内存设备上下文
    HDC hdc = GetDC(hWnd); // 获取屏幕设备上下文
    airDc = CreateCompatibleDC(hdc);
    wallDc = CreateCompatibleDC(hdc);
    boxDc = CreateCompatibleDC(hdc);
    pointDc = CreateCompatibleDC(hdc);
    personaDc = CreateCompatibleDC(hdc);
    boxOnPointDc = CreateCompatibleDC(hdc);

    // 将位图选入设备上下文
    SelectObject(airDc, airBmp);
    SelectObject(wallDc, wallBmp);
    SelectObject(boxDc, boxBmp);
    SelectObject(pointDc, pointBmp);
    SelectObject(personaDc, personaBmp);
    SelectObject(boxOnPointDc, boxOnPointBmp);

    ReleaseDC(hWnd, hdc); // 释放屏幕设备上下文
    return;
}

// 绘制场景
void DrawScene(HWND hWnd, HDC hdc)
{
    // 兼容性DC
    HDC memDc = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);

    // 绘制白色背景
    RECT rect = { 0, 0, 960, 960 };
    FillRect(memDc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

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
            case BOX:
                BitBlt(memDc, x * 96, y * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
                break;
            case BOX_ON_POINT:
                BitBlt(memDc, x * 96, y * 96, 96, 96, boxOnPointDc, 0, 0, SRCCOPY);
                break;
            }
        }
    }

    // 绘制人物
    BitBlt(memDc,
        template[levelSelection].personaX * 96,
        template[levelSelection].personaY * 96,
        96, 96, personaDc, 0, 0, SRCCOPY);

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
    if (airBmp) DeleteObject(airBmp);
    if (wallBmp) DeleteObject(wallBmp);
    if (boxBmp) DeleteObject(boxBmp);
    if (pointBmp) DeleteObject(pointBmp);
    if (personaBmp) DeleteObject(personaBmp);
    if (boxOnPointBmp) DeleteObject(boxOnPointBmp);

    airBmp = wallBmp = boxBmp = pointBmp = personaBmp = boxOnPointBmp = NULL;

    // 删除设备上下文
    if (airDc) DeleteDC(airDc);
    if (wallDc) DeleteDC(wallDc);
    if (boxDc) DeleteDC(boxDc);
    if (pointDc) DeleteDC(pointDc);
    if (personaDc) DeleteDC(personaDc);
    if (boxOnPointDc) DeleteDC(boxOnPointDc);

    airDc = wallDc = boxDc = pointDc = personaDc = boxOnPointDc = NULL;

    return;
}