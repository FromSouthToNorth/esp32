#ifndef __NTP_TIME_H_
#define __NTP_TIME_H_

#include "esp_sntp.h"
#include <sys/time.h>
#include <time.h>

void ntp_time_init(void);
void sntp_task(void *param);
struct tm *get_time(void);

#endif
