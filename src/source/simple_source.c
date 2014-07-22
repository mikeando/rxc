#include "rxc.h"
#include "rxc/simple_source.h"

#include <assert.h>

struct rxc_simple_source__data {
  unsigned int n_subscriptions;
  rxc_subscription ** subscriptions;
};

typedef struct rxc_simple_source__data rxc_simple_source__data;

static void rxc_simple_source__notify(rxc_source * s, void * data) {

  rxc_simple_source__data * source_data = (rxc_simple_source__data*)(s->internal_data);
  assert(source_data!=NULL);
  for(unsigned int i=0; i<source_data->n_subscriptions; ++i) {
    rxc_subscription * subscription = source_data->subscriptions[i];
    assert(subscription!=NULL);
    rxc_observer_next(subscription->observer, subscription, data);
  }
}

static void rxc_simple_source__free(rxc_source * self) {
  //TODO: Let everyone know and wait for their unsubscriptions
  //      Means we should just set a flag here, then delete everything
  //      in unsubscribe...
  rxc_simple_source__data * source_data = (rxc_simple_source__data*)(self->internal_data);
  assert(source_data!=NULL);
  rxc__free(source_data->subscriptions);
  rxc__free(source_data);
}

static void rxc_simple_source__subscribe(rxc_subscription * subscription) {
  rxc_source * self = subscription->source;
  rxc_simple_source__data * source_data = (rxc_simple_source__data*)(self->internal_data);
  assert(source_data!=NULL);

  rxc_subscription** new_subscriptions = (rxc_subscription**)rxc__malloc( 
      (source_data->n_subscriptions +1) * sizeof(rxc_subscription*) );

  if(source_data->subscriptions!=NULL) {
    memcpy(new_subscriptions, source_data->subscriptions, 
        source_data->n_subscriptions * sizeof(rxc_subscription*) );
    rxc__free(source_data->subscriptions);
  }
 
  new_subscriptions[source_data->n_subscriptions] = subscription;
  source_data->n_subscriptions++;
  source_data->subscriptions = new_subscriptions;
}

static rxc_source__vtable rxc_simple_source__vtable = {
  rxc_simple_source__subscribe,
  rxc_simple_source__free,
  rxc_simple_source__notify,
};

int rxc_simple_source_create(rxc_source**source) {
  //TODO: Clean up if rxc_source_create fails.
  rxc_simple_source__data * data = (rxc_simple_source__data*) rxc__malloc(sizeof(rxc_simple_source__data));
  memset(data,0,sizeof(rxc_simple_source__data));
  data->n_subscriptions = 0;
  data->subscriptions = NULL;
  return rxc_source_create(source, &rxc_simple_source__vtable, data, NULL);
}

