#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <windows.h>
#include <string>

using namespace std;

char proc_priority_info[100];
char virtualization_info[100];

DWORD WINAPI Thread(void*)
{
    // Подключение библиотеки 
    HINSTANCE hinstLib = LoadLibrary(TEXT("info.dll"));

    //Подключаем функцию, определяющую приоритет текущего процесса
    typedef DWORD(*GPP)();
    GPP GetProcessPriority;
    GetProcessPriority = (GPP)GetProcAddress(hinstLib, "GetProcessPriority");

    //Подключаем функцию, определяющую наличие поддержки виртуального режима процессора
    typedef BOOL(*SV)();
    SV SupportVirtualization = (SV)GetProcAddress(hinstLib, "SupportVirtualization");

    // Сохраняем полученную информацию в глобальных переменных
    sprintf(proc_priority_info, "Приоритет текущего процесса: %s", GetProcessPriority());
    sprintf(virtualization_info, "Наличие поддержки виртуального режима работы процессора: %s", SupportVirtualization() ? "Да" : "Нет");

    FreeLibrary(hinstLib); //Отключаем библиотеку
    return 1;
}

//Определение внутренних параметров окна
LRESULT CALLBACK Windows_Create(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hDC;

    //Определяем параметры текста
    HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Helvetica"));
    switch (msg)
    {
    case WM_CREATE: // Создание потока
        HANDLE hThread;
        DWORD IDThread;
        hThread = CreateThread(NULL, 0, Thread, NULL, 0, &IDThread);
        WaitForSingleObject(hThread, INFINITE); // Ожидание завершения потока
        CloseHandle(hThread); // Удаление дескриптора потока
        break;
    case WM_DESTROY: //Закрываем окно
        PostQuitMessage(0);
        break;
    case WM_PAINT: // Инициализация контекста устройства
        hDC = BeginPaint(hWnd, &ps);
        SelectObject(hDC, hFont);
        SetTextColor(hDC, RGB(24, 10, 210)); //установка цвета текста
        TextOutA(hDC, 15, 17, proc_priority_info, strlen(proc_priority_info)); //Вывод информации в окно
        TextOutA(hDC, 15, 50, virtualization_info, strlen(virtualization_info)); //Вывод информации в окно
        TextOutA(hDC, 15, 150, "Автор: Панасенко Сергей, ПМИ-12", strlen("Автор: Панасенко Сергей, ПМИ-12")); //Вывод информации в окно
        EndPaint(hWnd, &ps);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

//Главная функция
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR str, int nWinMode)
{
	MSG msg;
	WNDCLASS wcl;//Указатель на экземпляры класса окна
	HWND hWnd;	 // Дескриптор окна
	LPCWSTR szClassName = L"ResourceProject";

	// Параметры класса окна
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

	// В случае отсутствия регистрации класса
	if (!RegisterClass(&wcl))
	{
		MessageBox(NULL, L"Не удалось зарегистрировать класс!", L"ERROR", MB_OK);
		return NULL;//выход 
	}

	// Создание окна
	hWnd = CreateWindow(szClassName, L"Индивидуальная работа по курсу УРВВС",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 500, 500, 650, 250, HWND_DESKTOP, NULL, hInstance, NULL);

	// В случае некорректного создания окна
	if (!hWnd)
	{
		MessageBox(NULL, L"Не удалось создать окно!", L"ERROR", MB_OK);
		return NULL;//выход 
	}

	// Отображение окна 
	ShowWindow(hWnd, nWinMode);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) // Пока получаем сообщения
		DispatchMessage(&msg); //Передача сообщения в нужную оконную процедуру

	return msg.wParam;
}