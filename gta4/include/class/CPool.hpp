#ifndef CPOOL_HPP
#define CPOOL_HPP
#include <cstdint>
template <typename T>
class CPool
{
public:
uint8_t* m_pObjects; // array of objects
uint8_t* m_pFlags; // each flag per object
uint32_t m_nCount;					// 08-0C
uint32_t m_nEntrySize;				// 0C-10
int32_t m_nTop;						// 10-14
uint32_t m_nUsed;					// 14-18
uint8_t m_bAllocated;				// 18-19
uint8_t pad[3];	


uint32_t indexOf(T* obj)
{
    return ((std::uint32_t)obj - (std::uint32_t)this->m_pObjects) / this->m_nEntrySize;
}

bool isObjectInPool(T* obj)
{
    bool res = false; // result
    if ((std::uint32_t)obj >= this->m_pObjects)
    {
        if ( obj <= (unsigned int)(m_pObjects + m_nEntrySize * (this->m_nSize - 1))
        && !((obj - m_pObjects) % m_nEntrySize)
        && *(char *)((obj - m_pObjects) / m_nEntrySize + this->m_pFlags) >= 0 )
        {
            return 1;
        }
    }
    return res;
}

bool IsValid(int slot)
{
    return !(m_pFlags[slot] & 0x80);
}

T* Get(int slot)
{
    if (!IsValid(slot)) return nullptr;
    return (T*)&m_pObjects[m_nEntrySize * slot];
}

int FindNextUsed(int curr)
{
    for (int i = curr; i < m_nCount; i++)
    {
        if (IsValid(i)) return i;
    }
    return -1;
}

// pool iterator
struct Iterator
{
    Iterator(CPool<T>* ptr, int id)
    {
        pool = ptr;
        index = id;
    }

    T* operator*() const { return pool->Get(index); }
    Iterator& operator++()
    {
        index = pool->FindNextUsed(index + 1);
        return *this;
    }
    bool operator!= (const Iterator& b) { return index != b.index; };

private:
    CPool<T>* pool;
    int index = 0;
};

};

template<typename T>
auto begin(CPool<T>* pool) { return CPool<T>::Iterator(pool, pool->FindNextUsed(0)); }
template<typename T>
auto end(CPool<T>* pool) { return CPool<T>::Iterator(pool, -1); }

#endif /* CPOOL_HPP */
