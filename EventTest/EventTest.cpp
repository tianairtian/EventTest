// EventTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

/************************************************************************/
/* �¼�(Event)��WIN32�ṩ���������̼߳�ͬ����ʽ���¼����Դ��ڼ���״̬(signaled or true)��δ����״̬(unsignal or false)��
����״̬��Ǩ��ʽ�Ĳ�ͬ���¼��ɷ�Ϊ���ࣺ
������1���ֶ����ã����ֶ���ֻ�����ó����ֶ����ã�����Ҫ���¼������¼�����ʱ������SetEvent��ResetEvent���������á�
  ������2���Զ��ָ���һ���¼���������������Զ��ָ���û���¼�״̬������Ҫ�ٴ����á�
	���������¼��ĺ���ԭ��Ϊ��
	  HANDLE CreateEvent(
	  ��LPSECURITY_ATTRIBUTES lpEventAttributes,
	   ��// SECURITY_ATTRIBUTES�ṹָ�룬��ΪNULL
		��BOOL bManualReset, 
		 ��// �ֶ�/�Զ�
		  ��// TRUE����WaitForSingleObject������ֶ�����ResetEvent����ź�
		   ��// FALSE����WaitForSingleObject��ϵͳ�Զ�����¼��ź�
			��BOOL bInitialState, //��ʼ״̬
			 ��LPCTSTR lpName //�¼�������
			  );
			  ʹ��"�¼�"����Ӧע���������
			  ������1���������̷����¼���������¼��������ڶ��¼�������ʱ��Ҫע�ⲻҪ��ϵͳ�����ռ��е�����ȫ�����������ͻ��
				������2���¼��Ƿ�Ҫ�Զ��ָ���
				  ������3���¼��ĳ�ʼ״̬���á�                                                                     */
/************************************************************************/
#include "stdafx.h"
#include <iostream>
#include <windows.h>
using namespace std;

void locket();//����
void unLocket();//����

DWORD WINAPI ThreadProc(LPVOID lpParam);
DWORD WINAPI ThreadProc2(LPVOID lpParam);

DWORD g_dwThreadID;
DWORD g_dwThreadID2;

UINT g_nTickets = 100;
HWND g_hEvent = NULL;

HANDLE g_hEvent1 = NULL;
HANDLE g_hEvent2 = NULL;

CRITICAL_SECTION g_cs;//�ٽ�������
int ThreadCout = 0;

int _tmain(int argc, _TCHAR* argv[])
{
	cout<< "Main Thread is Running." << endl;
	InitializeCriticalSection(&g_cs);//��ʼ���ٽ���
	HANDLE hHandle = CreateThread(NULL, 0, ThreadProc, NULL, 0, &g_dwThreadID);//�����߳�
	ThreadCout++;
	HANDLE hHandle2 = CreateThread(NULL, 0, ThreadProc2, NULL, 0, &g_dwThreadID2);//�����߳�2;
	ThreadCout++;
	// �ڶ�����Ϊtrue:Ϊ���ֶ����á� Ϊfalse:��Ϊ�Զ��ָ���
	//��1���ֶ����ã����ֶ���ֻ�����ó����ֶ����ã�����Ҫ���¼������¼�����ʱ������SetEvent��ResetEvent���������á�
	//��2���Զ��ָ���һ���¼���������������Զ��ָ���û���¼�״̬������Ҫ�ٴ����á�
	g_hEvent1 = CreateEvent(NULL, FALSE, TRUE, NULL);//����1�������ʶ
	g_hEvent2 = CreateEvent(NULL, FALSE, TRUE, NULL);

//	ResetEvent(g_hEvent1);
//	ResetEvent(g_hEvent2);
	SetEvent(g_hEvent1);

	while (true)
	{
		locket();
		int nCount = ThreadCout;
		unLocket();
		if (nCount == 0)
		{
			cout << "Main thread is break.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << endl << endl << endl;
			break;
		}
	}
	Sleep(1000);
	CloseHandle(hHandle);
	CloseHandle(hHandle2);

	DeleteCriticalSection(&g_cs);
	cout << "Main Thread is end."<< endl;
	system("pause");
	return 0;
}
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	while (true)
	{
		WaitForSingleObject(g_hEvent1, INFINITE);//INFINITE ���޳�ʱ 0��INFINITE����Ϊ0����ú����������أ���ΪINFINITE�����߳�һֱ������
		cout << "No.1 " << g_dwThreadID << "Thread is running." << endl;
		locket();
		int temp = g_nTickets;
		unLocket();//LeaveCriticalSection(&g_cs);//����

		cout << "No.1 " << g_dwThreadID << "Thread is temp." << endl;

		if(temp > 0)
		{
			Sleep(100);
			cout << "No.1-" << g_dwThreadID << "sell ticket:" << temp << endl;

			locket();
			g_nTickets--;
			unLocket();
			SetEvent(g_hEvent2);
		}
		else
		{
			cout << "No.1-break00000000000000000000" << endl;
			SetEvent(g_hEvent2);//û�����ThreadProc2������ֹ
			break;
		}
	}
	locket();
	ThreadCout--;
	unLocket();
	cout << "No.1- end" << endl;

	return 0;
}
DWORD WINAPI ThreadProc2(LPVOID lpParam)
{
	while (true)
	{
		WaitForSingleObject(g_hEvent2, INFINITE);
		cout << "No.2 " <<  g_dwThreadID2 <<  "Thread is Running." << endl;

		locket();
		int temp = g_nTickets;
		unLocket();

		if(temp > 0)
		{
			Sleep(100);
			cout << "No.2-" << g_dwThreadID2 << " Sell ticket:" << temp << endl;
			locket();
			g_nTickets--;
			unLocket();

			SetEvent(g_hEvent1);
		}
		else
		{
			cout << "No.2- break" << endl;
			SetEvent(g_hEvent1);//ͬ�������⣬û�����ThreadProc������ֹ
			break;
		}
	}
	locket();
	ThreadCout--;
	unLocket();
	cout << "No.2 -end" << endl;

	return 0;
}
//����
void locket()
{
	EnterCriticalSection(&g_cs);
}
//����
void unLocket()
{
	LeaveCriticalSection(&g_cs);//����
}
