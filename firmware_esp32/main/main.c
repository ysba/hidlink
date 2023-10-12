#include "main.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGD(TAG, "%s, start", __func__);
    esp_log_level_set("MAIN", ESP_LOG_DEBUG);
    esp_log_level_set("HIDLINK", ESP_LOG_DEBUG);
    esp_log_level_set("BT_GAP", ESP_LOG_DEBUG);
    esp_log_level_set("BT_HID_HOST", ESP_LOG_DEBUG);
    esp_log_level_set("BLE_GAP", ESP_LOG_DEBUG);
    esp_log_level_set("BLE_GATTS", ESP_LOG_DEBUG);
    esp_log_level_set("BLE_CDTP", ESP_LOG_DEBUG);
    xTaskCreate(hidlink_main_task, "hidlink", 4096, NULL, 10, NULL);
    ESP_LOGD(TAG, "%s, stop", __func__);
    vTaskDelete(NULL);
}
