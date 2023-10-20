#ifndef __RING__
#define __RING__

#define RING_HANDLE_MAX     1

typedef uint32_t ring_handle_t;

typedef uint8_t ring_data_t;

void ring_init();
ring_handle_t ring_create(uint32_t len);
void ring_destroy(ring_handle_t handle);
void ring_push(ring_handle_t handle, ring_data_t data);
bool ring_pop(ring_handle_t handle, ring_data_t *data);

#endif
