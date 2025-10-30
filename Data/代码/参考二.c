#include <windows.h>

#include "resource.h"

// ��������
HINSTANCE hinstance = 0;

// λͼ���豸����������
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0;

// �ؿ���������
int levelSelection = 0; // ��Ϊ��0��ʼ
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
    hinstance = hInstance;// ����ʵ�����
    // ���岢��ʼ��������
    WNDCLASSEX wcex = { 0 };
    // �Խṹ�����Ա��ʼ��
    wcex.cbSize = sizeof(WNDCLASSEX);          		                // �ṹ��С
    wcex.style = CS_HREDRAW | CS_VREDRAW;      		                // ������ʽ����ˮƽ��ֱ����仯ʱ�ػ�
    wcex.lpfnWndProc = WindowProc;             			            // ���ڹ��̺������ص���������
    wcex.hInstance = hInstance;                		                // ʵ�����
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); 	// �Զ���ͼ��
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   	                // ��꣺ʹ�ñ�׼��ͷ���
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); 	            // ������ʹ��ϵͳ������ɫ
    wcex.lpszClassName = L"Sokoban";      			                // ��������
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // �Զ���Сͼ��
    // ע�ᴰ����
    if (!RegisterClassEx(&wcex))
        return 0;
    // ��ȡ��Ļ���
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // ���ô��ڿ��
    int winWidth = 976;
    int winHeight = 999;
    // �������λ��
    int x = (screenWidth - winWidth) / 2;
    int y = (screenHeight - winHeight) / 2;
    // ��������
    HWND hWnd = CreateWindowEx(
        0,                              // ��չ��ʽ
        L"Sokoban",                     // ��������
        L"������",                      // ���ڱ���
        WS_OVERLAPPEDWINDOW,            // ���ڷ��
        x, y,                           // ��ʼλ��
        winWidth, winHeight,            // ��ʼ��С
        NULL,                           // �����ھ��
        NULL,                           // �˵����
        hInstance,                      // ʵ�����
        NULL                            // ���Ӳ���
    );
    if (!hWnd)
        return 0;
    // ��ʾ�͸��´���
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    // ��Ϣѭ��
    MSG msg;// ������Ϣ�ṹ�����
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
// ���ڹ��̺�������������Ϣ���ص�������
LRESULT CALLBACK WindowProc(HWND hWnd, UINT mSgId, WPARAM wParam, LPARAM lParam)
{
    switch (mSgId)//�����ĵڶ�������
    {
    case WM_CREATE:// ���ڴ���ʱ�ĳ�ʼ������
        LoadResources(hWnd);
        return 0;

    case WM_PAINT:// ���ƴ�������
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // �ڴ˴���ӻ�ͼ����
        DrawScene(hWnd, hdc);
        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_SIZE:// �����ڴ�С�ı�
        return 0;

    case WM_KEYDOWN:// ������̰����¼�
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case VK_ESCAPE:// ����Esc��ʱ�رմ���
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
        case VK_UP:// ��ɫ�ƶ�����
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
        case 'r': // ���ùؿ�
            // �������������ùؿ����߼�
            break;
        }

        // ����Ƿ���ɹؿ�
        if (CheckLevelComplete())
        {
            MessageBox(hWnd, L"��ϲ���ؿ���ɣ�", L"����", MB_OK);
            // ������������ӽ�����һ�ص��߼�
        }

        // ֻ������Ϸ���򣬱�����˸
        UpdateGameArea(hWnd);
    }
    return 0;

    case WM_COMMAND:// ����˵��Ϳؼ�����
        return 0;

    case WM_DESTROY:// ��������ʱ���������
        ReleaseResources();
        PostQuitMessage(0);
        return 0;
    }
    // ����δ�������Ϣ����Ĭ�ϴ��ڹ���
    return DefWindowProc(hWnd, mSgId, wParam, lParam);
}

// ��Դ���غ���
void LoadResources(HWND hWnd)
{
    // ����λͼ��Դ
    airBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_AIR));
    wallBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_WALLS));
    boxBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BOX));
    pointBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_POINTS_));
    personaBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_PERSONA));
    // �����ڴ��豸������
    HDC hdc = GetDC(hWnd); // ��ȡ��Ļ�豸������
    airDc = CreateCompatibleDC(hdc);
    wallDc = CreateCompatibleDC(hdc);
    boxDc = CreateCompatibleDC(hdc);
    pointDc = CreateCompatibleDC(hdc);
    personaDc = CreateCompatibleDC(hdc);
    // ��λͼѡ���豸������
    SelectObject(airDc, airBmp);
    SelectObject(wallDc, wallBmp);
    SelectObject(boxDc, boxBmp);
    SelectObject(pointDc, pointBmp);
    SelectObject(personaDc, personaBmp);

    ReleaseDC(hWnd, hdc); // �ͷ���Ļ�豸������
    return;
}

// ���ƺ���
void DrawScene(HWND hWnd, HDC hdc)
{
    // ������DC
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
    return;
}

// ��Դ�ͷź���
void ReleaseResources(void)
{
    // �ͷ�λͼ��Դ
    if (wallBmp) DeleteObject(wallBmp);
    if (boxBmp) DeleteObject(boxBmp);
    if (pointBmp) DeleteObject(pointBmp);
    if (airBmp) DeleteObject(airBmp);
    if (personaBmp) DeleteObject(personaBmp);
    personaBmp = airBmp = wallBmp = boxBmp = pointBmp = NULL;
    // ɾ���豸������
    if (airDc) DeleteDC(airDc);
    if (wallDc) DeleteDC(wallDc);
    if (boxDc) DeleteDC(boxDc);
    if (pointDc) DeleteDC(pointDc);
    if (personaDc) DeleteDC(personaDc);
    personaDc = airDc = wallDc = boxDc = pointDc = NULL;
    return;
}

// ���ؿ��Ƿ����
BOOL CheckLevelComplete(void)
{
    // ����һ�������Ƿ���Ŀ�����
    BOOL box1OnPoint = (template[levelSelection].boxX == template[levelSelection].pointsX &&
        template[levelSelection].boxY == template[levelSelection].pointsY) ||
        (template[levelSelection].boxX == template[levelSelection].pointsX_2 &&
            template[levelSelection].boxY == template[levelSelection].pointsY_2);

    // ���ڶ��������Ƿ���Ŀ�����
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

// �����ƶ��Ĵ���
// ���ƶ�
void MoveUp(void)
{
    // ��ȡ��ǰ�������������
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    // �ж��Ϸ��Ƿ�Ϊǽ��
    if (template[levelSelection].scene[personaY - 1][personaX] == WALL)
        return;

    // �ж��Ϸ��Ƿ�Ϊ���ӣ��������Ϸ�Ϊ��������ֵ�
    if ((boxX == personaX) && (boxY == personaY - 1))
    {
        if (template[levelSelection].scene[boxY - 1][boxX] == WALL)
            return;

        // �ƶ���һ������
        template[levelSelection].boxY--;
        // �ƶ�����
        template[levelSelection].personaY--;
    }
    else if ((boxX_2 == personaX) && (boxY_2 == personaY - 1))
    {
        if (template[levelSelection].scene[boxY_2 - 1][boxX_2] == WALL)
            return;

        // �ƶ��ڶ�������
        template[levelSelection].boxY_2--;
        // �ƶ�����
        template[levelSelection].personaY--;
    }
    else
    {
        // �Ϸ��������ӣ�ֱ���ƶ�����
        template[levelSelection].personaY--;
    }
    return;
}

// ���ƶ�
void MoveDown(void)
{
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    // �ж��·��Ƿ�Ϊǽ��
    if (template[levelSelection].scene[personaY + 1][personaX] == WALL)
        return;

    // �ж��·��Ƿ�Ϊ����
    if ((boxX == personaX) && (boxY == personaY + 1))
    {
        if (template[levelSelection].scene[boxY + 1][boxX] == WALL)
            return;

        // �ƶ���һ������
        template[levelSelection].boxY++;
        // �ƶ�����
        template[levelSelection].personaY++;
    }
    else if ((boxX_2 == personaX) && (boxY_2 == personaY + 1))
    {
        if (template[levelSelection].scene[boxY_2 + 1][boxX_2] == WALL)
            return;

        // �ƶ��ڶ�������
        template[levelSelection].boxY_2++;
        // �ƶ�����
        template[levelSelection].personaY++;
    }
    else
    {
        // �·��������ӣ�ֱ���ƶ�����
        template[levelSelection].personaY++;
    }
    return;
}

// ���ƶ�
void MoveLeft(void)
{
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    // �ж�����Ƿ�Ϊǽ��
    if (template[levelSelection].scene[personaY][personaX - 1] == WALL)
        return;

    // �ж�����Ƿ�Ϊ����
    if ((boxX == personaX - 1) && (boxY == personaY))
    {
        if (template[levelSelection].scene[boxY][boxX - 1] == WALL)
            return;

        // �ƶ���һ������
        template[levelSelection].boxX--;
        // �ƶ�����
        template[levelSelection].personaX--;
    }
    else if ((boxX_2 == personaX - 1) && (boxY_2 == personaY))
    {
        if (template[levelSelection].scene[boxY_2][boxX_2 - 1] == WALL)
            return;

        // �ƶ��ڶ�������
        template[levelSelection].boxX_2--;
        // �ƶ�����
        template[levelSelection].personaX--;
    }
    else
    {
        // ��಻�����ӣ�ֱ���ƶ�����
        template[levelSelection].personaX--;
    }
    return;
}

// ���ƶ�
void MoveRight(void)
{
    int personaX = template[levelSelection].personaX;
    int personaY = template[levelSelection].personaY;
    int boxX = template[levelSelection].boxX;
    int boxY = template[levelSelection].boxY;
    int boxX_2 = template[levelSelection].boxX_2;
    int boxY_2 = template[levelSelection].boxY_2;

    // �޸�����ȷ����������Ӧ���� [personaY][personaX + 1]
    if (template[levelSelection].scene[personaY][personaX + 1] == WALL)
        return;

    // �ж��Ҳ��Ƿ�Ϊ����
    if ((boxX == personaX + 1) && (boxY == personaY))
    {
        if (template[levelSelection].scene[boxY][boxX + 1] == WALL)
            return;

        // �ƶ���һ������
        template[levelSelection].boxX++;
        // �ƶ�����
        template[levelSelection].personaX++;
    }
    else if ((boxX_2 == personaX + 1) && (boxY_2 == personaY))
    {
        if (template[levelSelection].scene[boxY_2][boxX_2 + 1] == WALL)
            return;

        // �ƶ��ڶ�������
        template[levelSelection].boxX_2++;
        // �ƶ�����
        template[levelSelection].personaX++;
    }
    else
    {
        // �Ҳ಻�����ӣ�ֱ���ƶ�����
        template[levelSelection].personaX++;
    }
    return;
}