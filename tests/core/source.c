//
//  tests/core/source.c
//
//  Created by Michael Anderson on 30/05/2014.
//  Copyright (c) 2014 Michael Anderson. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "clar/clar.h"
#include "rxc.h"

//Since we're going to check the internals are set up correctly we need to know the internals ;)
#include "rxc/internal/types.h"

void test_source__v_cb_subscribe(rxc_subscription * subscription) {
}

void test_source__v_free(rxc_source * self) {
} 
void test_source__v_notify(rxc_source * self, void* notification) {
} 
static void test_ob_next(rxc_observer * self, rxc_subscription * subscription, void * data) {
}
static void test_ob_done(rxc_observer * self, rxc_subscription * subscription) {
}
static void test_ob_error(rxc_observer * self, rxc_subscription * subscription, void * data) {
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
  test_ob_free,
};

void test_core_source__subscribe_returns_a_subscription(void) {
  int ok;
  rxc_source * source = NULL;
  rxc_observer * observer = NULL;

  rxc_source__vtable vtable = {
    test_source__v_cb_subscribe,
    test_source__v_free,
    test_source__v_notify

  };

  ok = rxc_source_create(&source, &vtable, NULL,NULL);
  cl_assert_equal_i(0,ok);
  ok = rxc_observer_create(&observer,&ob_vtable,NULL);
  cl_assert_equal_i(0,ok);

  //The NULL user data is stored into the subscription (along with the observer) so that the callbacks can access them
  rxc_subscription * subscription = rxc_source_subscribe(source, observer, NULL);

  cl_assert(subscription!=NULL);


  rxc_subscription_unsubscribe(subscription);
  // Caller owns the subscription, and so must delete it!
  rxc_subscription_free(subscription);
  rxc_source_free(source);
  rxc_observer_free(observer);
}

struct callback_info_for_test {
  int number_of_value_callbacks;
  int last_value;
  int first_value;
  int number_of_done_callbacks;
  int number_of_error_callbacks;
};
typedef struct callback_info_for_test callback_info_for_test;

void dummy_on_next(rxc_observer* self, rxc_subscription * subscription, void * data) {
  int * ip = (int*)data;
  callback_info_for_test * info = (callback_info_for_test*)rxc_subscription_data(subscription);
  if(info->number_of_value_callbacks==0) {
    info->first_value = *ip;
  }
  info->number_of_value_callbacks++;
  info->last_value = *ip;
}

void dummy_on_done(rxc_observer * self, rxc_subscription * subscription) {
  callback_info_for_test * info = (callback_info_for_test*)rxc_subscription_data(subscription);
  info->number_of_done_callbacks++;
}

void dummy_on_error(rxc_observer * self, rxc_subscription * subscription, void * error ) {
  callback_info_for_test * info = (callback_info_for_test*)rxc_subscription_data(subscription);
  info->number_of_error_callbacks++;
}

static rxc_observer_vtable seq_test_ob_vtable = {
  dummy_on_next,
  dummy_on_done,
  dummy_on_error,
  test_ob_on_subscribe,
  test_ob_free,
};


void test_core_source__seq_source(void) {

  int ok;
  rxc_source * source = NULL;
  rxc_observer * observer = NULL;

  ok = rxc_source_create_seq(&source, 1, 10);

  rxc_observer_create(&observer, &seq_test_ob_vtable, NULL);


  struct callback_info_for_test info = {0,0,0,0,0};

  rxc_subscription * subscription = rxc_source_subscribe(source, observer, &info );

  // A seq source should notify the connecting observers immediately

  cl_assert_equal_i(10, info.number_of_value_callbacks);
  cl_assert_equal_i(10, info.last_value);
  cl_assert_equal_i(1,  info.first_value);
  cl_assert_equal_i(1,  info.number_of_done_callbacks);
  cl_assert_equal_i(0,  info.number_of_error_callbacks);

  rxc_subscription_unsubscribe(subscription);

  rxc_subscription_free(subscription);
  rxc_source_free(source);
  rxc_observer_free(observer);

}

void test_core_source__notify(void) {

}
