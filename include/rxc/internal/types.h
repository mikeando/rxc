#ifndef RXC_INTERNAL_TYPES_H
#define RXC_INTERNAL_TYPES_H

#include "rxc.h"

struct rxc_observer__vtable {
  rxc_observer_next_callback next;
  rxc_observer_done_callback done;
};

typedef struct rxc_observer__vtable rxc_observer__vtable;

struct rxc_observer {
  const rxc_observer__vtable * vtable;
};


typedef void(*rxc_source__cb_subscribe)(rxc_source * self, rxc_subscription * subscription, rxc_observer * observer );

struct rxc_source__vtable {
  rxc_source__cb_subscribe on_subscribe;
};

typedef struct rxc_source__vtable rxc_source__vtable;

struct rxc_source {
  const rxc_source__vtable * vtable;
  void * data;
};

//TODO: This should get observer and source members
struct rxc_subscription {
  void * data;
};

#endif
