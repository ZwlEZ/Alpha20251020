#include <windows.h>
#include "resource.h"

// ��������
HINSTANCE hinstance = 0;

// λͼ���豸����������
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0, boxOnPointBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0, boxOnPointDc = 0;

// ����Ԫ��ö��
enum { AIR, WALL, POINTS_, BOX, PERSONA, BOX_ON_POINT };

// �ؿ���������
int levelSelection = 0;
int totalLevels = 2;
struct LEVEL
{
    unsigned char personaX, personaY;   // ��������
    unsigned char scene[10][10];        // ��������
}
template[] = {// �ؿ�ģ������
    // �ؿ�1
    {
        5, 4, // ��������
        {// ���������ִ�����Ԫ�أ�1Ϊǽ�ڣ�0Ϊ������3Ϊ���ӣ�2Ϊ���ֵ�
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
    // �ؿ�2
    {
        1, 1, // ��������
        {// ���������ִ�����Ԫ�أ�1Ϊǽ�ڣ�0Ϊ������3Ϊ���ӣ�2Ϊ���ֵ�
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

// ���ڹ��̺�������
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void LoadResources(HWND);
void DrawScene(HWND, HDC);
void ReleaseResources(void);
BOOL MoveBox(int fromX, int fromY, int toX, int toY);
BOOL CheckLevelComplete(void);
void ResetLevel(void);
void LoadLevel(int level);

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
        LoadLevel(levelSelection);
        return 0;

    case WM_PAINT:// ���ƴ�������
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ������Ϸ����
        DrawScene(hWnd, hdc);

        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_SIZE:// �����ڴ�С�ı�
        return 0;

    case WM_KEYDOWN:// ������̰����¼�
    {
        int wmId = LOWORD(wParam);
        int x = template[levelSelection].personaX;
        int y = template[levelSelection].personaY;
        int newX = x, newY = y;
        int boxX, boxY;

        switch (wmId)
        {
        case VK_ESCAPE:// ����Esc��ʱ�رմ���
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;

        case 'R':// ���õ�ǰ�ؿ�
        case 'r':
            ResetLevel();
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;

        case VK_UP:// ��ɫ�ƶ�����
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

        // ����ƶ��Ƿ���Ч
        if (newX >= 0 && newX < 10 && newY >= 0 && newY < 10)
        {
            int targetCell = template[levelSelection].scene[newY][newX];

            if (targetCell == WALL)
            {
                // ����ǽ�ڣ������ƶ�
                break;
            }
            else if (targetCell == BOX || targetCell == BOX_ON_POINT)
            {
                // �����ƶ�����
                boxX = newX + (newX - x);
                boxY = newY + (newY - y);

                // ��������ܷ��ƶ�
                if (boxX >= 0 && boxX < 10 && boxY >= 0 && boxY < 10)
                {
                    int boxTargetCell = template[levelSelection].scene[boxY][boxX];

                    if (boxTargetCell == AIR || boxTargetCell == POINTS_)
                    {
                        // �ƶ�����
                        if (MoveBox(newX, newY, boxX, boxY))
                        {
                            // �����ƶ��ɹ��������ƶ������ӵ�λ��
                            template[levelSelection].personaX = newX;
                            template[levelSelection].personaY = newY;
                        }
                    }
                }
            }
            else
            {
                // �ƶ����յػ�Ŀ���
                template[levelSelection].personaX = newX;
                template[levelSelection].personaY = newY;
            }

            // ���ؿ��Ƿ����
            if (CheckLevelComplete())
            {
                MessageBox(hWnd, L"��ϲ���ؿ���ɣ�", L"����", MB_OK | MB_ICONINFORMATION);
                // ������һ��
                levelSelection = (levelSelection + 1) % totalLevels;
                LoadLevel(levelSelection);
            }

            // �ػ泡��
            InvalidateRect(hWnd, NULL, TRUE);
        }
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

// ���عؿ�
void LoadLevel(int level)
{
    if (level >= 0 && level < totalLevels)
    {
        levelSelection = level;
        // ���������Ӹ���ؿ���ʼ���߼�
    }
}

// ���õ�ǰ�ؿ�
void ResetLevel(void)
{
    // ���¼��ص�ǰ�ؿ�
    LoadLevel(levelSelection);
}

// �ƶ�����
BOOL MoveBox(int fromX, int fromY, int toX, int toY)
{
    if (fromX < 0 || fromX >= 10 || fromY < 0 || fromY >= 10 ||
        toX < 0 || toX >= 10 || toY < 0 || toY >= 10)
    {
        return FALSE;
    }

    int fromCell = template[levelSelection].scene[fromY][fromX];
    int toCell = template[levelSelection].scene[toY][toX];

    // ����Ƿ����ƶ�����
    if ((fromCell == BOX || fromCell == BOX_ON_POINT) &&
        (toCell == AIR || toCell == POINTS_))
    {
        // �ƶ�����
        if (toCell == AIR)
        {
            template[levelSelection].scene[toY][toX] = BOX;
        }
        else if (toCell == POINTS_)
        {
            template[levelSelection].scene[toY][toX] = BOX_ON_POINT;
        }

        // �ָ�ԭλ��
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

// ���ؿ��Ƿ����
BOOL CheckLevelComplete(void)
{
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            // �������Ŀ���û�б����Ӹ��ǣ���ؿ�δ���
            if (template[levelSelection].scene[y][x] == POINTS_)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
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
    boxOnPointBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BOX)); // ����ʹ��ͬһ������ͼƬ�������ר��ͼƬ

    // �����ڴ��豸������
    HDC hdc = GetDC(hWnd); // ��ȡ��Ļ�豸������
    airDc = CreateCompatibleDC(hdc);
    wallDc = CreateCompatibleDC(hdc);
    boxDc = CreateCompatibleDC(hdc);
    pointDc = CreateCompatibleDC(hdc);
    personaDc = CreateCompatibleDC(hdc);
    boxOnPointDc = CreateCompatibleDC(hdc);

    // ��λͼѡ���豸������
    SelectObject(airDc, airBmp);
    SelectObject(wallDc, wallBmp);
    SelectObject(boxDc, boxBmp);
    SelectObject(pointDc, pointBmp);
    SelectObject(personaDc, personaBmp);
    SelectObject(boxOnPointDc, boxOnPointBmp);

    ReleaseDC(hWnd, hdc); // �ͷ���Ļ�豸������
    return;
}

// ���Ƴ���
void DrawScene(HWND hWnd, HDC hdc)
{
    // ������DC
    HDC memDc = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);

    // ���ư�ɫ����
    RECT rect = { 0, 0, 960, 960 };
    FillRect(memDc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

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
            case BOX:
                BitBlt(memDc, x * 96, y * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
                break;
            case BOX_ON_POINT:
                BitBlt(memDc, x * 96, y * 96, 96, 96, boxOnPointDc, 0, 0, SRCCOPY);
                break;
            }
        }
    }

    // ��������
    BitBlt(memDc,
        template[levelSelection].personaX * 96,
        template[levelSelection].personaY * 96,
        96, 96, personaDc, 0, 0, SRCCOPY);

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
    if (airBmp) DeleteObject(airBmp);
    if (wallBmp) DeleteObject(wallBmp);
    if (boxBmp) DeleteObject(boxBmp);
    if (pointBmp) DeleteObject(pointBmp);
    if (personaBmp) DeleteObject(personaBmp);
    if (boxOnPointBmp) DeleteObject(boxOnPointBmp);

    airBmp = wallBmp = boxBmp = pointBmp = personaBmp = boxOnPointBmp = NULL;

    // ɾ���豸������
    if (airDc) DeleteDC(airDc);
    if (wallDc) DeleteDC(wallDc);
    if (boxDc) DeleteDC(boxDc);
    if (pointDc) DeleteDC(pointDc);
    if (personaDc) DeleteDC(personaDc);
    if (boxOnPointDc) DeleteDC(boxOnPointDc);

    airDc = wallDc = boxDc = pointDc = personaDc = boxOnPointDc = NULL;

    return;
}