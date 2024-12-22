#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

std::ostringstream ossRecursionLog; // Used to store recursion logs
std::vector<double> predefinedExpenses = {}; // isi banyak data cth 10 data = {800, 1050, 500, 1200, 700, 600, 900, 1000, 1100, 950}

double calculateExpenseRec(double expenses[], int n) {
    if (n == 1) {
        ossRecursionLog << "Base case: return " << expenses[0] << "\n";
        return expenses[0];
    }
    double result = expenses[n - 1] + calculateExpenseRec(expenses, n - 1);
    ossRecursionLog << "calculateExpenseRec(expenses, " << n << ") = " << result << "\n";
    return result;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "ExpenseCalculatorRec";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Expense Calculator (Recursive)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::vector<HWND> hwndEdits;
    static HWND hwndCalculateButton;

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CREATE:
        for (size_t i = 0; i < predefinedExpenses.size(); ++i) {
            // Display each month in the format "Month 1", "Month 2", ...
            CreateWindow(
                "STATIC", ("Month " + std::to_string(i + 1)).c_str(),
                WS_VISIBLE | WS_CHILD,
                10, 10 + i * 40, 100, 20,
                hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            HWND hwndEdit = CreateWindow(
                "EDIT", std::to_string(predefinedExpenses[i]).c_str(),
                WS_BORDER | WS_CHILD | WS_VISIBLE,
                120, 10 + i * 40, 100, 20,
                hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            hwndEdits.push_back(hwndEdit);
        }

        hwndCalculateButton = CreateWindow(
            "BUTTON", "Calculate",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            230, 10, 100, 30,
            hwnd, (HMENU)1, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            std::vector<double> expenses;
            for (HWND hwndEdit : hwndEdits) {
                char buffer[256];
                GetWindowText(hwndEdit, buffer, 256);
                expenses.push_back(atof(buffer));
            }

            // Start measuring time
            LARGE_INTEGER frequency, start, end;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&start);

            ossRecursionLog.str("");  // Clear previous recursion logs
            ossRecursionLog.clear();

            // Pass the expenses array to the recursion function
            double total = calculateExpenseRec(expenses.data(), expenses.size());

            // Stop measuring time
            QueryPerformanceCounter(&end);
            double elapsedTime = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;

            // Format the output to display time in normal decimal format (not scientific)
            std::ostringstream oss;
            oss << "Total expenses: " << total << "\n";
            oss << "Elapsed time: " << std::fixed << std::setprecision(6) << elapsedTime << " seconds\n";
            oss << "Recursion Log:\n" << ossRecursionLog.str();  // Include the recursion log

            MessageBox(hwnd, oss.str().c_str(), "Result", MB_OK);
        }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
