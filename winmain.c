#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "resource.h"

// 变量声明
HINSTANCE hinstance = 0;

// 位图和设备上下文声明
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointsBmp = 0, personaBmp = 0, scoreBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointsDc = 0, personaDc = 0, scoreDc = 0;

// 关卡数据声明
size_t levelSelection = 0;
struct LEVEL
{
	unsigned char personaX, personaY;   // 人物坐标
	unsigned char scene[10][10];        // 场景数组
}
template[] =// 关卡模板数组
{   // 关卡1
    5, 4,// 人物坐标
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
    1, 1,
    {
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
	},
    // 关卡3
    1,2,
    {
		1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,1,
		1,0,2,0,0,0,0,0,0,1,
		1,1,1,1,1,3,0,0,0,1,
		1,0,0,0,0,0,0,0,0,1,
		1,0,0,0,3,1,1,1,1,1,
		1,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,2,1,
		1,1,1,1,1,1,1,1,1,1
    },
	// 关卡4
	8,1,
    {
        1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,3,0,0,0,0,0,0,1,
        1,0,0,0,1,1,1,0,0,1,
        1,1,0,0,1,0,1,0,0,1,
        1,0,0,0,1,0,1,0,0,1,
        1,0,1,1,1,0,1,0,0,1,
        1,0,0,0,0,0,3,0,0,1,
        1,2,0,0,0,0,0,0,2,1,
        1,1,1,1,1,1,1,1,1,1
	},
    // 关卡5 - 中心对称
    4, 4,
    {
        1,1,1,1,1,1,1,1,1,1,
        1,2,0,0,0,0,0,0,2,1,
        1,0,1,0,0,0,0,1,0,1,
        1,0,0,3,0,0,3,0,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,0,3,0,0,3,0,0,1,
        1,0,1,0,0,0,0,1,0,1,
        1,2,0,0,0,0,0,0,2,1,
        1,1,1,1,1,1,1,1,1,1
    },
    // 关卡6 - 走廊迷宫
    5, 4,
    {
        1,1,1,1,1,1,1,1,1,1,
        1,2,0,0,1,0,0,0,2,1,
        1,0,3,0,1,0,3,0,0,1,
        1,0,0,0,1,0,0,0,0,1,
        1,1,1,0,1,0,1,1,1,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,1,0,0,0,0,1,
        1,0,3,0,1,0,3,0,0,1,
        1,2,0,0,1,0,0,0,2,1,
        1,1,1,1,1,1,1,1,1,1
    },
    // 关卡7 - 螺旋迷宫
    4, 1,
    {
        1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,1,1,1,1,1,1,0,1,
        1,0,1,0,0,0,0,1,0,1,
        1,0,1,0,1,1,0,1,0,1,
        1,0,1,0,1,2,0,1,0,1,
        1,0,1,0,0,0,0,1,0,1,
        1,0,1,1,1,3,1,1,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1
    },
    // 关卡8 - 十字路口
    4, 4,
    {
        1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,2,0,0,0,0,2,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,3,0,0,0,1,
        1,0,0,0,3,3,3,0,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,2,0,0,0,0,2,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1
    },
    // 关卡9 - 复杂迷宫
    1, 1,
    {
        1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,1,0,0,0,0,1,
        1,0,3,0,0,0,0,3,0,1,
        1,0,0,0,1,0,0,0,0,1,
        1,1,1,0,1,1,1,1,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,1,0,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,1,
        1,2,0,0,0,0,0,0,2,1,
        1,1,1,1,1,1,1,1,1,1
    },
    // 关卡10 - 最终挑战
    4, 8,
    {
        1,1,1,1,1,1,1,1,1,1,
        1,2,0,0,0,0,0,0,2,1,
        1,0,1,1,0,0,1,1,0,1,
        1,0,1,0,0,0,0,1,0,1,
        1,0,0,0,3,3,0,0,0,1,
        1,0,1,0,0,0,0,1,0,1,
        1,0,1,1,0,0,1,1,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1
    }
};
size_t totalLevels = sizeof template / sizeof(template[0]); // 总关卡数
enum { AIR, WALL, POINTS_, BOX, PERSONA, SCORE };           // 场景元素枚举
unsigned char boxCounts = 0, scoreCounts = 0;
struct LEVEL templateBackup = {0}; // 备份

// 窗口过程函数声明
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void LoadResources(HWND);
void UpdateWindowTitle(HWND);
void DrawScene(HWND, HDC);
void ReleaseResources(void);
void MoveUp(void);
void MoveDown(void);
void MoveLeft(void);
void MoveRight(void);
void NextLevel(HWND);
void ResetCurrentLevel(HWND);
void BackupRestore(bool, size_t);

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
        UpdateWindowTitle(hWnd);
        BackupRestore(true, levelSelection);
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
        switch (wmId)
        {
        case VK_ESCAPE:// 按下Esc键时关闭窗口
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
		case VK_UP:// 角色移动控制
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
        case 'r':
        case 'R':
            ResetCurrentLevel(hWnd);
            return 0;
        }
		HDC hdc = GetDC(hWnd);
		DrawScene(hWnd, hdc);
		ReleaseDC(hWnd, hdc);
        if ((scoreCounts != 0) && (boxCounts == 0)) NextLevel(hWnd);
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
	pointsBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_POINTS_));
    personaBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_PERSONA));
	scoreBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_SCORE));
    // 创建内存设备上下文
    HDC hdc = GetDC(hWnd); // 获取屏幕设备上下文
    airDc = CreateCompatibleDC(hdc);
    wallDc = CreateCompatibleDC(hdc);
    boxDc = CreateCompatibleDC(hdc);
	pointsDc = CreateCompatibleDC(hdc);
    personaDc = CreateCompatibleDC(hdc);
	scoreDc = CreateCompatibleDC(hdc);
    // 将位图选入设备上下文
    SelectObject(airDc, airBmp);
    SelectObject(wallDc, wallBmp);
    SelectObject(boxDc, boxBmp);
    SelectObject(pointsDc, pointsBmp);
    SelectObject(personaDc, personaBmp);
	SelectObject(scoreDc, scoreBmp);

    ReleaseDC(hWnd, hdc); // 释放屏幕设备上下文
	return;
}
// 更新窗口标题显示当前关卡信息
void UpdateWindowTitle(HWND hWnd)
{
    WCHAR title[100];
    swprintf(title, 100, L"推箱子 - 内置关卡：第%zd关/共%zd关", levelSelection + 1, totalLevels);
    SetWindowText(hWnd, title);
}
// 绘制函数（示例）
void DrawScene(HWND hWnd, HDC hdc)
{
    // 兼容性DC
	HDC memDc = CreateCompatibleDC(hdc);
	HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);
	// 绘制场景
    boxCounts = 0, scoreCounts = 0;// 计算器
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
				BitBlt(memDc, x * 96, y * 96, 96, 96, pointsDc, 0, 0, SRCCOPY);
				break;
            case BOX:
                BitBlt(memDc, x * 96, y * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
                ++boxCounts;
                break;
            case SCORE:
                BitBlt(memDc, x * 96, y * 96, 96, 96, scoreDc, 0, 0, SRCCOPY);
                ++scoreCounts;
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
	return;
}
// 资源释放函数
void ReleaseResources(void)
{
    // 删除设备上下文
    if(airDc) DeleteDC(airDc);
    if(wallDc) DeleteDC(wallDc);
    if(boxDc) DeleteDC(boxDc);
    if(pointsDc) DeleteDC(pointsDc);
    if(personaDc) DeleteDC(personaDc);
	if (scoreDc) DeleteDC(scoreDc);
	personaDc = airDc = wallDc = boxDc = pointsDc = scoreDc = NULL;
    // 释放位图资源
	if(wallBmp) DeleteObject(wallBmp);
	if(boxBmp) DeleteObject(boxBmp);
	if(pointsBmp) DeleteObject(pointsBmp);
    if(airBmp) DeleteObject(airBmp);
	if(personaBmp) DeleteObject(personaBmp);
	if (scoreBmp) DeleteObject(scoreBmp);
	personaBmp = airBmp = wallBmp = boxBmp = pointsBmp = scoreBmp = NULL;
	return;
}


// 处理移动的代码
// 上移动
void MoveUp(void)
{
    // 获取当前人物坐标
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    // 判断上方是否为墙壁
    if ((POINTS_ == template[levelSelection].scene[personaY - 1][personaX]) || (AIR == template[levelSelection].scene[personaY - 1][personaX]))
    {
        --template[levelSelection].personaY;
        return;
    }
    // 判断上方是否为箱子，且箱子上方为空气或积分点
    if (BOX == template[levelSelection].scene[personaY - 1][personaX])
    {
        if (AIR == template[levelSelection].scene[personaY - 2][personaX])
        {
            template[levelSelection].scene[personaY - 2][personaX] = BOX;
            template[levelSelection].scene[personaY - 1][personaX] = AIR;
            --template[levelSelection].personaY;
            return;
        }
        if (POINTS_ == template[levelSelection].scene[personaY - 2][personaX])
        {
            template[levelSelection].scene[personaY - 2][personaX] = SCORE;
            template[levelSelection].scene[personaY - 1][personaX] = AIR;
            --template[levelSelection].personaY;
            return;
        }
        return;
    }
    // 当上方为得分时的逻辑
    if ((SCORE == template[levelSelection].scene[personaY - 1][personaX]) && (AIR == template[levelSelection].scene[personaY - 2][personaX]))
    {
        template[levelSelection].scene[personaY - 2][personaX] = BOX;
        template[levelSelection].scene[personaY - 1][personaX] = POINTS_;
        --template[levelSelection].personaY;
        return;
    }
	return;
}
// 下移动
void MoveDown(void)
{
	int personaX = template[levelSelection].personaX;
	int personaY = template[levelSelection].personaY;

    if ((POINTS_ == template[levelSelection].scene[personaY + 1][personaX]) || (AIR == template[levelSelection].scene[personaY + 1][personaX]))
    {
        ++template[levelSelection].personaY;
        return;
    }
    if (BOX == template[levelSelection].scene[personaY + 1][personaX])
    {
        if (AIR == template[levelSelection].scene[personaY + 2][personaX])
        {
            template[levelSelection].scene[personaY + 2][personaX] = BOX;
            template[levelSelection].scene[personaY + 1][personaX] = AIR;
            ++template[levelSelection].personaY;
            return;
        }
        if (POINTS_ == template[levelSelection].scene[personaY + 2][personaX])
        {
            template[levelSelection].scene[personaY + 2][personaX] = SCORE;
            template[levelSelection].scene[personaY + 1][personaX] = AIR;
            ++template[levelSelection].personaY;
            return;
        }
        return;
    }
    if ((SCORE == template[levelSelection].scene[personaY + 1][personaX]) && (AIR == template[levelSelection].scene[personaY + 2][personaX]))
    {
        template[levelSelection].scene[personaY + 2][personaX] = BOX;
        template[levelSelection].scene[personaY + 1][personaX] = POINTS_;
        ++template[levelSelection].personaY;
        return;
    }
	return;
}
// 左移动
void MoveLeft(void)
{
	int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    if ((POINTS_ == template[levelSelection].scene[personaY][personaX - 1]) || (AIR == template[levelSelection].scene[personaY][personaX - 1]))
    {
        --template[levelSelection].personaX;
        return;
    }
    if (BOX == template[levelSelection].scene[personaY][personaX - 1])
    {
        if (AIR == template[levelSelection].scene[personaY][personaX - 2])
        {
            template[levelSelection].scene[personaY][personaX - 2] = BOX;
            template[levelSelection].scene[personaY][personaX - 1] = AIR;
            --template[levelSelection].personaX;
            return;
        }
        if (POINTS_ == template[levelSelection].scene[personaY][personaX - 2])
        {
            template[levelSelection].scene[personaY][personaX - 2] = SCORE;
            template[levelSelection].scene[personaY][personaX - 1] = AIR;
            --template[levelSelection].personaX;
            return;
        }
        return;
    }
    if ((SCORE == template[levelSelection].scene[personaY][personaX - 1]) && (AIR == template[levelSelection].scene[personaY][personaX - 2]))
    {
        template[levelSelection].scene[personaY][personaX - 2] = BOX;
        template[levelSelection].scene[personaY][personaX - 1] = POINTS_;
        --template[levelSelection].personaX;
        return;
    }
	return;
}
// 右移动
void MoveRight(void)
{
	int personaX = template[levelSelection].personaX;
	int personaY = template[levelSelection].personaY;
    if ((POINTS_ == template[levelSelection].scene[personaY][personaX + 1]) || (AIR == template[levelSelection].scene[personaY][personaX + 1]))
    {
        ++template[levelSelection].personaX;
        return;
    }
    if (BOX == template[levelSelection].scene[personaY][personaX + 1])
    {
        if (AIR == template[levelSelection].scene[personaY][personaX + 2])
        {
            template[levelSelection].scene[personaY][personaX + 2] = BOX;
            template[levelSelection].scene[personaY][personaX + 1] = AIR;
            ++template[levelSelection].personaX;
            return;
        }
        if (POINTS_ == template[levelSelection].scene[personaY][personaX + 2])
        {
            template[levelSelection].scene[personaY][personaX + 2] = SCORE;
            template[levelSelection].scene[personaY][personaX + 1] = AIR;
            ++template[levelSelection].personaX;
            return;
        }
        return;
    }
    if ((SCORE == template[levelSelection].scene[personaY][personaX + 1]) && (AIR == template[levelSelection].scene[personaY][personaX + 2]))
    {
        template[levelSelection].scene[personaY][personaX + 2] = BOX;
        template[levelSelection].scene[personaY][personaX + 1] = POINTS_;
        ++template[levelSelection].personaX;
        return;
    }
	return;
}


// 切换到下一关和回到开始关
void NextLevel(HWND hWnd)
{
    if (levelSelection < totalLevels - 1)
    {
        // 数据还原
        BackupRestore(false, levelSelection);
        // 显示过关消息
        MessageBox(hWnd, L"恭喜！关卡完成！\n按下回车或空格继续", L"过关", MB_OK | MB_ICONINFORMATION);
        // 切换到下一关
        ++levelSelection;
        // 备份数据
        BackupRestore(true, levelSelection);
    }
    else
    {
        // 回到开始关卡
        BackupRestore(false, levelSelection);
        MessageBox(hWnd, L"恭喜！你已通关所有关卡！\n按下回车或空格，回到初始关卡", L"游戏通关", MB_OK | MB_ICONINFORMATION);
        levelSelection = 0;
        BackupRestore(true, levelSelection);
    }
        // 更新窗口标题
        UpdateWindowTitle(hWnd);
        // 重绘场景
        InvalidateRect(hWnd, NULL, TRUE);
    return;
}
// 重置当前关卡
void ResetCurrentLevel(HWND hWnd)
{
    BackupRestore(false, levelSelection);
    // 重绘场景
    InvalidateRect(hWnd, NULL, TRUE);
    MessageBox(hWnd, L"当前关卡已重置", L"重置", MB_OK | MB_ICONINFORMATION);
}
// 备份与还原
void BackupRestore(bool cOondition, size_t lEvelsElection)
{
    if (cOondition)// 进行备份
    {
        templateBackup = template[lEvelsElection];
        return;
    }
    else if (!cOondition)// 进行还原
    {
        template[lEvelsElection] = templateBackup;
        return;
    }
    return;
}