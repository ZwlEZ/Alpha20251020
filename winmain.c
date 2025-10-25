#include <windows.h>

#include "resource.h"

// ��������
HINSTANCE hIns;
HBITMAP hBmp = NULL;
HDC hdc, bmpdc;
int score = 0;
HWND hScoreLabel = NULL;

// ���ڹ��̺�������
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void OnButtonClick(HWND, LPARAM);

// ������ڵ�
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	hIns = hInstance;// ����ʵ�����
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
    wcex.lpszClassName = L"MyWindow";      			                // ��������
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // �Զ���Сͼ��
	// ע�ᴰ����
    if (!RegisterClassEx(&wcex))
        return 0;
	// ��������
	HWND hWnd = CreateWindowEx(
            0,                              // ��չ��ʽ
            L"MyWindow",                    // ��������
            L"��ã�Windows",               // ���ڱ���
            WS_OVERLAPPEDWINDOW,            // ���ڷ��
            CW_USEDEFAULT, CW_USEDEFAULT,   // ��ʼλ��
            800, 600,                       // ��ʼ��С
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
    {
        OnButtonClick(hWnd, lParam);
    }
        return 0;

    case WM_PAINT:// ���ƴ�������
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // �ڴ˴���ӻ�ͼ����

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
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

    case WM_COMMAND:// ����˵��Ϳؼ�����
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case 1: // ��ťID
            score++;
            // ���·�����ʾ
            wchar_t buf[32];
            wsprintf(buf, L"����: %d", score);
            SetWindowText(hScoreLabel, buf);
            return 0;
        default:
            return DefWindowProc(hWnd, mSgId, wParam, lParam);
        }
    }
        return 0;

    case WM_DESTROY:// ��������ʱ���������
        
        PostQuitMessage(0);
        return 0;
    }
    // ����δ�������Ϣ����Ĭ�ϴ��ڹ���
    return DefWindowProc(hWnd, mSgId, wParam, lParam);
}

// ��ť����
void OnButtonClick(HWND hWnd, LPARAM lParam)
{
    static HWND hButton = NULL;
    if (!hButton) {
        hButton = CreateWindow(
            L"BUTTON", L"�����",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30,
            hWnd, (HMENU)1, hIns, NULL
        );
        // ����������ʾ��ǩ
        hScoreLabel = CreateWindow(
            L"STATIC", L"����: 0",
            WS_VISIBLE | WS_CHILD,
            120, 10, 100, 30,
            hWnd, NULL, hIns, NULL
        );
    }
}