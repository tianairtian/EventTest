// EventTest.cpp : 定义控制台应用程序的入口点。
//

/************************************************************************/
/* 事件(Event)是WIN32提供的最灵活的线程间同步方式，事件可以处于激发状态(signaled or true)或未激发状态(unsignal or false)。
根据状态变迁方式的不同，事件可分为两类：
　　（1）手动设置：这种对象只可能用程序手动设置，在需要该事件或者事件发生时，采用SetEvent及ResetEvent来进行设置。
  　　（2）自动恢复：一旦事件发生并被处理后，自动恢复到没有事件状态，不需要再次设置。
	　　创建事件的函数原型为：
	  HANDLE CreateEvent(
	  　LPSECURITY_ATTRIBUTES lpEventAttributes,
	   　// SECURITY_ATTRIBUTES结构指针，可为NULL
		　BOOL bManualReset, 
		 　// 手动/自动
		  　// TRUE：在WaitForSingleObject后必须手动调用ResetEvent清除信号
		   　// FALSE：在WaitForSingleObject后，系统自动清除事件信号
			　BOOL bInitialState, //初始状态
			 　LPCTSTR lpName //事件的名称
			  );
			  使用"事件"机制应注意以下事项：
			  　　（1）如果跨进程访问事件，必须对事件命名，在对事件命名的时候，要注意不要与系统命名空间中的其它全局命名对象冲突；
				　　（2）事件是否要自动恢复；
				  　　（3）事件的初始状态设置。                                                                     */
/************************************************************************/
#include "stdafx.h"
#include <iostream>
#include <windows.h>
using namespace std;

void locket();//加锁
void unLocket();//解锁

DWORD WINAPI ThreadProc(LPVOID lpParam);
DWORD WINAPI ThreadProc2(LPVOID lpParam);

DWORD g_dwThreadID;
DWORD g_dwThreadID2;

UINT g_nTickets = 100;
HWND g_hEvent = NULL;

HANDLE g_hEvent1 = NULL;
HANDLE g_hEvent2 = NULL;

CRITICAL_SECTION g_cs;//临界区对象
int ThreadCout = 0;

int _tmain(int argc, _TCHAR* argv[])
{
	cout<< "Main Thread is Running." << endl;
	InitializeCriticalSection(&g_cs);//初始化临界区
	HANDLE hHandle = CreateThread(NULL, 0, ThreadProc, NULL, 0, &g_dwThreadID);//创建线程
	ThreadCout++;
	HANDLE hHandle2 = CreateThread(NULL, 0, ThreadProc2, NULL, 0, &g_dwThreadID2);//创建线程2;
	ThreadCout++;
	// 第二参数为true:为【手动设置】 为false:【为自动恢复】
	//（1）手动设置：这种对象只可能用程序手动设置，在需要该事件或者事件发生时，采用SetEvent及ResetEvent来进行设置。
	//（2）自动恢复：一旦事件发生并被处理后，自动恢复到没有事件状态，不需要再次设置。
	g_hEvent1 = CreateEvent(NULL, FALSE, TRUE, NULL);//参数1：句柄标识
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
		WaitForSingleObject(g_hEvent1, INFINITE);//INFINITE 无限超时 0和INFINITE。若为0，则该函数立即返回；若为INFINITE，则线程一直被挂起，
		cout << "No.1 " << g_dwThreadID << "Thread is running." << endl;
		locket();
		int temp = g_nTickets;
		unLocket();//LeaveCriticalSection(&g_cs);//解锁

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
			SetEvent(g_hEvent2);//没有这个ThreadProc2不能终止
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
			SetEvent(g_hEvent1);//同样的问题，没有这个ThreadProc不能终止
			break;
		}
	}
	locket();
	ThreadCout--;
	unLocket();
	cout << "No.2 -end" << endl;

	return 0;
}
//加锁
void locket()
{
	EnterCriticalSection(&g_cs);
}
//解锁
void unLocket()
{
	LeaveCriticalSection(&g_cs);//解锁
}
