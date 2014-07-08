#include "rxc.h"
#include "rxc/simple_source.h"
//p is the property this source belongs to.
int rxc_property__create_source(rxc_source** source, void * p) {
  //TODO: We need to track the subscriptions...
  return rxc_simple_source_create(source);
}

rxc_property * rxc_property_create(const rxc_property_vtable* vtable) {
  rxc_property * p = (rxc_property*)rxc__malloc(sizeof(rxc_property));
  memset(p, 0, sizeof(rxc_property));
  p->value = (rxc_property_value*)rxc__malloc(sizeof(rxc_property_value));
  memset(p->value, 0, sizeof(rxc_property_value));
  p->vtable = vtable;
  //TODO: Check errors!
  rxc_property__create_source(&p->source,p);
  return p;
}

void rxc_property_free(rxc_property* p) {
  //TODO: Should hide the observer behind a vtable?
  p->vtable->free(p);

  rxc__free(p->value);
  rxc_source_free(p->source);
  rxc__free(p);
}


void rxc_property_value_copy( rxc_property_value * target, const rxc_property_value * source) {
  // For now this struct is pretty dumb, so a straight memcpy will do the job.
  memcpy(target,source,sizeof(rxc_property_value));
}


void rxc_property_get(rxc_property* p, rxc_property_value *v) {
  rxc_property_value_copy(v,p->value);
}

void rxc_property_set(rxc_property* p, const rxc_property_value *v) {
  rxc_property_value_copy(p->value, v);
  rxc_event event = { RXC_PROPERTY_CHANGED_EVENT, 
                      p };
  rxc_source * source = rxc_property_source(p);
  rxc_source_notify(source, &event);
}

void rxc_property_set_integer(rxc_property* p, int i) {
  rxc_property_value v = {};
  v.type = RXC_PROPERTY_INT;
  v.value.as_int = i;
  rxc_property_set(p,&v);
}

rxc_source * rxc_property_source(rxc_property * p) {
  return p->source;
}

struct rxc_property_derived_data {
  rxc_observer * observer;
  int n_subscriptions;
  rxc_subscription ** subscriptions;
  void(*map)(rxc_property*, rxc_property**, int);
};

typedef struct rxc_property_derived_data rxc_property_derived_data;

void prop_observer_next(rxc_observer * self, rxc_subscription * subscription, void * data) {
  //TODO: Implement me
}
void prop_observer_error(rxc_observer * self, rxc_subscription * subscription, void * data) {
  //TODO: Implement me
}
void prop_observer_done(rxc_observer * self, rxc_subscription * subscription) {
  //TODO: Implement me
}
void prop_observer_on_subscribe(rxc_subscription * subscription) {
  //TODO: Implement me
}
void prop_observer_free(rxc_observer * self) {
  //TODO: Implement me
}

static rxc_observer_vtable derived_observer_vtable = {
  prop_observer_next,
  prop_observer_done,
  prop_observer_error,
  prop_observer_on_subscribe,
  prop_observer_free
};

void rxc_property__derived_free(rxc_property*self);
void rxc_property__derived_free(rxc_property*self) {
  rxc_property_derived_data * data = (rxc_property_derived_data*)self->data;
  for(int i=0; i<data->n_subscriptions; ++i) {
    //TODO: Should we unsubscribe here?
    rxc_subscription_free(data->subscriptions[i]);
  }
  rxc__free(data->subscriptions);
  rxc_observer_free(data->observer);
  rxc__free(data);
}

static rxc_property_vtable derived_property_vtable = {
  rxc_property__derived_free,
  //TODO: Fill me in.
};



rxc_property* rxc_property_derived_create( void(*map)(rxc_property*, rxc_property**, int), rxc_property** properties, int n_properties) {
  rxc_property * property = rxc_property_create(&derived_property_vtable);
  rxc_property_derived_data * data = (rxc_property_derived_data*)rxc__malloc(sizeof(rxc_property_derived_data));
  memset(data,0,sizeof(rxc_property_derived_data));

  rxc_observer_create(&data->observer, &derived_observer_vtable, property);

  // Now we need to set listeners on the other properties.
  rxc_subscription ** subs= (rxc_subscription**)rxc__malloc(n_properties*sizeof(rxc_subscription*));
  for( int i=0; i<n_properties; ++i) {
    rxc_source * source = rxc_property_source(properties[i]);
    subs[i] = rxc_source_subscribe(source,data->observer,NULL);
  }

  data->n_subscriptions = n_properties;
  data->subscriptions = subs;
  data->map = map;

  property->data = data;

  return property;
} 

