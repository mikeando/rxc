#include "rxc.h"
#include <stdlib.h>
#include <string.h>

#include "rxc/internal/types.h"
#include "rxc/internal/memory.h"

int rxc_observer_create(rxc_observer** observer) {
  *observer = (rxc_observer*)rxc__malloc(sizeof(rxc_observer));
  memset((rxc_observer*)*observer,0,sizeof(rxc_observer));
  (*observer)->vtable = (rxc_observer__vtable*)rxc__malloc(sizeof(rxc_observer__vtable));
  memset((rxc_observer__vtable*)(*observer)->vtable,0,sizeof(rxc_observer__vtable));
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
  // Should we really duplicate the vtable and then set?
  ((rxc_observer__vtable*)observer->vtable)->error = on_error;
  return 0;
}

int rxc_observer_set_on_done_callback(
    rxc_observer* observer,
    rxc_observer_done_callback on_done) {
  // Should we really duplicate the vtable and then set?
  ((rxc_observer__vtable*)observer->vtable)->done = on_done;
  return 0;
}

//Note: we pass the self parameter even though it could be obtained from the subscription
//      this is more for consistency than necessity - maybe this should be moved into the
//      subscription class?
void rxc_observer_next(
  rxc_observer * self,
  rxc_subscription * subscription,
  void * value
  ) {
    self->vtable->next(self, subscription, value);
}

void rxc_observer_done(
  rxc_observer * self,
  rxc_subscription * subscription
  ) {
    self->vtable->done(self, subscription);
}
