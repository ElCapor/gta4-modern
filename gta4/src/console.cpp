#include <console.hpp>

FILE* Console::fp = NULL;

void Console::Open()
{
    AllocConsole();
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
}

void Console::Close()
{
    fclose(fp);
    HWND hwnd = GetConsoleWindow();
    if (hwnd != nullptr) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    FreeConsole();
}

void Console::Wait()
{
    std::cin.ignore();
}
