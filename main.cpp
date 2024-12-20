#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

std::ostringstream ossRecursionLog; // Used to store recursion logs
std::vector<double> predefinedExpenses = {1050, 500, 1400, 7000, 8500, 6900, 9000, 1100, 1400, 4700, 3100, 6500, 2100, 8500, 5700, 6000, 2900, 2000, 1500, 7800, 800, 9700, 7300, 950, 3800, 3500, 300, 7700, 3000, 1800, 4400, 5900, 6400, 3200, 2700, 1700, 1900, 2000, 2300, 1600, 9900, 4800, 5000, 3600, 1200, 5900, 5500, 2400, 4000, 9100, 1000, 1300, 1900, 6300, 4500, 10000, 7800, 6700, 8100, 3900, 5700, 1500, 8600, 2000, 2500, 9200, 9400, 9600, 7400, 3200, 1100, 3400, 1900, 700, 6400, 9300, 2100, 6100, 4500, 6700, 800, 1500, 1200, 9100, 4500, 8500, 1700, 8200, 8500, 4000, 1100, 950, 7200, 3000};

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
