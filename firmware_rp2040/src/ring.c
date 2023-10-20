#include "main.h"


typedef struct {
    uint32_t push;
    uint32_t pop;
    uint32_t len;
    union {
        uint32_t val;
        struct {
            bool busy:1;
            bool full:1;
        } bits;
    } flags;
    uint8_t *data;
} ring_buffer_t;

ring_buffer_t ring[RING_HANDLE_MAX] = {0};


ring_handle_t ring_create(uint32_t len) {

    ring_handle_t i;
    ring_handle_t ret_handle = RING_HANDLE_MAX;

    for (i = 0; i < RING_HANDLE_MAX; i++) {

        if (ring[i].flags.bits.busy == false) {

            ring[i].data = (uint8_t *) malloc(len * sizeof(ring_data_t));

            if (ring[i].data != NULL) {

                ring[i].push = 0;
                ring[i].pop = 0;
                ring[i].len = len;
                ring[i].flags.bits.full = false;
                ring[i].flags.bits.busy = true;

                ret_handle = i;
                break;
            }
        }
    }

    return (ret_handle);
}


void ring_destroy(ring_handle_t handle) {

    if (handle >= RING_HANDLE_MAX)
        return;

    ring[handle].flags.bits.busy = false;
}


void ring_push(ring_handle_t handle, ring_data_t data) {

    ring_buffer_t *r;

    if (handle >= RING_HANDLE_MAX)
        return;

    if (ring[handle].flags.bits.busy == false) 
        return;

    r = &ring[handle];

    r->data[r->push++] = data;

    if (r->push >= r->len)
        r->push = 0;

    if (r->flags.bits.full == false) {
        if (r->push == r->pop) {
            r->flags.bits.full = true;
        }
    }
    else {
        r->pop = r->push;
    }
}


bool ring_pop(ring_handle_t handle, ring_data_t *data) {

    ring_buffer_t *r;
    bool ret_val = false;
    
    if (handle < RING_HANDLE_MAX && ring[handle].flags.bits.busy == true) {

        r = &ring[handle];

        if (r->push != r->pop || r->flags.bits.full == true) {
            ret_val = true;
            r->flags.bits.full = false;
            *data = r->data[r->pop++];
            if (r->pop >= r->len) {
                r->pop = 0;
            }
        }
    }
        
    return (ret_val);
}
