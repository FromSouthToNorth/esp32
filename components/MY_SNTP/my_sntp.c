#include "esp_attr.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "lwip/ip_addr.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

static const char *TAG = "SNTP";
static struct tm timeinfo;

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

struct tm *get_time(void) {
  return &timeinfo;
}

static void esp_initialize_sntp(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  /* This helper function configures Wi-Fi or Ethernet, as selected in
   * menuconfig. Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
  ESP_ERROR_CHECK(example_connect());
  ESP_LOGI(TAG, "Initializing and starting SNTP");
  ESP_LOGI(TAG, "This is the basic default config with one server and starting "
                "he service");
  /*
   * This is the basic default config with one server and starting the service
   */
  esp_sntp_config_t config =
      ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
  esp_netif_sntp_init(&config);

  // wait for time to be set
  time_t now = 0;
  int retry = 0;
  const int retry_count = 15;
  while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) ==
             ESP_ERR_TIMEOUT &&
         ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry,
             retry_count);
  }

  setenv("TZ", "CST-8", 1); // 设置为中国的时区
  time(&now);
  localtime_r(&now, &timeinfo);

  ESP_ERROR_CHECK(example_disconnect());
  esp_netif_sntp_deinit(); //初始化
}

void sntp_task(void *param) {
  time_t now;
  esp_initialize_sntp();

  // 延时等待SNTP初始化完成
  do {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  } while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET);

  esp_sntp_stop();

  while (1) {
    time(&now);                   // 获取网络时间, 64bit的秒计数
    tzset();                      // 更新本地C库时间
    localtime_r(&now, &timeinfo); // 转换成具体的时间参数
    ESP_LOGI(TAG, "%4d-%02d-%02d %02d:%02d:%02d week:%d",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
             timeinfo.tm_wday);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
