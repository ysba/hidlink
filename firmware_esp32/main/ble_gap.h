#ifndef __BLE_GAP__
#define __BLE_GAP__

uint8_t *ble_gap_get_scan_data();
uint32_t ble_gap_get_scan_data_len();
void ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

#endif