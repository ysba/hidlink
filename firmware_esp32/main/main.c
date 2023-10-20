#include "main.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGD(TAG, "%s, start", __func__);
    xTaskCreate(hidlink_main_task, "hidlink", 4096, NULL, 10, NULL);
    ESP_LOGD(TAG, "%s, stop", __func__);
    vTaskDelete(NULL);
}
