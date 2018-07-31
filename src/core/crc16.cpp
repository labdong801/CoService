#include "crc16.h"

inline uint16_t reverse(uint16_t value)
{
    uint16_t result = 0;

    for(size_t bit_index = 0; bit_index < 16; bit_index++)
    {
        const uint16_t value_mask = 1 << (15 - bit_index);

        result |= (value & value_mask) ? 1 << bit_index : 0;
    }

    return result;
}

inline uint16_t crc16_xor_reg(uint16_t reg, uint16_t reg_msb)
{
    return reg_msb ? reg ^ CRC16_GENERATOR : reg;
}

uint16_t crc16_compute(const uint8_t *data, size_t length)
{
    uint16_t reg = 0;

    for(size_t byte_index = 0; byte_index < length; byte_index++)
    {
        const uint8_t byte = data[byte_index];

        for(size_t bit_index = 0; bit_index < 8; bit_index++)
        {
            const uint16_t reg_msb = reg >> 15;
            const uint16_t bit = (byte >> bit_index) & 1;

            reg = (reg << 1) | bit;
            reg = crc16_xor_reg(reg, reg_msb);
        }
    }

    for(size_t i = 0; i < 16; i++)
    {
        const uint16_t reg_msb = reg >> 15;

        reg <<= 1;
        reg = crc16_xor_reg(reg, reg_msb);
    }

    return reverse(reg);
}