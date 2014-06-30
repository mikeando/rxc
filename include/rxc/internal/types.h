#ifndef RXC_INTERNAL_TYPES_H
#define RXC_INTERNAL_TYPES_H

#include "rxc.h"


struct rxc_observer {
  const rxc_observer_vtable * vtable;
  void * data;
};


typedef void(*rxc_source__cb_subscribe)(rxc_subscription * subscription);
typedef void(*rxc_source__cb_free)(rxc_source * self);
typedef void(*rxc_source__notify)(rxc_source * self, void * data);

struct rxc_source__vtable {
  rxc_source__cb_subscribe on_subscribe;
  rxc_source__cb_free free;
  rxc_source__notify notify;
};

//TODO: Uncomment this when the definition is removed from rxc.h
//typedef struct rxc_source__vtable rxc_source__vtable;

struct rxc_source {
  const rxc_source__vtable * vtable;
  void * data;
};

//TODO: This should get observer and source members
struct rxc_subscription {
  void * data;
  rxc_source * source;
  rxc_observer * observer;
};

#endif
