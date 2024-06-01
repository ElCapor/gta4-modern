#ifndef RAGE_HPP
#define RAGE_HPP
#include <cstdint>

namespace rage
{
    namespace utils
    {
        std::uint32_t HashString(char* string, uint32_t seed);
    }
}

#endif /* RAGE_HPP */
