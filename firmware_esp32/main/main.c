#include "main.h"

static const char *TAG = "main";

void app_main(void) {
    ESP_LOGI(TAG, "%s, enter", __func__);
    xTaskCreate(hidlink_main_task, "hidlink", 4096, NULL, 2, NULL);
    ESP_LOGI(TAG, "%s, exit", __func__);
    vTaskDelete(NULL);
}
