
#include <cstdio>
#include <new>

#include "msequence.hpp"

MSequence::MSequence(const char* filepath) : _state(false)
{
    FILE* fp = fopen(filepath, "rb");

    if (fp != NULL)
    {
        fread(&_length, sizeof(int64_t), 1, fp);

        printf("_length = %ld\n", _length);

        _data = new int32_t[_length];

        fread(_data, sizeof(int32_t), _length, fp);

        fclose(fp);
        _state = true;
    }
}

MSequence::~MSequence()
{
    if (_data != nullptr)
    {
        printf("Deleting _data\n");
        delete[] _data;
    }
    _length = 0;
}

int32_t* MSequence::get_frame(int32_t nframe)
{
    int32_t idx = nframe % FRAME_MAX;
    int64_t inc = 0;

    for (int k = 0; k < FRAME_SIZE; ++k)
    {
        uint32_t buf = 0;
        for (int j = 0; j < BITS_PER_ELEMENT; ++j)
        {
            if (get_bit(idx + inc))
            {
                // set the j'th bit high
                buf |= (0x00000001 << j);
            }

            // the value of the k'th pixel on the n'th frame is the value of the
            // first pixel on frame n + 128 * k
            inc += 128;
        }
        // we need signed int for GL, but we need to perserve the bit order
        // _frame[k] = reinterpret_cast<int32_t>(buf);
        _frame[k] = (int32_t)buf;
    }
    return _frame;
}
