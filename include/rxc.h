#ifndef rxc_h
#define rxc_h

/**
 * Type forward declarations
 */
typedef struct rxc_subscription rxc_subscription;
typedef struct rxc_observer rxc_observer;
typedef struct rxc_source rxc_source;

/**
 * Subscriptions
 */

void rxc_subscription_unsubscribe(rxc_subscription * subscription);
void * rxc_subscription_data(rxc_subscription * subscription);
void rxc_subscription_free(rxc_subscription * subscription);

/**
 * Observers
 */

/**
 * Observer Callbacks and VTable
 */
typedef void(*rxc_observer_next_callback)(rxc_observer * self, rxc_subscription * subscription, void * data);
typedef void(*rxc_observer_error_callback)(rxc_observer * self, rxc_subscription * subscription, void * data);
typedef void(*rxc_observer_done_callback)(rxc_observer * self, rxc_subscription * subscription);
typedef void(*rxc_observer_on_subscribe_callback)(rxc_subscription * subscription);
typedef void(*rxc_observer_free_function)(rxc_observer * self);

struct rxc_observer_vtable {
  rxc_observer_next_callback next;
  rxc_observer_done_callback done;
  rxc_observer_error_callback error;
  rxc_observer_on_subscribe_callback on_subscribe;
  rxc_observer_free_function free;
};
typedef struct rxc_observer_vtable rxc_observer_vtable;

int rxc_observer_create(rxc_observer** observer, const rxc_observer_vtable * vtable, void * data);
int rxc_observer_free(rxc_observer* observer);

/* These are used by sources to notify observers */
//TODO: WHat should the return types be?
void rxc_observer_next(rxc_observer* observer, rxc_subscription* subscription, void* value);
void rxc_observer_done(rxc_observer* observer, rxc_subscription* subscription);

/**
 * Source
 */

//TODO: This should probably be internal!
typedef struct rxc_source__vtable rxc_source__vtable;
//TODO: This should probably become internal!
int rxc_source_create(
    rxc_source** source,
    const rxc_source__vtable * vtable,
    void * data);
int rxc_source_free(rxc_source* observer);


//TODO: Need some tests for me!
void rxc_source_notify(rxc_source * source, void * data);


rxc_subscription * rxc_source_subscribe(rxc_source * source, rxc_observer * observer, void * user_data);

//Source factory functions
int rxc_source_create_seq(rxc_source ** source, int min, int max);

#endif
