#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "xtensa/core-macros.h"
#include "cct.h"

static uint32_t cctMarker;

// only works if task is locked to one cpu

void cct_DelayUs(uint32_t us) {
  volatile uint32_t waitCycleCount = (CCT_TICKS_PER_US * us ) + XTHAL_GET_CCOUNT();
  do  {} while (XTHAL_GET_CCOUNT()  < waitCycleCount);
  }


uint32_t  cct_IntervalUs(uint32_t before, uint32_t after) {
  return  (before <= after ?
    ((after - before)+CCT_TICKS_PER_US/2)/CCT_TICKS_PER_US :
    (after + (0xFFFFFFFF - before) + CCT_TICKS_PER_US/2)/CCT_TICKS_PER_US);
}

float  cct_IntervalSecs(uint32_t before, uint32_t after) {
  return  (before <= after ?
    (float)(after - before)/(float)SYSTEM_CLOCK_HZ :
    (float)(after + (0xFFFFFFFF - before))/(float)SYSTEM_CLOCK_HZ);
}

void cct_SetMarker(void) {
  cctMarker = XTHAL_GET_CCOUNT();
}

uint32_t cct_ElapsedTimeUs(void) {
  uint32_t now = XTHAL_GET_CCOUNT();
  return  (cctMarker <= now ?
    ((now - cctMarker)+CCT_TICKS_PER_US/2)/CCT_TICKS_PER_US :
    (now + (0xFFFFFFFF - cctMarker) + CCT_TICKS_PER_US/2)/CCT_TICKS_PER_US);
}
