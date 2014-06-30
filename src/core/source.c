#include "rxc.h"
#include <stdlib.h>

#include "rxc/internal/types.h"
#include "rxc/internal/memory.h"

//TODO: Only for debugging.
#include <stdio.h>

// NOTE: we *DONT* allocate or free the vtable as it
// will usually be a static variable.
// In the cases where it is dynamically allocated
// the caller must set it, and set its
// vtable->free to do the right freeing

//TODO: Should this take a data and vtable pointer?
int rxc_source_create(
    rxc_source** source,
    const rxc_source__vtable * vtable,
    void * data) {
  *source = (rxc_source*)rxc__malloc(sizeof(rxc_source));
  memset(*source, 0, sizeof(rxc_source));
  (*source)->vtable = vtable;
  (*source)->data = data;
  return 0;
}

int rxc_source_free(rxc_source* source) {
  // See rxc_source_create for notes on allocation and deallocation
  // of the vtable! 
  source->vtable->free(source);
  rxc__free(source);
  return 0;
}



//Source factory functions

// Sequence type
struct rxc_source_seq__data {
  int min;
  int max;
};

typedef struct rxc_source_seq__data rxc_source_seq__data;


//TODO: What about if an unsubscribe is fired from one of the callbacks
static void rxc_source_seq__cb_subscribe(rxc_subscription * subscription) {
  rxc_source * self = subscription->source;
  rxc_observer * observer = subscription->observer;
  rxc_source_seq__data * data = (rxc_source_seq__data*)self->data;
  for(int i=data->min; i<=data->max; ++i) {
    // Give the observer each value
    rxc_observer_next(observer, subscription, &i);
  }
  rxc_observer_done(observer, subscription);
}

void rxc_source_seq__free(rxc_source * self) {
  free(self->data);
}

static const rxc_source__vtable seq_vtable = {
  rxc_source_seq__cb_subscribe,
  rxc_source_seq__free
};

int rxc_source_create_seq(rxc_source ** source, int min, int max) {
  rxc_source_seq__data * data = (rxc_source_seq__data*)rxc__malloc(sizeof(rxc_source_seq__data));
  rxc_source_create(source, &seq_vtable, data);
  (*source)->data = data;

  data->min = min;
  data->max = max;
  return 0;
}

void rxc_subscription_unsubscribe(rxc_subscription * subscription) {
}


// Caller owns the returned soubscription. It is their responsibility to free the
// subscription after the source calls their error or done callbacks. In any
// other case if they wish to stop receiving notifications they _must_ call unsubscribe
// and then free the object.

rxc_subscription * rxc_source_subscribe(rxc_source * source, rxc_observer * observer, void * user_data) {

  rxc_subscription * subscription = (rxc_subscription*)rxc__malloc(sizeof(rxc_subscription));
  subscription->data = user_data;
  subscription->source = source;
  subscription->observer = observer;

  //TODO: Should not having a vtable be an error? - probably!
  //TODO: Should the callback be able to veto the subscription?
  if(source->vtable) {
    if(source->vtable->on_subscribe) {
      source->vtable->on_subscribe(subscription);
    }
  }

  if(observer->vtable) {
    if(observer->vtable->on_subscribe) {
      observer->vtable->on_subscribe(subscription);
    }
  }

  return subscription;
}

void * rxc_subscription_data(rxc_subscription * subscription) {
  return subscription->data;
}

void rxc_subscription_free(rxc_subscription * subscription) {
  rxc__free(subscription);
}

//TODO: Better error handling?
void rxc_source_notify(rxc_source * source, void * data) {
  if(!source)
    return;
  if(!source->vtable)
    return;
  if(!source->vtable->notify)
    return;
  source->vtable->notify(source,data);
}
