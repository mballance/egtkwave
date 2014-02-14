/*
 * egtkwave_win32_jni.c
 *
 *  Created on: Dec 8, 2013
 *      Author: ballance
 */

#include <jni.h>
#include <windows.h>
#include <stdio.h>

static LRESULT CALLBACK (*parent_win_proc)(HWND, UINT, WPARAM, LPARAM) = 0;
static HWND child_hwnd = 0;

// typedef struct

static LRESULT CALLBACK win_proc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
{
//	fprintf(stdout, "win_proc: %d\n", msg);
//	fflush(stdout);


	if (child_hwnd &&
			msg == WM_KEYDOWN ||
			msg == WM_KEYUP ||
			msg == WM_CHAR ||
			msg == WM_KILLFOCUS ||
			msg == WM_SETFOCUS ||
			msg == 641 || msg == 642) {
		fprintf(stdout, "[CHILD] win_proc: %p %d\n", parent_win_proc, msg);
		fflush(stdout);
		SendMessage(child_hwnd, msg, wParam, lParam);
		return DefWindowProc(hwnd, msg, wParam, lParam);
//		return FALSE;
	} else {
		fprintf(stdout, "[PARENT] win_proc: %p %d\n", parent_win_proc, msg);
		fflush(stdout);
		return CallWindowProc(parent_win_proc, hwnd, msg, wParam, lParam);
	}

//	return parent_win_proc(hwnd, msg, wParam, lParam);
//	return DefWindowProc(hwnd, msg, wParam, lParam);

#ifdef UNDEFINED
		if (parent_win_proc) {
			//		fprintf(stdout, "parent_win_proc=%p\n", parent_win_proc);
			//		return parent_win_proc(hwnd, msg, wParam, lParam);
			if (child_hwnd) {
				//			fprintf(stdout, "call child\n");
				SendMessage(child_hwnd, msg, wParam, lParam);
				//			DefWindowProc(child_hwnd, msg, wParam, lParam);
			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
		} else {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
#endif

}

typedef struct {
	DWORD		dwProcessId;
	HWND		hWnd;
} cb_data_t;

BOOL CALLBACK enum_win_cb(HWND hwnd, LPARAM lparam) {
	DWORD proc;
	cb_data_t *cbd = (cb_data_t *)lparam;

	GetWindowThreadProcessId(hwnd, &proc);

//	fprintf(stdout, "dwProcessId=%d proc=%d\n", cbd->dwProcessId, proc);

	if (cbd->dwProcessId == proc) {
		char buf[256];
		GetWindowText(hwnd, buf, 256);

//		fprintf(stdout, "buf=%s\n", buf);

//		if (strstr(buf, "GTKWave")) {
		if (strstr(buf, "GTKWave")) {
			cbd->hWnd = hwnd;
			return FALSE;
		}
	}

	return TRUE;
}

jlong __declspec(dllexport) Java_net_sf_egtkwave_ui_jni_GtkWaveJni_launch(
		JNIEnv *, jclass, jlong, jstring);

void __declspec(dllexport) Java_net_sf_egtkwave_ui_jni_GtkWaveJni_terminate(
		JNIEnv *, jclass, jlong);

jlong Java_net_sf_egtkwave_ui_jni_GtkWaveJni_launch(
		JNIEnv			*env,
		jclass			cls,
		jlong			parent_hwnd_l,
		jstring			gtkwave_cmd)
{
	jlong ret = 0;
	const char *gtkwave_cmd_s = (*env)->GetStringUTFChars(env, gtkwave_cmd, 0);
	HWND parent_hwnd = (HWND)parent_hwnd_l;
//	char *cmd = (char *)malloc(strlen(gtkwave_path_s) + 16);
	char *cmd = (char *)malloc(strlen(gtkwave_cmd_s) + 1024);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL cp_ret;
	cb_data_t cbd;

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));

	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	si.dwFlags |= STARTF_USESTDHANDLES;
	/*
	 */

	/*
	cp_ret = CreateProcess(NULL, (char *)gtkwave_cmd_s,
			NULL,
			NULL,
			FALSE,
			DETACHED_PROCESS,
			NULL,
			NULL,
			&si,
			&pi);
	 */

	// Wait for the parent window to be visible
	while (!IsWindowVisible(parent_hwnd)) {
		Sleep(1);
	}

	cp_ret = CreateProcess(NULL, (char *)gtkwave_cmd_s,
			NULL,
			NULL,
			TRUE,
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&si,
			&pi);

	fprintf(stdout, "cp_ret=%d\n", cp_ret);
	fflush(stdout);


	WaitForInputIdle(pi.hProcess, 0);

	cbd.hWnd = 0;
	cbd.dwProcessId = pi.dwProcessId;

	do {
		Sleep(1);
		EnumWindows(&enum_win_cb, (LPARAM)&cbd);
	} while (cbd.hWnd == 0);

	// Hooking window proc passes keyboard events through, but messes
	// with resizing
	parent_win_proc = GetWindowLongPtr(parent_hwnd, GWLP_WNDPROC);
	/* parent_win_proc = */SetWindowLongPtr(parent_hwnd, GWLP_WNDPROC, (LONG_PTR)&win_proc);
	child_hwnd = cbd.hWnd;
#ifdef ENABLED
#endif


	SetParent(cbd.hWnd, parent_hwnd);

	{
		DWORD style = GetWindowLong(cbd.hWnd, GWL_STYLE);
		style |= WS_CHILD;
		SetWindowLong(cbd.hWnd, GWL_STYLE, style);
		SetWindowPos(cbd.hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

//		int pid = GetWindowThreadProcessId()
//		AttachThreadInput(pi.dwThreadId, GetCurrentThreadId(), TRUE);
		AttachThreadInput(pi.dwThreadId, GetCurrentProcessId(), TRUE);
//		AttachThreadInput(GetWindowThreadProcessId(parent_hwnd, NULL),
//				GetWindowThreadProcessId(cbd.hWnd, NULL), TRUE);
//2		AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(cbd.hWnd, NULL), TRUE);
	}

	/**
	// This is important for window to show
	{
		// New code
		SendMessage(parent_hwnd, WM_UPDATEUISTATE, UIS_INITIALIZE, 0);
		UpdateWindow(cbd.hWnd);

		SetForegroundWindow(parent_hwnd);
//		SetForegroundWindow(cbd.hWnd);
	}
	 */

	// New code 2
//	SetFocus(cbd.hWnd);

	{
		RECT r;
		GetClientRect(parent_hwnd, &r);
		MoveWindow(cbd.hWnd, 0, 0,
				abs(r.left-r.right),
				abs(r.top-r.bottom), TRUE);
	}

	{
		// New code
		SendMessage(parent_hwnd, WM_UPDATEUISTATE, UIS_INITIALIZE, 0);
		UpdateWindow(cbd.hWnd);

		SetForegroundWindow(parent_hwnd);
//		SetForegroundWindow(cbd.hWnd);
	}

	ret = (jlong)pi.dwProcessId;

//	RedrawWindow(cbd.hWnd, 0, 0, RDW_VALIDATE);
//	InvalidateRect(cbd.hWnd, 0, TRUE);

	fprintf(stdout, "launch: %s\n", gtkwave_cmd_s);

	(*env)->ReleaseStringUTFChars(env, gtkwave_cmd, gtkwave_cmd_s);

	return ret;
}


void Java_net_sf_egtkwave_ui_jni_GtkWaveJni_terminate(
		JNIEnv			*env,
		jclass			cls,
		jlong			process_id)
{
	HANDLE phndl = OpenProcess(PROCESS_TERMINATE, FALSE, process_id);

	TerminateProcess(phndl, 0);
}
