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

static void a_test_objs_next(rxc_observer * observer, rxc_subscription * subscription, void * data) {
}

static void a_test_objs_on_error(rxc_observer * observer, rxc_subscription * subscription, void * data) {
}

static void a_test_objs_complete(rxc_observer * observer, rxc_subscription * subscription) {
}

static void a_test_objs_on_subscribe(rxc_subscription * subscription) {
}

static void a_test_objs_free(rxc_observer * self) {
}

static rxc_observer_vtable a_test_ob_vtable = {
  a_test_objs_next,
  a_test_objs_complete,
  a_test_objs_on_error,
  a_test_objs_on_subscribe,
  a_test_objs_free
};

void test_core_observer__can_create(void) {
  rxc_observer * observer = NULL;
  int ok = rxc_observer_create(&observer, &a_test_ob_vtable, NULL);
  cl_assert_equal_i(0,ok);
  cl_assert(observer!=NULL);
  rxc_observer_free(observer);
}

// vim: sw=2 ts=2 expandtab
