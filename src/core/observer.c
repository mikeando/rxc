#include "rxc.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rxc/internal/types.h"
#include "rxc/internal/memory.h"

static void validate_vtable(const rxc_observer_vtable * vtable ) {
  assert(vtable);
  assert(vtable->next);
  assert(vtable->done);
  assert(vtable->error);
  assert(vtable->on_subscribe);
  assert(vtable->free);
}

int rxc_observer_create(rxc_observer** observer, const rxc_observer_vtable * vtable, void * data) {
  if(observer==NULL)
    return 1;
  if(vtable==NULL)
    return 1;

  validate_vtable(vtable);
  
  rxc_observer * ob = (rxc_observer*)rxc__malloc(sizeof(rxc_observer));
  memset((rxc_observer*)ob,0,sizeof(rxc_observer));
  ob->vtable = vtable;
  ob->data = data;
  *observer = ob;
  return 0;
}

int rxc_observer_free(rxc_observer* observer) {
  assert(observer->vtable);
  assert(observer->vtable->free);
  observer->vtable->free(observer);
  rxc__free(observer);
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
    assert(self->vtable);
    assert(self->vtable->next);

    self->vtable->next(self, subscription, value);
}

void rxc_observer_done(
  rxc_observer * self,
  rxc_subscription * subscription
  ) {
    self->vtable->done(self, subscription);
}
