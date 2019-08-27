#ifndef MSEQUENCE_HPP_
#define MSEQUENCE_HPP_

#include <stdint.h>

// elements per frame = FRAME_SIZE * 32
#define FRAME_SIZE 8
#define FRAME_SHIFT 4
#define BITS_PER_ELEMENT 32
#define FRAME_MAX 32767 //2^15-1

class MSequence
{
public:
    MSequence(const char* filepath);
    ~MSequence();
    int32_t* get_frame(int32_t nframe);
    inline bool get_bit(int64_t n)
    {
        int32_t nwrap = static_cast<int32_t>(n % FRAME_MAX);
        int32_t kel = nwrap / BITS_PER_ELEMENT;
        int8_t shift = nwrap - (kel * BITS_PER_ELEMENT);
        return static_cast<bool>((_data[kel] >> shift) & 0x01);
    }
    inline bool isvalid() { return _state; }
    inline size_t length() { return _length * BITS_PER_ELEMENT; }
private:
    int32_t* _data = nullptr;
    int64_t _length;
    int32_t _frame[FRAME_SIZE];
    bool _state;
};

#endif
