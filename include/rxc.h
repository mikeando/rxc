#ifndef rxc_h
#define rxc_h
typedef struct rxc_observer rxc_observer;
typedef struct rxc_subscription rxc_subscription;


int rxc_observer_create(rxc_observer** observer);
int rxc_observer_free(rxc_observer* observer);

typedef void(*rxc_observer_next_callback)(rxc_observer * self, rxc_subscription * subscription, void * data);
typedef void(*rxc_observer_error_callback)(rxc_observer * self, rxc_subscription * subscription, void * data);
typedef void(*rxc_observer_done_callback)(rxc_observer * self, rxc_subscription * subscription);


int rxc_observer_set_on_next_callback(
    rxc_observer* observer,
    rxc_observer_next_callback on_next
    );

int rxc_observer_set_on_error_callback(
    rxc_observer* observer,
    rxc_observer_error_callback on_error
    );

int rxc_observer_set_on_done_callback(
    rxc_observer* observer,
    rxc_observer_done_callback on_done
    );

/* These are used by sources to notify observers */
int rxc_observer_next(rxc_observer* observer, void* value);
int rxc_observer_done(rxc_observer* observer);

typedef struct rxc_source rxc_source;
int rxc_source_create(rxc_source** source);
int rxc_source_free(rxc_source* observer);

//Source factory functions
int rxc_source_create_seq(rxc_source ** source, int min, int max);


rxc_subscription * rxc_source_subscribe(rxc_source * source, rxc_observer * observer, void * user_data);
void rxc_subscription_unsubscribe(rxc_subscription * subscription);

void * rxc_subscription_data(rxc_subscription * subscription);


#endif
