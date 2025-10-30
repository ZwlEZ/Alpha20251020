#include <windows.h>
#include <imm.h>  // ������뷨֧��ͷ�ļ�
#pragma comment(lib, "imm32.lib")  // �������뷨��

#include "resource.h"

// ��������
HINSTANCE hinstance = 0;

// λͼ���豸����������
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0;

// �ؿ���������
int levelSelection = 0;
struct LEVEL
{
    unsigned char personaX, personaY;   // ��������
    unsigned char pointsX, pointsY;     // ���ֵ�����
    unsigned char pointsX_2, pointsY_2; // �ڶ������ֵ�����
    unsigned char boxX, boxY;           // ��������
    unsigned char boxX_2, boxY_2;       // �ڶ�����������
    unsigned char scene[10][10];        // ��������
}
template[] =// �ؿ�ģ������
{   // �ؿ�1
    5, 4,// ��������
    2, 2,// ���ֵ�����
    7, 7,// �ڶ������ֵ�����
    5, 5,// ��������
    4, 6,// �ڶ�����������
    {// ���������ִ�����Ԫ�أ�1Ϊǽ�ڣ�0Ϊ������3Ϊ���ӣ�2Ϊ���ֵ㣬4Ϊ���һ��������ڵ�ֵ��
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
    // �ؿ�2
    1, 1,// ��������
    8, 8,// ���ֵ�����
    1, 8,// �ڶ������ֵ�����
    2, 2,// ��������
    6, 7,// �ڶ�����������
    {// ���������ִ�����Ԫ�أ�1Ϊǽ�ڣ�0Ϊ������3Ϊ���ӣ�2Ϊ���ֵ㣬4Ϊ���һ��������ڵ�ֵ��
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
enum { AIR, WALL, POINTS_, BOX, PERSONA };// ����Ԫ��ö��

// ���ڹ��̺�������
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

// ������ڵ�
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
        0, L"Sokoban", L"������",
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

// ���ڹ��̺���
LRESULT CALLBACK WindowProc(HWND hWnd, UINT mSgId, WPARAM wParam, LPARAM lParam)
{
    switch (mSgId)
    {
    case WM_CREATE:
        LoadResources(hWnd);
        // ����IME���뷨�����ǹؼ����裡
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

    case WM_CHAR:  // ʹ��WM_CHAR��Ϣ��������������뷨��Ҳ����������
    {
        switch (wParam)
        {
        case 'w':
        case 'W':
        case 0x1A:  // �������뷨�µ�W
            MoveUp();
            break;
        case 's':
        case 'S':
        case 0x13:  // �������뷨�µ�S
            MoveDown();
            break;
        case 'a':
        case 'A':
        case 0x01:  // �������뷨�µ�A
            MoveLeft();
            break;
        case 'd':
        case 'D':
        case 0x04:  // �������뷨�µ�D
            MoveRight();
            break;
        case 'r':
        case 'R':
            // ���ùؿ��߼��������������
            break;
        }

        if (CheckLevelComplete())
        {
            MessageBox(hWnd, L"��ϲ���ؿ���ɣ�", L"����", MB_OK);
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

// ��Դ���غ���
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

// ���Ƴ���
void DrawScene(HWND hWnd, HDC hdc)
{
    HDC memDc = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);

    // ���ư�ɫ����
    RECT rect = { 0, 0, 960, 960 };
    HBRUSH whiteBrush = GetStockObject(WHITE_BRUSH);
    FillRect(memDc, &rect, whiteBrush);

    // ���Ƴ���
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

    // ��������
    BitBlt(memDc, template[levelSelection].boxX * 96, template[levelSelection].boxY * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
    BitBlt(memDc, template[levelSelection].boxX_2 * 96, template[levelSelection].boxY_2 * 96, 96, 96, boxDc, 0, 0, SRCCOPY);

    // ��������
    BitBlt(memDc, template[levelSelection].personaX * 96, template[levelSelection].personaY * 96, 96, 96, personaDc, 0, 0, SRCCOPY);

    // ���ڴ�DC���ݸ��Ƶ�����DC
    BitBlt(hdc, 0, 0, 960, 960, memDc, 0, 0, SRCCOPY);

    // ����
    DeleteObject(memBmp);
    DeleteDC(memDc);
}

// ��Դ�ͷź���
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

// ���ؿ��Ƿ����
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

// ֻ������Ϸ���򣬱�����˸
void UpdateGameArea(HWND hWnd)
{
    RECT gameRect = { 0, 0, 960, 960 };
    InvalidateRect(hWnd, &gameRect, FALSE);
    UpdateWindow(hWnd);
}

// �ƶ��������ֲ���
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