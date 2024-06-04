#ifndef MEM_HPP
#define MEM_HPP
#define PATTERN pattern_holder

#include <utils/pattern.hpp>
#include <cstdint>

template<typename T> T& GetRef(uint32_t address)
{
    return *reinterpret_cast<T*>(address);
}

struct pattern_holder
{
public:
    const char* aob;//ida format aob
    void* target; // target address

    pattern_holder(const char* aob = "", void* target = nullptr) : aob(aob), target(target)
    {
        // init the structure
    }

    void* find(ptrdiff_t offset)
    {
        target = hook::pattern(aob).get_first(offset);
        return target;
    }

    void* get(std::uint32_t index, ptrdiff_t offset)
    {
        target = hook::pattern(aob).get(index).get<void>(offset);
        return target;
    }
};

#endif /* MEM_HPP */
