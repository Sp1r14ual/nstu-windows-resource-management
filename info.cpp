#define _CRT_SECURE_NO_WARNINGS
#define UNICODE
#include <windows.h>
#include <stdint.h>
#include <string.h>

// ������������� ����������
char cpu_vendor[13];

// ������� ��� ��������� ���������� �������� ��������
extern "C" __declspec(dllexport) char* GetProcessPriority()
{
	//�������� ��������� �������� �������� � ������������� ����
	DWORD process_priority = GetPriorityClass(GetCurrentProcess());

	//������������ ������������� ��������� ���������� � ��������� ���������
	switch(process_priority)
	{
		case ABOVE_NORMAL_PRIORITY_CLASS:
			return "���� ��������";
		case BELOW_NORMAL_PRIORITY_CLASS:
			return "���� ��������";
		case HIGH_PRIORITY_CLASS:
			return "�������";
		case IDLE_PRIORITY_CLASS:
			return "����������";
		case NORMAL_PRIORITY_CLASS:
			return "�������";
		case REALTIME_PRIORITY_CLASS:
			return "������������";	
		default:
			return "������";	
	}
}

//������� ��� ������ CPUID �� ��������� EAX = 1
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

//������� ��� ���������� ������������� ����������
void get_cpu_vendor()
{
	// CPUID �� ��������� EAX = 0
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

	//������� ���������� ������������� ����������
	get_cpu_vendor();

	//CPUID � ���������� EAX = 1
	mycpuid(x, 1);

	//��������� �������� �������� ECX
	res = static_cast<uint32_t>(x[2]);

	//���� Intel, ��������� 5 ���
	if (strcmp(cpu_vendor, "GenuineIntel") == 0)
		return res & 0x20;
	//���� AMD, ��������� 2 ���
	else if (strcmp(cpu_vendor, "AuthenticAMD") == 0)
		return res & 0x04;
	//����� �� ��������������
	else 
		return FALSE;
} 

