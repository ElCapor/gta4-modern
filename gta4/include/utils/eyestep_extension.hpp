#ifndef EYESTEP_EXTENSION_HPP
#define EYESTEP_EXTENSION_HPP
#include <cstdint>
#include <vector>
namespace EyeStep
{
namespace util
{
    bool is_instruction(uint32_t address, uint8_t instruction);
    bool compare_instructions(uint32_t address, std::vector<uint8_t> instructions, int align_bytes = 1);
    void GoForward(uint32_t &address, unsigned int step = 1);
    void GoBackward(uint32_t &address, unsigned int step = 1);
    uint32_t get_next_instruction_set(uint32_t address, std::vector<uint8_t> instructions, int max_distance = 50, int align_bytes = 1);
    uint32_t get_previous_instruction_set(uint32_t address, std::vector<uint8_t> instructions, int max_distance = 50, int align_bytes = 1);
    uint32_t get_sub_esp_prologue(uint32_t start, bool next = true, int max_distance = 50, int align_bytes = 1);
}
}

#endif /* EYESTEP_EXTENSION_HPP */
