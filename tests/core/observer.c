//
//  tests/core/observer.c
//
//  Created by Michael Anderson on 28/05/2014.
//  Copyright (c) 2014 Michael Anderson. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "clar/clar.h"
#include "rxc.h"



void test_core_observer__can_create(void) {
  rxc_observer * observer = NULL;
  int ok = rxc_observer_create(&observer);
  cl_assert_equal_i(0,ok);
  cl_assert(observer!=NULL);
  rxc_observer_free(observer);
}


static void a_test_objs_next(rxc_observer * observer, rxc_subscription * subscription, void * data) {
}

static void a_test_objs_on_error(rxc_observer * observer, rxc_subscription * subscription, void * data) {
}

static void a_test_objs_complete(rxc_observer * observer, rxc_subscription * subscription) {
}

void test_core_observer__can_set_callbacks(void) {
  rxc_observer * observer = NULL;
  int ok = rxc_observer_create(&observer);
  cl_assert_equal_i(0,ok);

  ok = rxc_observer_set_on_next_callback(observer,a_test_objs_next);
  cl_assert_equal_i(0,ok);

  ok = rxc_observer_set_on_error_callback(observer,a_test_objs_on_error);
  cl_assert_equal_i(0,ok);

  ok = rxc_observer_set_on_done_callback(observer,a_test_objs_complete);
  cl_assert_equal_i(0,ok);

  rxc_observer_free(observer);

}

// vim: sw=2 ts=2 expandtab
