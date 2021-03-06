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
#include <stdio.h>

static rxc_property * i1 = NULL;
static rxc_property * i2 = NULL;
static rxc_property * d1 = NULL;

struct test_data {
  int notification_count;
  void * last_p;
};
static struct test_data * data = NULL;
static void * dummy = &dummy;

static rxc_source * naked_source = NULL;

static void test_ob_next(rxc_observer * self, rxc_subscription * subscription, void * indata) {
  rxc_event * event = (rxc_event*)indata;
  cl_assert( event->event_type == RXC_PROPERTY_CHANGED_EVENT );

  struct test_data * ob_data = (struct test_data*)self->data;
  ob_data->notification_count++;
  ob_data->last_p = event->data;
}

static void test_ob_done(rxc_observer * self, rxc_subscription * subscription) {
}

static void test_ob_error(rxc_observer * self, rxc_subscription * subscription, void * indata) {
}

static void test_ob_on_subscribe(rxc_subscription * subscription) {
}

static void test_ob_free(rxc_observer * self) {
}

static rxc_observer_vtable ob_vtable = {
  test_ob_next,
  test_ob_done,
  test_ob_error,
  test_ob_on_subscribe,
  test_ob_free
};

static rxc_observer * ob=NULL;

static void simple_property_free(rxc_property * p) {
}

static rxc_property_vtable simple_property_vtable = {
  simple_property_free
};

void test_core_properties__initialize(void) {
  i1 = rxc_property_create(&simple_property_vtable);
  i2 = rxc_property_create(&simple_property_vtable);


  d1 = NULL;

  data = (struct test_data*)malloc(sizeof(struct test_data));
  memset(data,0,sizeof(struct test_data));

  rxc_property__create_source(&naked_source, dummy);
  rxc_observer_create(&ob, &ob_vtable, data);
}

void test_core_properties__cleanup(void) {
  if(i2) {
    rxc_property_free(i2);
    i2 = NULL;
  }

  if(i1) {
    rxc_property_free(i1);
    i1 = NULL;
  }

  if(d1) {
    rxc_property_free(d1);
    d1 = NULL;
  }

  if(data) {
    free(data);
    data = NULL;
  }

  if(naked_source) {
    rxc_source_free(naked_source);
    naked_source = NULL;
  }

  if(ob) {
    rxc_observer_free(ob);
    ob=NULL;
  }
}


void test_int_add(rxc_property_value* v, rxc_property_value* ps, unsigned int n) {
  int sum = 0;
  for(unsigned int i=0; i<n; ++i) {
    if( ps[i].type!=RXC_PROPERTY_INT ) {
      //TODO Should we flag an error of some kind here.. if so how!
      v->type = RXC_PROPERTY_UNKNOWN;
      return;
    }
    sum += ps[i].value.as_int;
  }
  v->type = RXC_PROPERTY_INT;
  v->value.as_int = sum;
}

// This is more of a functional test than a unit
// test, as bits start working I'll build unit
// tests for them below, but getting this guy
// woriking is a longer term target.
void test_core_properties__chained_properties(void) {

  // Lets try chaining some int properties
  rxc_property * ps[2] = {i1,i2};
  d1 = rxc_property_derived_create(test_int_add, ps, 2);
  cl_assert(d1!=NULL);

  rxc_property_value v;
  memset(&v,0,sizeof(rxc_property_value));
  cl_assert( v.type == RXC_PROPERTY_UNKNOWN );
  rxc_property_get(d1,&v);
  cl_assert( v.type == RXC_PROPERTY_UNKNOWN );

  rxc_property_set_integer(i1, 10);
  rxc_property_set_integer(i2, 20);

  rxc_property_get(d1, &v);
  cl_assert( v.type == RXC_PROPERTY_INT );
  cl_assert_equal_i(30, v.value.as_int);
}


static int derived_map_call_count;

static void test_derived_map(rxc_property_value * out, rxc_property_value * values, unsigned int count) {
  cl_assert(count==1);
  derived_map_call_count++;
  out->type = RXC_PROPERTY_UNKNOWN;
}

void test_core_properties__derived_property_calls_map_on_create(void) {
  derived_map_call_count = 0;
  d1 = rxc_property_derived_create(test_derived_map, &i1, 1);
  cl_assert_equal_i(1, derived_map_call_count);
}

void test_core_properties__derived_property_calls_map_on_change_event(void) {
  derived_map_call_count = 0;
  d1 = rxc_property_derived_create(test_derived_map, &i1, 1);
  cl_assert_equal_i(1, derived_map_call_count);
  rxc_property_set_integer(i1,3);
  cl_assert_equal_i(2, derived_map_call_count);
}

void test_core_properties__derived_property_forwards_error(void) {
	cl_fail("NYI");
}

void test_core_properties__derived_property_forwards_done(void) {
	cl_fail("NYI");
}



void test_core_properties__set_and_get(void) {
  rxc_property_value v;
  memset(&v,0,sizeof(rxc_property_value));

  rxc_property_get(i1, &v);
  cl_assert( v.type == RXC_PROPERTY_UNKNOWN );

  rxc_property_set_integer(i1, 10);
  rxc_property_get(i1, &v);
  cl_assert( v.type == RXC_PROPERTY_INT );
  cl_assert_equal_i(10, v.value.as_int);
}

void test_core_properties__raw_get_and_set(void) {
  
  rxc_property_value v;
  memset(&v,0,sizeof(rxc_property_value));

  rxc_property_value v2;
  memset(&v2,0,sizeof(rxc_property_value));

  v2.type = RXC_PROPERTY_INT;
  v2.value.as_int = 10;

  rxc_property_set(i1, &v2);
  rxc_property_get(i1, &v);
  cl_assert( v.type == RXC_PROPERTY_INT );
  cl_assert_equal_i(10, v.value.as_int);
}

void test_core_properties__property_value_copy(void) {
  rxc_property_value v;
  memset(&v,0,sizeof(rxc_property_value));

  v.type = RXC_PROPERTY_INT;
  v.value.as_int = 10;

  rxc_property_value v_cpy;
  memset(&v_cpy,0,sizeof(rxc_property_value));
  rxc_property_value_copy(&v_cpy,&v);

  cl_assert( v_cpy.type==RXC_PROPERTY_INT );
  cl_assert( v_cpy.value.as_int==10 );
}

void test_core_properties__properties_have_sources(void) {
  rxc_source * s1 = rxc_property_source(i1);
  rxc_source * s2 = rxc_property_source(i1);
  rxc_source * s3 = rxc_property_source(i2);

  cl_assert( s1!=NULL );
  cl_assert( s2!=NULL );
  cl_assert( s3!=NULL );
  cl_assert( s1==s2 );
  cl_assert( s1!=s3 );
}


void test_core_properties__property_create_source_works(void) {

  rxc_subscription * sub = rxc_source_subscribe(naked_source,ob,NULL);

  cl_assert(sub!=NULL);

  rxc_event event = { RXC_PROPERTY_CHANGED_EVENT, 
                      dummy };

  rxc_source_notify(naked_source, &event);

  cl_assert_equal_i(1, data->notification_count);
  cl_assert( dummy == data->last_p );

  rxc_subscription_unsubscribe(sub);
  rxc_subscription_free(sub);
}

void test_core_properties__set_sends_change_event(void) {
  rxc_source * source = rxc_property_source(i1);
  rxc_subscription * sub = rxc_source_subscribe(source,ob,NULL);

  rxc_property_set_integer(i1,7);

  cl_assert_equal_i(1, data->notification_count);
  cl_assert( data->last_p == i1 );

  //Clean up
  rxc_subscription_unsubscribe(sub);
  rxc_subscription_free(sub);
}

void test_core_properties__property_source_data_is_property(void) {
  rxc_source * src = rxc_property_source(i1);
  void * data = src->user_data;
  
  cl_assert_equal_p(data,i1);
}
