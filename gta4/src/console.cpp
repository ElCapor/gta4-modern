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
    FreeConsole();
}

void Console::Wait()
{
    std::cin.ignore();
}
