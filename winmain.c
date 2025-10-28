#include <windows.h>

#include "resource.h"

// ��������
HINSTANCE hinstance = 0;

// λͼ���豸����������
HBITMAP airBmp = 0, wallBmp = 0, boxBmp = 0, pointBmp = 0, personaBmp = 0;
HDC airDc = 0, wallDc = 0, boxDc = 0, pointDc = 0, personaDc = 0;

// �ؿ���������
int levelSelection = 1;
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
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
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
        switch (wmId)
        {
        case VK_ESCAPE:// ����Esc��ʱ�رմ���
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
		case VK_UP:// ��ɫ�ƶ�����
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
// ���ƺ�����ʾ����
void DrawScene(HWND hWnd, HDC hdc)
{
    // ������DC
	HDC memDc = CreateCompatibleDC(hdc);
	HBITMAP memBmp = CreateCompatibleBitmap(hdc, 960, 960);
    SelectObject(memDc, memBmp);
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
    BitBlt(memDc, template[levelSelection].personaX * 96, template[levelSelection].personaY * 96, 96, 96, personaDc, 0, 0, SRCCOPY);
    // ��������
	BitBlt(memDc, template[levelSelection].boxX * 96, template[levelSelection].boxY * 96, 96, 96, boxDc, 0, 0, SRCCOPY);
	BitBlt(memDc, template[levelSelection].boxX_2 * 96, template[levelSelection].boxY_2 * 96, 96, 96, boxDc, 0, 0, SRCCOPY);

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
    // ɾ���豸������
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