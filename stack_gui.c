#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STACK 10

int stack[MAX_STACK];
int top = -1;

HWND hInputBox, hPushBtn, hPopBtn, hPeekBtn, hResetBtn;
HWND hStackBox[MAX_STACK];

HBRUSH hNormalBrush;
HBRUSH hTopBrush;
HBRUSH hWindowBrush;
int isTopBox[MAX_STACK] = {0};

int isFull() { return top == MAX_STACK - 1; }
int isEmpty() { return top == -1; }

void push(int value) {
    if (!isFull()) stack[++top] = value;
}

int pop() {
    if (isEmpty()) return -999999;
    return stack[top--];
}

int peek() {
    if (isEmpty()) return -999999;
    return stack[top];
}

void RefreshStack(HWND hwnd) {
    char text[20];

    for (int i = 0; i < MAX_STACK; i++) {
        if (i <= top) {
            sprintf(text, "%d", stack[top - i]);
            SetWindowText(hStackBox[i], text);
            isTopBox[i] = (i == 0) ? 1 : 0;
        } else {
            SetWindowText(hStackBox[i], "");
            isTopBox[i] = 0;
        }
    }

    InvalidateRect(hwnd, NULL, TRUE);
}


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_COMMAND:

        if ((HWND)lParam == hPushBtn) {
            char buf[20];
            GetWindowText(hInputBox, buf, 20);
            if (strlen(buf) == 0) return MessageBox(hwnd, "Enter a number!", "Error", MB_OK), 0;
            if (isFull()) return MessageBox(hwnd, "Stack Overflow!", "Error", MB_OK), 0;
            push(atoi(buf));
            RefreshStack(hwnd);
        }

        if ((HWND)lParam == hPopBtn) {
            if (isEmpty()) return MessageBox(hwnd, "Stack Underflow!", "Error", MB_OK), 0;
            int value = pop();
            char msgTxt[50];
            sprintf(msgTxt, "Popped: %d", value);
            MessageBox(hwnd, msgTxt, "POP", MB_OK);
            RefreshStack(hwnd);
        }

        if ((HWND)lParam == hPeekBtn) {
            if (isEmpty()) return MessageBox(hwnd, "Stack Empty!", "Error", MB_OK), 0;
            int value = peek();
            char msgTxt[50];
            sprintf(msgTxt, "Top: %d", value);
            MessageBox(hwnd, msgTxt, "PEEK", MB_OK);
        }

        if ((HWND)lParam == hResetBtn) {
            top = -1;
            RefreshStack(hwnd);
        }

        break;

    case WM_CREATE: {

        hWindowBrush = CreateSolidBrush(RGB(245, 247, 250));
        hNormalBrush = CreateSolidBrush(RGB(235, 235, 245));
        hTopBrush = CreateSolidBrush(RGB(255, 250, 200));

        hInputBox = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
            30, 20, 120, 25, hwnd, NULL, NULL, NULL);

        hPushBtn = CreateWindow("BUTTON", "PUSH", WS_CHILD | WS_VISIBLE,
            170, 20, 80, 25, hwnd, NULL, NULL, NULL);

        hPopBtn = CreateWindow("BUTTON", "POP", WS_CHILD | WS_VISIBLE,
            260, 20, 80, 25, hwnd, NULL, NULL, NULL);

        hPeekBtn = CreateWindow("BUTTON", "PEEK", WS_CHILD | WS_VISIBLE,
            350, 20, 80, 25, hwnd, NULL, NULL, NULL);

        hResetBtn = CreateWindow("BUTTON", "RESET", WS_CHILD | WS_VISIBLE,
            440, 20, 80, 25, hwnd, NULL, NULL, NULL);

        for (int i = 0; i < MAX_STACK; i++) {
            hStackBox[i] = CreateWindow(
                "STATIC", "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
                200, 70 + i * 35, 150, 30,
                hwnd, NULL, NULL, NULL
            );
            // no subclassing; coloring handled in parent WM_CTLCOLORSTATIC
        }
    }
    break;

    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);
        if (hWindowBrush) FillRect(hdc, &rc, hWindowBrush);
        return 1;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        HWND child = (HWND)lParam;

        // default text color and opaque background so brush is visible
        SetTextColor(hdc, RGB(32, 32, 32));
        SetBkMode(hdc, OPAQUE);

        for (int i = 0; i < MAX_STACK; i++) {
            if (child == hStackBox[i]) {
                // If this is the top box, use top brush and darker text
                if (isTopBox[i]) {
                    SetBkColor(hdc, RGB(255, 250, 200));
                    SetTextColor(hdc, RGB(40, 40, 40));
                    return (LRESULT)hTopBrush;
                } else {
                    SetBkColor(hdc, RGB(235, 235, 245));
                    SetTextColor(hdc, RGB(30, 30, 30));
                    return (LRESULT)hNormalBrush;
                }
            }
        }

        // fall back for other static controls
        return (LRESULT)hWindowBrush;
    }

    case WM_CTLCOLORDLG:
        return (LRESULT)hWindowBrush;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nCmdShow) {

    WNDCLASS wc = { 0 };
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = "StackSimWin";
    wc.lpfnWndProc = WindowProcedure;

    RegisterClass(&wc);

    CreateWindow(
        "StackSimWin",
        "Stack Simulator - Enhanced GUI",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        300, 100, 600, 500,
        NULL, NULL, NULL, NULL
    );

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
