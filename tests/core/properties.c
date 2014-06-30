//
//  tests/core/source.c
//
//  Created by Michael Anderson on 16/06/2014.
//  Copyright (c) 2014 Michael Anderson. All rights reserved.
//
//

#include "clar/clar.h"
#include <assert.h>

#include "rxc/simple_source.h"

#include "rxc.h"
#include "rxc/internal/memory.h"
#include "rxc/internal/types.h"
enum rxc_property_value_type {
  RXC_PROPERTY_UNKNOWN = 0,
  RXC_PROPERTY_INT = 1
};

typedef enum rxc_property_value_type rxc_property_value_type;

struct rxc_property_value {
  rxc_property_value_type type;
  union {
    int as_int;
  } value;
};

typedef struct rxc_property_value rxc_property_value;


struct rxc_property {
  rxc_property_value * value;
  rxc_source * source;
};


typedef struct rxc_property rxc_property;

struct rxc_event {
  //TODO: Make this a more robust type
  void * event_type;
  void * data;
};

typedef struct rxc_event rxc_event;

static void * RXC_PROPERTY_CHANGED_EVENT = &RXC_PROPERTY_CHANGED_EVENT;

#include <stdio.h>


//p is the property this source belongs to.
int rxc_property__create_source(rxc_source** source, void * p) {
  //TODO: We need to track the subscriptions...
  return rxc_simple_source_create(source);
}

rxc_property * rxc_property_create();
rxc_property * rxc_property_create() {
  rxc_property * p = (rxc_property*)rxc__malloc(sizeof(rxc_property));
  memset(p, 0, sizeof(rxc_property));
  p->value = (rxc_property_value*)rxc__malloc(sizeof(rxc_property_value));
  memset(p->value, 0, sizeof(rxc_property_value));
  //TODO: Check errors!
  rxc_property__create_source(&p->source,p);
  return p;
}

rxc_property* rxc_property_derived_create( void(*map)(rxc_property*, rxc_property**, int), rxc_property**, int); 


void test_int_add(rxc_property* v, rxc_property** ps, int n) {
}


void rxc_property_value_copy( rxc_property_value * target, const rxc_property_value * source);
void rxc_property_value_copy( rxc_property_value * target, const rxc_property_value * source) {
  // For now this struct is pretty dumb, so a straight memcpy will do the job.
  memcpy(target,source,sizeof(rxc_property_value));
}

void rxc_property_get(rxc_property* p, rxc_property_value *v);
void rxc_property_set_integer(rxc_property* p, int i);
void rxc_property_set(rxc_property* p, const rxc_property_value *v);

rxc_source* rxc_property_source(rxc_property* p);

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

//TODO: Fill this in!
static rxc_observer_vtable derived_observer_vtable = {
  prop_observer_next,
  prop_observer_done,
  prop_observer_error,
  prop_observer_on_subscribe,
  prop_observer_free
};

rxc_property* rxc_property_derived_create( void(*map)(rxc_property*, rxc_property**, int), rxc_property** properties, int n_properties) {
  rxc_property * property = rxc_property_create();

  //TODO: Need to check rxc_observer_create return value
  rxc_observer * observer=NULL;
  rxc_observer_create(&observer, &derived_observer_vtable, property);

  // Now we need to set listeners on the other properties.
  rxc_subscription ** subs= (rxc_subscription**)rxc__malloc(n_properties*sizeof(rxc_subscription*));
  for( int i=0; i<n_properties; ++i) {
    rxc_source * source = rxc_property_source(properties[i]);
    subs[i] = rxc_source_subscribe(source,observer,NULL);
  }

  return property;
} 

// This is more of a functional test than a unit
// test, as bits start working I'll build unit
// tests for them below, but getting this guy
// woriking is a longer term target.
void test_core_properties__chained_properties(void) {

  // Lets try chaining some int properties
  rxc_property * i1 = rxc_property_create();
  rxc_property * i2 = rxc_property_create();
  rxc_property * ps[2] = {i1,i2};
  rxc_property * d1 = rxc_property_derived_create(test_int_add, ps, 2);
  cl_assert(d1!=NULL);

  rxc_property_value v = {};
  cl_assert( v.type == RXC_PROPERTY_UNKNOWN );
  rxc_property_get(d1,&v);
  cl_assert( v.type == RXC_PROPERTY_UNKNOWN );

  rxc_property_set_integer(i1, 10);
  rxc_property_set_integer(i2, 20);

  rxc_property_get(d1, &v);
  cl_assert( v.type == RXC_PROPERTY_INT );
  cl_assert_equal_i(30, v.value.as_int);
}

void test_core_properties__set_and_get(void) {
  
  rxc_property * p1 = rxc_property_create();
  rxc_property_value v = {};

  rxc_property_get(p1, &v);
  cl_assert( v.type == RXC_PROPERTY_UNKNOWN );

  rxc_property_set_integer(p1, 10);
  rxc_property_get(p1, &v);
  cl_assert( v.type == RXC_PROPERTY_INT );
  cl_assert_equal_i(10, v.value.as_int);
}

void test_core_properties__raw_get_and_set(void) {
  
  rxc_property * p1 = rxc_property_create();
  rxc_property_value v = {};

  rxc_property_value v2 = {};
  v2.type = RXC_PROPERTY_INT;
  v2.value.as_int = 10;

  rxc_property_set(p1, &v2);
  rxc_property_get(p1, &v);
  cl_assert( v.type == RXC_PROPERTY_INT );
  cl_assert_equal_i(10, v.value.as_int);
}

void test_core_properties__property_value_copy(void) {
  rxc_property_value v = {};
  v.type = RXC_PROPERTY_INT;
  v.value.as_int = 10;

  rxc_property_value v_cpy = {};
  rxc_property_value_copy(&v_cpy,&v);

  cl_assert( v_cpy.type==RXC_PROPERTY_INT );
  cl_assert( v_cpy.value.as_int==10 );
}

void test_core_properties__properties_have_sources(void) {
  rxc_property * p = rxc_property_create();
  rxc_source * s1 = rxc_property_source(p);
  rxc_source * s2 = rxc_property_source(p);

  cl_assert( s1!=NULL );
  cl_assert( s2!=NULL );
  cl_assert( s1==s2 );
}

struct test_data {
  int notification_count;
  void * last_p;
};






void test_ob_next(rxc_observer * self, rxc_subscription * subscription, void * data) {
  rxc_event * event = (rxc_event*)data;
  cl_assert( event->event_type == RXC_PROPERTY_CHANGED_EVENT );

  struct test_data * ob_data = (struct test_data*)self->data;
  ob_data->notification_count++;
  ob_data->last_p = event->data;
}

void test_ob_done(rxc_observer * self, rxc_subscription * subscription) {
}

void test_ob_error(rxc_observer * self, rxc_subscription * subscription, void * data) {
}

void test_ob_on_subscribe(rxc_subscription * subscription) {
}

void test_ob_free(rxc_observer * self) {
}

void test_core_properties__property_create_source_works(void) {
  struct test_data data = {};
  //Now check we get notifications
  cl_assert_equal_i(0, data.notification_count);
  cl_assert( NULL == data.last_p );

  void * dummy = &dummy;
  rxc_source * s = NULL;
  rxc_property__create_source(&s, dummy);
  cl_assert( s!=NULL);

  rxc_observer_vtable ob_vtable = {};
  ob_vtable.next = test_ob_next;
  ob_vtable.done = test_ob_done;
  ob_vtable.error = test_ob_error;
  ob_vtable.on_subscribe = test_ob_on_subscribe;
  ob_vtable.free = test_ob_free;

  rxc_observer * ob=NULL;
  rxc_observer_create(&ob, &ob_vtable, &data);
  cl_assert(ob!=NULL);

  rxc_subscription * sub = rxc_source_subscribe(s,ob,NULL);

  cl_assert(sub!=NULL);

  rxc_event event = { RXC_PROPERTY_CHANGED_EVENT, 
                      dummy };

  rxc_source_notify(s, &event);

  cl_assert_equal_i(1, data.notification_count);
  cl_assert( dummy == data.last_p );

}

void test_core_properties__set_sends_change_event(void) {
  struct test_data data = {};

  rxc_property * p = rxc_property_create();
  rxc_observer_vtable ob_vtable = {};
  ob_vtable.next = test_ob_next;
  ob_vtable.done = test_ob_done;
  ob_vtable.error = test_ob_error;
  ob_vtable.on_subscribe = test_ob_on_subscribe;
  ob_vtable.free = test_ob_free;
  ob_vtable.next = test_ob_next;

  rxc_observer * ob = NULL;
  rxc_observer_create(&ob, &ob_vtable, &data);

  rxc_source * source = rxc_property_source(p);
  rxc_subscription * sub = rxc_source_subscribe(source,ob,NULL);

  rxc_property_set_integer(p,7);

  cl_assert_equal_i(1, data.notification_count);
  cl_assert( data.last_p == p );
}
