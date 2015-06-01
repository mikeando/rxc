# Reactive Extensions for C (RXC)

Pure library implementation of reactive extensions for C.

The AIM is to be C99 compatable. But at the moment the code is
only getting compiled with the OS X 10.8 clang default settings.

# What it contains

## Sources
## Observers
## Properties

# What it does

# How it differs from C# / Java etc.

There are a few big differences compared to a library for C# or Java or other
language with rich typing / generics / templates and garbage collection. These primarily have to
do with the limitations of working in C.

## Type Safety
Since C doesn't support generics, the source and observer code is forced to work using `void*` data.

The properties code is slightly higher level and works instead with a discriminated enum type. It is likely that 
the enum type will be adjusted to support custom types with reference counted data etc.

It is possible that the source and observer code will be adjusted to use the same enum structure.

But all this leaves you with much more room for error when passing types around.

Whether this will require the creation of macros to help create real type-safe
observers, sources and properties remains to be seen.

## Memory management
Memory management for RXC is tricky. In particular we need to ensure that we
never send messages from sources to destroyed observers, or have observers unsubscribe from
destroyed sources. Similarly we must not end up with floating objects.

This means we need several contracts about orders of destruction and object ownership.

## Thread safety
Aaargh Threads!

### Reference Counts

* retain
* release

objects are created with a reference count of 1.

If you have a retained pointer to an object, then it will not be destroyed until after you have released it.


### Object states

* Live - object exists and is usable
* Dieing - object exists, but is in a limited partially shutting down state.
* Dead - object no longer exists - all access to it are undefined behaviuor.

#### Object state transition functions
after_become_live
before_become_dieing - can be used to prevent the transition to dieing
after_become_dead
before_become_dead - last thing called before guts are deallocated - can be used to prevent the destruction.

### Sources

What are the functions every source must support?

* The OSTs

* free - called by the "owner" when the source no longer makes sense
* subscribe - called by an observer, and gives us back a subscription
* unsubscribe - called by an observer when it no-longer cares about the source (but only takes a subscription?)

* notify - Send a message to all observers.
* error -  When there's an error and all observers should go away.
* done - When the observers should all go away.

#### `rxc_source_free(source)`
If the source is Dieing - do nothing.

Will make the source Dead if it has no subscribers.
Otherwise will make the source Dieing. 

#### `rxc_source_subscribe(source)`
If source is Dieing - return an error. E_RXC_SOURCE_DIEING
If source is Live - add the subscription to the list of subscriptions

#### `rxc_source_unsubscribe(source)`
If source is Dieing - remove subscription - if no more subscriptions become Dead.
If source is Live - remove subscription

#### `rxc_source_noitfy(source, noitification)`
If source is Dieing - do not notify - return E_RXC_SOURCE_DIEING
If source is Live - notify all subscribers

### Observers 

#### `rxc_observer_free`
if the observer is dieing - do nothing
transition to dieing
if the observer has a subscription - unsubscribe - ignoring E_RXC_SOURCE_DIEING
transition to dead

#### `rxc_observer_notify`
if observer is dieing - do nothing
otherwise pass on to vtable

#### `rxc_observer_unsubscribe(subscription)`
????


### Subscriptions
#### `rxc_subscription_free`
#### `rxc_subscription_unsubscribe(subscription)`
#### `rxc_subscription_unsubscribe_source(subscription)`
#### `rxc_subscription_unsubscribe_observer(subscription)`

### Properties
#### `rxc_property_free`
#### `rxc_property_get`
#### `rxc_property_subscribe`
#### rxc_property_????









### Destroying an observer
*
*
*

# Unsubscribing

# Getting a message

# What are the conventions

# What are the contracts

