#ifndef I2S_PARALLEL_H
#define I2S_PARALLEL_H

#include <stdint.h>
#include "soc/i2s_struct.h"
#include "config.h"

typedef enum {
    I2S_PARALLEL_BITS_8 	= 8,
    I2S_PARALLEL_BITS_16	= 16,
    I2S_PARALLEL_BITS_32	= 32,
} i2s_parallel_cfg_bits_t;

typedef struct {
    void* memory;
    size_t size;
} i2s_parallel_buffer_desc_t;

typedef struct {
    int gpio_bus[24];
    int gpio_clk;
    int clkspeed_hz;
    i2s_parallel_cfg_bits_t bits;
#ifdef DOUBLE_BUFFERED
    i2s_parallel_buffer_desc_t* bufa;
    i2s_parallel_buffer_desc_t* bufb;
#else
    i2s_parallel_buffer_desc_t* buf;
#endif
} i2s_parallel_config_t;

void i2s_parallel_setup(i2s_dev_t *dev, const i2s_parallel_config_t *cfg);
#ifdef DOUBLE_BUFFERED
void i2s_parallel_flip_to_buffer(i2s_dev_t *dev, int bufid);
#endif

#endif
