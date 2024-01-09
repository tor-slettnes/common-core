Common Core Apps Demo -- API
============================

This abstract API forms the contract between our event source/producer and its downstream consumers. It is intended solely for illustration, so the methods may seem a bit random.

Methods
-------

The following methods are provided

* `say_hello(Greeting)` to issue a greeting to whomever may be listening via `signal_greeting`

* `get_current_time() -> TimeData` returns the current time

* `start_ticking()` to start a scheduled task that emits time updates via `signal_time` once per second

* `stop_ticking()` to cancel any existing time update task

These are declared in [demo-api.h++](demo-api.h++).


Signals
-------

Event notifications are issued via the following two signals:

```
shared::signal::Signal<TimeData> signal_time("cc::demo::signal_time");
shared::signal::MappedSignal<Greeting> signal_greeting("cc::demo::signal_greeting", true);
```

The difference between `Signal<T>` and `MappedSignal<T>` lies in whether the
payload is emitted as a singular transient event or as an
addition/update/removal of an item in a keyed data set (map).

### `Signal<T>`: Singular/atomic event

In the former case, a signal is emitted on as follows:

```
cc::demo::TimeData time_data(...);
shared::signal::signal_time.emit(time_data);
```

Correspondingly, the callback function signature required to connect to this signal is

```
std::function<void(cc::demo::TimeData)>
```

### `MappedSignal<T>`: Addition/Update/Removal of data in a map

This type of signal is intended for use cases where a collection of data items
identified by a key can change over time. For instance, a NetworkManager proxy
might use a `MappedSignal<AccessPoint>` instance to issue a notification
whenever an AP is newly discovered, has updated data, or is no longer available.

This template's `emit()` method nominally takes three arguments:

```
shared::signal::MappedSignal<T, K>::emit(MappingChange change, K key, T value)
```

In our case, a new client might issue a greeting like this:

```
cc::demo::signal_greeting.emit(shared::signal::MAP_ADDITION,
                              ::options->identity,
                              cc::demo::Greeting(...));
```

These three arguments are then passed onto any connected slots, which in this
case would need the function signature

```
std::function<void(shared::signal::MappingChange, std::string, cc::demo::Greeting)>;
```
