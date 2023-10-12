#ifndef __BT_GAP__
#define __BT_GAP__

void bt_gap_event_handler(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

char *bda2str(esp_bd_addr_t bda, char *str, size_t size);

#endif
