#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <windows.h>
#include <stdint.h>
#include <string.h>

// Производитель процессора
char cpu_vendor[13];

// Функция для получения приоритета текущего процесса
extern "C" __declspec(dllexport) char* GetProcessPriority()
{
	//Получаем приоритет текущего процесса в целочисленном виде
	DWORD process_priority = GetPriorityClass(GetCurrentProcess());

	//Сопоставляем целочисленную константу приоритета с текстовым описанием
	switch(process_priority)
	{
		case ABOVE_NORMAL_PRIORITY_CLASS:
			return "Выше среднего";
		case BELOW_NORMAL_PRIORITY_CLASS:
			return "Ниже среднего";
		case HIGH_PRIORITY_CLASS:
			return "Высокий";
		case IDLE_PRIORITY_CLASS:
			return "Отложенный";
		case NORMAL_PRIORITY_CLASS:
			return "Обычный";
		case REALTIME_PRIORITY_CLASS:
			return "Максимальный";	
		default:
			return "Ошибка";	
	}
}

//Функция для вызова CPUID со значением EAX = 1
void mycpuid(int x[4], int leaf = 1)
{
    int ieax, iebx, iecx, iedx;

	__asm
	{
		mov eax, leaf
		cpuid
		mov ieax, eax
		mov iebx, ebx
		mov iecx, ecx
		mov iedx, edx
	}

	x[0] = ieax; x[1] = iebx; x[2] = iecx; x[3] = iedx;
}

//Функция для считывания производителя процессора
void get_cpu_vendor()
{
	// CPUID со значением EAX = 0
	__asm
	{
		mov eax, 0
		cpuid
		mov dword ptr[cpu_vendor], ebx
		mov dword ptr[cpu_vendor + 4], edx
		mov dword ptr[cpu_vendor + 8], ecx
	}

	cpu_vendor[12] = 0;
}


extern "C" __declspec(dllexport) BOOL SupportVirtualization()
{
	int x[4];
	uint32_t res;

	//Функция считывания производителя процессора
	get_cpu_vendor();

	//CPUID с параметром EAX = 1
	mycpuid(x, 1);

	//Сохраняем значение регистра ECX
	res = static_cast<uint32_t>(x[2]);

	//Если Intel, проверяем 5 бит
	if (strcmp(cpu_vendor, "GenuineIntel") == 0)
		return res & 0x20;
	//Если AMD, проверяем 2 бит
	else if (strcmp(cpu_vendor, "AuthenticAMD") == 0)
		return res & 0x04;
	//Иначе не поддерживается
	else 
		return FALSE;
} 

