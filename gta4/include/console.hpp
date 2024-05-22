#ifndef CONSOLE_HPP
#define CONSOLE_HPP
// debug utils 
#include <Windows.h>
#include <iostream>
// debug console class with colors
class Console
{
protected:
    template <typename T>
    static void log_argument(T t) {
        std::cout << t << " ";
    }
    static FILE* fp;

public:
    static void Open();
    static void Close();
    static void Wait();

     template <typename... Args>
    static void debug(Args&&... args) {
        std::cout << "[DEBUG] ";
        (log_argument(std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }

    template <typename... Args>
    static void log(Args&&... args) {
        std::cout << "[LOG] ";
        (log_argument(std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }

    template <typename... Args>
    static void warn(Args&&... args) {
        std::cout << "[WARNING] ";
        (log_argument(std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }

    template <typename... Args>
    static void error(Args&&... args) {
        std::cout << "[ERROR] ";
        (log_argument(std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }
};


#endif /* CONSOLE_HPP */
