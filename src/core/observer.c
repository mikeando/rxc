#include "rxc.h"
#include <stdlib.h>

#include "rxc/internal/types.h"

int rxc_observer_create(rxc_observer** observer) {
  *observer = (rxc_observer*)malloc(sizeof(rxc_observer));
  (*observer)->vtable = (rxc_observer__vtable*)malloc(sizeof(rxc_observer__vtable));
  return 0;
}

int rxc_observer_free(rxc_observer* observer) {
  free((void*)observer->vtable);
  free(observer);
  return 0;
}

int rxc_observer_set_on_next_callback(
    rxc_observer* observer,
    rxc_observer_next_callback on_next ) {
  // Should we really duplicate the vtable and then set?
  ((rxc_observer__vtable*)observer->vtable)->next = on_next;
  return 0;
}

int rxc_observer_set_on_error_callback(
    rxc_observer* observer,
    rxc_observer_error_callback on_error) {
  return 1;
}

int rxc_observer_set_on_done_callback(
    rxc_observer* observer, 
    rxc_observer_done_callback on_done) {
  // Should we really duplicate the vtable and then set?
  ((rxc_observer__vtable*)observer->vtable)->done = on_done;
  return 0;
}


