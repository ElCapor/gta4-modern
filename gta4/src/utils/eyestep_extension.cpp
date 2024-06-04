#include "utils/eyestep_extension.hpp"
#include <cstdint>
#include <Windows.h>
#include <eyestep/eyestep.h>
#include <eyestep/eyestep_utility.h>
/// @brief Eyestep utilities that make your life easier
/// @author ElCapor
namespace EyeStep
{
namespace util
{

/// @brief Check if the specified address corresponds to a specific instruction
/// @param address target address
/// @param instruction the instruction we expect
/// @return true if iinstruction else false
bool is_instruction(uint32_t address, uint8_t instruction)
{
    return EyeStep::util::readByte(address) == instruction;
}

/// @brief Compare the value of the address to an array of instructions
/// @param address address to check the instructions at
/// @param instructions the instructions
/// @param align_bytes the number of bytes the address should be aligned to, defaults to one
/// @return true if same else false
bool compare_instructions(uint32_t address, std::vector<uint8_t> instructions, int align_bytes)
{
    if (address % align_bytes !=0)
        return false;
        
    for (int i =0; i <instructions.size(); i++)
    {
        if (!is_instruction(address + i, instructions[i]))
        {
            return false;
        }
        
    }
    return true;
}

/// @brief Step forward in memory by a number of bytes
/// @param address reference to the address we want to step, defaults to 1
/// @param step the number of steps in byte
void GoForward(uint32_t& address, unsigned int step) // unsigned int, if u wanna go back ward use the func dedicated to it
{
    address += abs((int)step);
}

/// @brief Step backward in memory by a number of bytes
/// @param address reference to the address we want to step, defaults to 1
/// @param step the number of steps in byte
void GoBackward(uint32_t& address, unsigned int step)
{
    address -= abs((int)step);
}

/// @brief Get the address of the next specified instruction set. NOTE : this is not an aob scan , but can be used to reach some specific instructions after an aob scan
/// @param instructions a byte array containing the target instructions
/// @param max_distance the max distance from the current address
/// @param address the current address we should start searching at
/// @param align_byes number of bytes the address should be aligned to, defaults to 1
/// @return the address the instruction was found at else 0
uint32_t get_next_instruction_set(uint32_t address,std::vector<uint8_t> instructions, int max_distance, int align_bytes)
{
    int distance = 0; // current distance from the address
    while (!compare_instructions(address, instructions) || distance <= max_distance)
    {
        GoForward(address);
        distance++;
    }
    return address;
}

/// @brief Get the address of the next specified instruction set. NOTE : this is not an aob scan , but can be used to reach some specific instructions after an aob scan
/// @param instructions a byte array containing the target instructions
/// @param max_distance the max distance from the current address
/// @param address the current address we should start searching at
/// @param align_byes number of bytes the address should be aligned to, defaults to 1
/// @return the address the instruction was found at else 0
uint32_t get_previous_instruction_set(uint32_t address,std::vector<uint8_t> instructions, int max_distance, int align_bytes)
{
    int distance = 0; // current distance from the address
    while (!compare_instructions(address, instructions, align_bytes) || distance <= max_distance)
    {
        GoBackward(address);
        distance++;
    }
    return address;
}

/// @brief Get the prologue of a function who's prologue is not push ebp , but sub esp
/// @param start start address
/// @param next search forward in memory if true, else backward if false
/// @param max_distance max distance from start address
/// @param align_btes number of bytes the address should be aligned to, defaults to 1
/// @return address of the found prologue
uint32_t get_sub_esp_prologue(uint32_t start, bool next, int max_distance, int align_bytes)
{
    return next ? get_next_instruction_set(start, {0x81, 0xEC}, max_distance, align_bytes) : get_previous_instruction_set(start, {0x81, 0xEC}, max_distance, align_bytes);
}

}
}