Common Core Apps Example -- Native C++ Library
===============================================

This self-contained library represents the first and most basic implementation of our [example API](../../api/README.md). In this approach, the underlying functionality is provided directly within the class [cc::demo::NativeImplementation()](example-native.h++). As such, this library can be linked into a standalone C++ executable.

Conversely, since this implementation does not involve any client/server communication, there is no corresponding DDS bus, RPC service, or bindings for other languages. There is also no way to send or receive greetings to or from other processes, only to ourselves.

Notifications
-------------

As for time notifiations, the `cc::demo::NativeImplementation::start_ticking()` method uses our shared [task scheduler](../../../../../../shared/source/cpp/platform/chrono/scheduler.h++) to invoke a specified callback function at the specified frequency. Conversely, `cc::demo::CoreProvider::stop_ticking()` cancels this task.

Similarly, greeting notices are issued via `cc::demo::signal_greeting`.

### *Why the indirection*, you ask.

Good question, I wish I had thought of that myself.  There's some general information on the signal/slot pattern on [on Wikipedia](https://en.wikipedia.org/wiki/Signals_and_slots). Basically:

* It simplifies one-to-many notifications. In our time example the event source (the task scheduler) handles multiple callback functions just fine, but what about the greeting? We'd otherwise have to add a boilerplate callback function which in turn would further distribute the event, and... wait, we're back to signals!

* It standardizes callback semantics. Or, put differently, it loosens the coupling between the event source and the receiver. There is a uniform way to register to receive signal events (`.connect()`) and to unregister (`.disconnect()`), and the signature of the callback method ("slot") is well-defined.

* Specific to our codebase, `Signal<>` template described above as well as the related `MappedSignal<>` template both support optional caching of the last emitted event (or in the case of `MappedSignal<>`, the current map). If enabled this cache is then replayed to any new callback function once connected, thereby avoiding the race condition that would otherwise be associated with bringing the receiver in sync via a separate query.


Components exposed
------------------

Certain features from our [shared components](../../../../../../shared/source/cpp) library `cc_shared` are introduced here, including:

* [task scheduler](../../../../../../shared/source/cpp/platform/chrono/scheduler.h++),
* [signal/slot semantics](../../../../../../shared/source/cpp/platform/thread/signaltemplate.h++)
* [string formatting](../../../../../../shared/source/cpp/platform/string/format.h++),
* [logging](../../../../../../shared/source/cpp/platform/logging/logging.h++),
* [exception handling](../../../../../../shared/source/cpp/platform/status/exceptions.h++),
