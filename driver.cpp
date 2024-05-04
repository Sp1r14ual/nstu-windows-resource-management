#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <windows.h>
#include <string>

using namespace std;

char proc_priority_info[100];
char virtualization_info[100];

DWORD WINAPI Thread(void*)
{
    // ����������� ���������� 
    HINSTANCE hinstLib = LoadLibrary(TEXT("info.dll"));

    //���������� �������, ������������ ��������� �������� ��������
    typedef DWORD(*GPP)();
    GPP GetProcessPriority;
    GetProcessPriority = (GPP)GetProcAddress(hinstLib, "GetProcessPriority");

    //���������� �������, ������������ ������� ��������� ������������ ������ ����������
    typedef BOOL(*SV)();
    SV SupportVirtualization = (SV)GetProcAddress(hinstLib, "SupportVirtualization");

    // ��������� ���������� ���������� � ���������� ����������
    sprintf(proc_priority_info, "��������� �������� ��������: %s", GetProcessPriority());
    sprintf(virtualization_info, "������� ��������� ������������ ������ ������ ����������: %s", SupportVirtualization() ? "��" : "���");

    FreeLibrary(hinstLib); //��������� ����������
    return 1;
}

//����������� ���������� ���������� ����
LRESULT CALLBACK Windows_Create(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hDC;

    //���������� ��������� ������
    HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Helvetica"));
    switch (msg)
    {
    case WM_CREATE: // �������� ������
        HANDLE hThread;
        DWORD IDThread;
        hThread = CreateThread(NULL, 0, Thread, NULL, 0, &IDThread);
        WaitForSingleObject(hThread, INFINITE); // �������� ���������� ������
        CloseHandle(hThread); // �������� ����������� ������
        break;
    case WM_DESTROY: //��������� ����
        PostQuitMessage(0);
        break;
    case WM_PAINT: // ������������� ��������� ����������
        hDC = BeginPaint(hWnd, &ps);
        SelectObject(hDC, hFont);
        SetTextColor(hDC, RGB(24, 10, 210)); //��������� ����� ������
        TextOutA(hDC, 15, 17, proc_priority_info, strlen(proc_priority_info)); //����� ���������� � ����
        TextOutA(hDC, 15, 50, virtualization_info, strlen(virtualization_info)); //����� ���������� � ����
        TextOutA(hDC, 15, 150, "�����: ��������� ������, ���-12", strlen("�����: ��������� ������, ���-12")); //����� ���������� � ����
        EndPaint(hWnd, &ps);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

//������� �������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR str, int nWinMode)
{
	MSG msg;
	WNDCLASS wcl;//��������� �� ���������� ������ ����
	HWND hWnd;	 // ���������� ����
	LPCWSTR szClassName = L"ResourceProject";

	// ��������� ������ ����
	wcl.hInstance = hInstance;
	wcl.lpszClassName = szClassName;
	wcl.lpfnWndProc = Windows_Create;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.hIcon = NULL;
	wcl.hCursor = NULL;
	wcl.lpszMenuName = NULL;
	wcl.cbClsExtra = NULL;	
	wcl.cbWndExtra = NULL;
	wcl.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

	// � ������ ���������� ����������� ������
	if (!RegisterClass(&wcl))
	{
		MessageBox(NULL, L"�� ������� ���������������� �����!", L"ERROR", MB_OK);
		return NULL;//����� 
	}

	// �������� ����
	hWnd = CreateWindow(szClassName, L"�������������� ������ �� ����� �����",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 500, 500, 650, 250, HWND_DESKTOP, NULL, hInstance, NULL);

	// � ������ ������������� �������� ����
	if (!hWnd)
	{
		MessageBox(NULL, L"�� ������� ������� ����!", L"ERROR", MB_OK);
		return NULL;//����� 
	}

	// ����������� ���� 
	ShowWindow(hWnd, nWinMode);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) // ���� �������� ���������
		DispatchMessage(&msg); //�������� ��������� � ������ ������� ���������

	return msg.wParam;
}