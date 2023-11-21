#include "framework.h"
#include "WindowsProject.h"
#include <Windows.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND hTextBox; // Global handle for the text box
HWND hInputBox; // Global handle for the input box

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

DWORD pid = GetCurrentProcessId();
char testStr[] = "Hello";

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR lpCmdLine,
  _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT));

  MSG msg;

  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  hInst = hInstance;

  HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, 400, 200, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd)
  {
    return FALSE;
  }

  hTextBox = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY,
    10, 50, 360, 100, hWnd, nullptr, hInstance, nullptr);

  SendMessage(hTextBox, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

  hInputBox = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_NUMBER,
    10, 10, 50, 30, hWnd, nullptr, hInstance, nullptr);

  SendMessage(hInputBox, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

  HWND hButton = CreateWindowW(L"BUTTON", L"Display", WS_CHILD | WS_VISIBLE,
    70, 10, 80, 30, hWnd, (HMENU)1001, hInstance, nullptr);

  SendMessage(hButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  //static wchar_t* testStr = nullptr; // Global pointer for testStr

  switch (message)
  {
  case WM_CREATE:
    //testStr = new wchar_t[256]; // Allocate memory for testStr
    break;
  case WM_COMMAND:
  {
    int wmId = LOWORD(wParam);
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    case 1001:
    {
      wchar_t inputText[256];
      GetWindowText(hInputBox, inputText, 256);

      int pid = _wtoi(inputText);

      // Create a new testStr based on the entered process ID
      //swprintf_s(testStr, 256, L"New String for PID %d", pid);

      wchar_t displayText[256];
      swprintf_s(displayText, L"%s\n%lu\nResult: %s", testStr, pid, testStr);

      SetWindowText(hTextBox, displayText);
    }
    break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  }
  break;
  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    DWORD pid = GetCurrentProcessId();

    wchar_t pidStr[20];
    swprintf_s(pidStr, L"%lu", pid);

    wchar_t displayText[256];
    swprintf_s(displayText, L"%s\n%s", testStr, pidStr);

    SetWindowText(hTextBox, displayText);

    EndPaint(hWnd, &ps);
  }
  break;
  case WM_DESTROY:
    delete[] testStr; // Free the allocated memory
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}



INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
