#include "rxc.h"
#include <stdlib.h>

#include "rxc/internal/types.h"

//TODO: Only for debugging.
#include <stdio.h>


int rxc_source_create(rxc_source** source) {
  *source = (rxc_source*)malloc(sizeof(rxc_source));
  return 0;
}

int rxc_source_free(rxc_source* source) {
  free(source);
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
static void rxc_source_seq__cb_subscribe(rxc_source * self, rxc_subscription * subscription, rxc_observer * observer ) {
  rxc_source_seq__data * data = (rxc_source_seq__data*)self->data;
  for(int i=data->min; i<=data->max; ++i) {
    // Give the observer each value
    //TODO: Shouldn't this be wrapped in a rxc_observer_next(...);
    observer->vtable->next(observer, subscription, &i);
  }
    //TODO: Shouldn't this be wrapped in a rxc_observer_done(...);
  observer->vtable->done(observer, subscription);
}

static const rxc_source__vtable seq_vtable = {rxc_source_seq__cb_subscribe};

int rxc_source_create_seq(rxc_source ** source, int min, int max) {
  rxc_source_create(source);
  (*source)->vtable = &seq_vtable;
  rxc_source_seq__data * data = (rxc_source_seq__data*)malloc(sizeof(rxc_source_seq__data));
  (*source)->data = data;

  data->min = min;
  data->max = max;
  return 0;
}

void rxc_subscription_unsubscribe(rxc_subscription * subscription) {
}


//TODO: This should be moved
rxc_subscription * rxc_source_subscribe(rxc_source * source, rxc_observer * observer, void * user_data) {
  rxc_subscription * subscription = (rxc_subscription*)malloc(sizeof(rxc_subscription));
  subscription->data = user_data;

  //TODO: Should not having a vtable be an error? - probably!
  //TODO: Should the callback be able to veto the subscription?
  if(source->vtable) {
    if(source->vtable->on_subscribe) {
      source->vtable->on_subscribe(source,subscription,observer);
    }
  }

  return subscription;
}

void * rxc_subscription_data(rxc_subscription * subscription) {
  return subscription->data;
}
