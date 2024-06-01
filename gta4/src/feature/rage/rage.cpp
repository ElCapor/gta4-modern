#include <feature/rage/rage.hpp>

std::uint32_t rage::utils::HashString(char* string, uint32_t seed)
{
    char *v3; // esi
    char i; // al

    v3 = string;
    if ( *string == 34 )
        v3 = string + 1;
    for ( i = *v3; *v3; i = *v3 )
    {
        if ( *string == 34 && i == 34 )
        break;
        ++v3;
        if ( (uint8_t)(i - 65) > 0x19u )
        {
        if ( i == 92 )
            i = 47;
        }
        else
        {
        i += 32;
        }
        seed = ((1025 * (seed + i)) >> 6) ^ (1025 * (seed + i));
    }
    return 32769 * ((9 * seed) ^ ((9 * seed) >> 11));
}   