#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "driver/timer.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "tcpip_adapter.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "hal/esp32-hal.h"  // has good stuff like NOP etc.

#define delayMs(ms)  vTaskDelay((ms) / portTICK_PERIOD_MS);

#define ABS(x) ((x) < 0 ? -(x) : (x))

#endif
