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

//TODO: Not sure it makes sense to have a naked rxc_source_create...
void test_core_source__can__create(void) {
  rxc_source * source = NULL;
  int ok = rxc_source_create(&source);
  cl_assert_equal_i(0,ok);
  cl_assert(source!=NULL);
  rxc_source_free(source);
}

void test_core_source__subscribe_returns_a_subscription(void) {
  int ok;
  rxc_source * source = NULL;
  rxc_observer * observer = NULL;

  ok = rxc_source_create(&source);
  cl_assert_equal_i(0,ok);
  ok = rxc_observer_create(&observer);
  cl_assert_equal_i(0,ok);

  //The NULL user data is stored into the subscription (along with the observer) so that the callbacks can access them
  rxc_subscription * subscription = rxc_source_subscribe(source, observer, NULL);

  cl_assert(subscription!=NULL);

  rxc_source_free(source);
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


void test_core_source__seq_source(void) {

  int ok;
  rxc_source * source = NULL;
  rxc_observer * observer = NULL;

  ok = rxc_source_create_seq(&source, 1, 10);

  rxc_observer_create(&observer);
  rxc_observer_set_on_next_callback(observer, dummy_on_next );
  rxc_observer_set_on_done_callback(observer, dummy_on_done );
  rxc_observer_set_on_error_callback(observer, dummy_on_error );


  struct callback_info_for_test info = {};

  rxc_subscription * subscription = rxc_source_subscribe(source, observer, &info );

  //TODO: Set the callbacks

  // A seq source should notify the connecting observers immediately

  cl_assert_equal_i(10, info.number_of_value_callbacks);
  cl_assert_equal_i(10, info.last_value);
  cl_assert_equal_i(1,  info.first_value);
  cl_assert_equal_i(1,  info.number_of_done_callbacks);
  cl_assert_equal_i(0,  info.number_of_error_callbacks);

  rxc_subscription_unsubscribe(subscription);

}
