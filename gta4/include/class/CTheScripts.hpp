#ifndef CTHESCRIPTS_HPP
#define CTHESCRIPTS_HPP
#include <cstdint>
#include <utils/singleton.hpp>

class CTheScripts : public Singleton<CTheScripts>
{
public:
    CTheScripts();

    void ResetThread()
    {

    }

private:
    uint32_t uCurrentThreadAddress;    
};

#endif /* CTHESCRIPTS_HPP */
