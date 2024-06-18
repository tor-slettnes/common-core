#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signal.py
## @brief Wrapper for ProtoBuf types in `signal.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install path
from .variant import Value, ValueList
from .wellknown import Message
from .utils import enumToValue, check_type
from .import_proto import import_proto
from core.invocation import safe_invoke

### Standard Python modules
from typing import Optional, Callable, Mapping, Union
import enum
import threading
import asyncio

### Import generated types from `signal.proto`. Symbols will appear within a new
### `cc.signal` namespace.
import_proto('signal', globals())


#===============================================================================
# Annotation types

Slot = Callable[[Message], None]
MappingAction = enum.Enum('MappingAction', cc.signal.MappingAction.items())

#===============================================================================
# SignalStore class

class SignalStore:
    '''Base implementation of signal/slot pattern using ProtoBuf messages as
    payload. Emitted signals can thus be serialized and propagated over
    transports such as gRPC, thereby emulating the pub/sub pattern.

    `SignalStore` works with a ProtoBuf wrapper message containing a `oneof`
    selector to discriminate between multiple message types, as illustrated in
    the following template:

    ```proto
    message Signal
    {
        oneof signal
        {
            MyTypeA signal_a = 8;
            MyTypeB signal_b = 9;
            ...
        }
    }
    ```

    This message would normally be defined in your `.proto` file, containing the
    events that you want to make available as signals to your consumers.  (See
    also `messaging.grpc.signal_client.SignalClient()` for details on
    propagating these signals over gRPC.)

    To connect to such a signal to a callback handler ("slot"), use the
    `connect_signal()` method below.  Alternatively you can use `connect_all()`
    to invoke the handler for any emitted signal.

    Correspondingly, to emit a signal, use `emit()` or `emit_event()`. The
    former takes a preconstructed `Signal` message and passes it on directly to
    connected slots, whereas the latter wraps this by taking in the signal name
    (e.g. `signal_a` in the example above) and associated value (in this case, a
    `MyTypeA` instance).

    This implementation also supports a specialization of the above, which we
    will refer to as "mapping signal". Here, two extra fields are introduced:

    ```proto
    message Signal
    {
        // Mapping action: one of MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL
        cc.signal.MappingAction mapping_action = 1;

        // Mapping key
        string mapping_key = 2;

        oneof signal
        {
            MyTypeA signal_a = 8;
            MyTypeB signal_b = 9;
            ...
        }
    }
    ```

    This is mainly useful where a producer/emitter and its consumers need to
    maintain synchronized data sets. The producer indicates whether a particular
    object has been added to, updated in, or removed from a synchronized map.
    To emit a mapping signal, use the `emit_mapping()` method, below.  At
    present, only a single mapping key is supported, and it must be a string.

    Finally, this implementation can optionally cache the latest emission of
    each signal -- or in the case of mapping signals, the latest emission for
    each key/value pair in the map.  This cache is subsequently replayed when
    new consumer invokes one of the `connect*()` methods.  Effectively this
    eliminates the "late subscriber" issue, where the consumer would otherwise
    have to make explicit out-of-band queries to obtain the current state or
    data set.  (This should however not be used for stateful controls, where the
    consumer takes stateful actions based on received signals, because the
    action would be repeated after any and all disconnect/reconnnect cycles).

    '''

    ## `signal_type` should be set in the final subclass, and is used to decode
    ##  ProtoBuf messages streamed back from the server's `watch()` method.
    signal_type = None


    def __init__(self,
                 use_cache   : bool = False,
                 signal_type : type = None):

        '''Instance initialization.

        If `use_cache` is set, cache the latest emitted instance of each signal.
        These are then replayed in response to any future `connect_signal()` or
        `connect_all()` invocations by invoking the corresponding callback
        methods.  In the case of mapping signals (see the `SignalStore` class
        description), the callback handler is invoked once for every key/value
        pair currently in the cache.

        The `signal_type` argument specifies what ProtoBuf message type to use
        to store/propagate signals.  This type should at minium contain a
        `oneof` selector with one or more signal alternatives, and optionally
        mapping fields `mapping_action` and `mapping_key`. For details see
        the `SignalStore` class description.

        '''

        if signal_type:
            self.signal_type = signal_type

        self.init_signals(use_cache)

    def init_signals(self,
                     use_cache: bool):

        assert self.signal_type,\
            ("%s instance must define `signal_type`, either by "
             "declaring a static member or passing it as argument "
             "to SignalStore.__init__()"%(type(self).__name__,))

        assert isinstance(self.signal_type, type) and issubclass(self.signal_type, Message), \
            'Signal type must be derived from %s, not %s'% \
            (Message.__name__, self.signal_type)

        self.slots                = {}
        self._completion_event    = threading.Event()
        self._completion_mutex    = threading.Lock()
        self._completion_deadline = None
        self._deadline_expired    = False

        if use_cache:
            self._cache = {}
        else:
            self._cache = None


    def descriptor(self):
        return self.signal_type.DESCRIPTOR


    def field_name(self, fieldnumber: int) -> str:
        '''
        Obtain the signal name associated with the specified field number.

        @return
            Number associated with the field name of the Signal message.

        @exception KeyError
            The specified field number does not exist
        '''
        try:
            return self.descriptor().fields_by_number[fieldnumber].name
        except KeyError:
            return None

    def field_number(self, fieldname: str) -> int:
        '''
        Return the field number associated with the specified signal name.

        @return
            Number associated with the field name of the Signal message.

        @exception KeyError
            The specified field does not exist.
        '''

        return self.descriptor().fields_by_name[signalname].number

    def signal_fields(self) -> dict:
        '''Return a dictionary of signal names to corresponding field numbers
        within the protobuf Signal message'''

        items = [(f.name, f.number)
                 for f in self.descriptor().oneofs[0].fields]

        return dict(items)

    def signal_names(self) -> list:
        '''
        Return a list of signal/slot names in the protobuf Signal message
        for this service.
        '''
        return [f.name for f in self.descriptor().oneofs[0].fields]

    def signal_name(self, msg):
        selector = msg.DESCRIPTOR.oneofs[0].name
        return msg.WhichOneof(selector) or ""



    def get_cached(self,
                   signalname: str,
                   timeout: float=3) -> Mapping[str, Message]:
        '''Get a specific signal map from the local cache.

        @param[in] signalname
            Signal name, corresponding to a field of the Signal message
            streamed from the server's `watch()` method.

        @param[in] timeout
            If the specified signal does not yet exist in the local cache, allow
            up to the specified timeout for it to be received from the server.
            Mainly applicable immediately after `start_watching()`, before the
            server cache has been received.

        @exception KeyError
            The specified `signalname` is not known

        @return
            The cached value, or None if the specified signal has yet not been
            received.
        '''

        if self._cache is None:
            raise RuntimeError("Signal cache is not enabled in %s instance"%
                               (type(self).__name__,))

        elif signalname in self.signal_fields():
            self.wait_complete()
            return self._cache.get(signalname, {})

        else:
            return self._cache[signalname]

    def connect_all(self,
                    slot: Slot):
        '''
        Connect a handler to _all_ signals in this store.
        '''

        self.slots.setdefault(None, []).append(slot)
        for name in self.signal_names():
            self.emit_cached_to(name, slot)


    def disconnect_all(self,
                       slot: Optional[Slot] = None):
        '''
        Disconnect handlers that are connected to _all_ signals.
        '''

        if slot:
            self.disconnect_signal(None, slot)
        else:
            self.slots.pop(None, None)


    def connect_signal(self,
                       name: str,
                       slot: Slot):

        '''Connect a callback handler (slot) to receive emitted Signal messages.

        @param[in] name
            Signal name, corresponding to a field within a `oneof` block of the
            signal message.

        @param[in] slot
            A callable handler (e.g. a function) that accepts the Signal
            instance as its first and only required argument.

        This variant is mainly suitable for mapping signals, since the entire
        Signal message including the `mapping_action` and `mapping_key`
        fields are passed on to the receiver.  For simple signals where these
        fields are unused, consider instead `connect_signal_data()` which passes
        on just the extracted payload from the named signal.

        '''

        assert callable(slot), \
            "Slot must be a callable object, like a function"

        assert name in self.signal_names(), \
            "Message type %s does not have a %r field"%(self.signal_type.__name__, name)

        self.slots.setdefault(name, []).append(slot)
        self.emit_cached_to(name, slot)


    def disconnect_signal(self,
                          name: str,
                          slot: Optional[Slot] = None):
        '''Disconnect a simple handler from the specified signal.

        @param[in] name
           Signal name

        @param[in] slot
           Signal handler. If not provided, remove all handlers from the
           specified signal slot.

        Returns True if the handler was found and removed; False otherwise.
        '''

        if slot:
            try:
                slots = self.slots[name]
                slots.remove(slot)
            except KeyError:
                return False
            except ValueError:
                if not slots:
                    self.slots.pop(name, None)
                return True
        else:
            return bool(self.slots.pop(name, None))


    def connect_signal_data(self,
                            name: str,
                            slot: Slot):

        '''.

        Connect a callback handler (slot) to a receive just the extracted
        payload from emitted signals.  This is mainly suitable for simple
        (event) signals rather than mapping signals (where the receiver also
        needs the `mapping_action` and `mapping_key` fields from the original `Signal`
        container).

        @param[in] name
            Signal name, corresponding to a field of the Signal message.

        @param[in] slot
            A callable handler (e.g. a function) that accepts the
            extracted signal data as its first and only required argument.

        '''

        self.connect_signal(name,
                            lambda signal: slot(getattr(signal, name)))



    def disconnect_signal_data(self,
                               name: str,
                               slot: Optional[Slot] = None):
        '''Disconnect a callback handler (slot) from a specific simple signal.

        @param[in] name
            Signal name, corresponding to a field of the Signal message.

        @param[in] slot
            A callable handler (e.g. a function) that accepts the
            extracted signal data as its first and only required argument.
        '''

        if slot:
            _slot = (lambda signal: slot(getattr(signal, name)))
        else:
            _slot = None

        self.disconnect_signal(name, _slot)


    def wait_complete(self) -> bool:
        '''
        Wait for all currently mapping signals to be received from the server.
        May be invoked after first connectint to ensure the local cache is
        complete before proceeding.

        Note that it is not usually necessary to invoke this method in order to
        obtain values from the local cache, because the `get_cached()` method
        implicitly waits for the specified signal map to be completed before
        proceeding.
        '''
        if self._completion_deadline and not self._deadline_expired:
            remaining = self._completion_deadline - time.time()
            if remaining > 0:
                self._deadline_expired = not self._completion_event.wait(remaining)

        return self._completion_event.is_set()


    def emit_cached_to(self, name, slot):
        if self._cache:
            mapped = False
            for key, signal in self.get_cached(name).items():
                self._emit_to(name, slot, signal)
                mapped = bool(key)

            ## Once the cache is exhausted, emit an empty signal
            ## to indicate that intialization is complete.
            if mapped:
                self._emit_to(name, slot, self.signal_type())


    def emit(self, msg):
        action, key = self._mapping_controls(msg);
        name = self.signal_name(msg)

        if name:
            if isinstance(self._cache, dict):
                self._update_cache(name, action, key, msg)

            ## Invoke each slot that was connected to this signal by name
            for callback in self.slots.get(name, []):
                self._emit_to(name, callback, msg)

            ## Invoke each slot that was connected to `all` signals
            for callback in self.slots.get(None, []):
                self._emit_to(name, callback, msg)

        if action == MappingAction.MAP_NONE:
            self._completion_event.set()

    def emit_event(self,
                   signal_name : str,
                   value       : Message):

        signal = self.signal_type(**{signal_name:value})
        self.emit(signal)

    def emit_mapping(self,
                     signal_name : str,
                     action      : MappingAction,
                     key         : str,
                     value       : Message):

        if key and not enumToValue(action):
            if value.ByteSize() == 0:
                action = MappingAction.MAP_REMOVAL
            elif key in self.get_cached(signal_name, {}):
                action = MappingAction.MAP_UPDATE
            else:
                action = MappingAction.MAP_ADDITION

        signal = self.signal_type(mapping_action = enumToValue(action),
                                  mapping_key = key,
                                  **{signal_name: value})
        self.emit(signal)


    def _update_cache(self, signalname, action, key, data):
        datamap = self._cache.setdefault(signalname, {})
        if action in (MappingAction.MAP_ADDITION, MappingAction.MAP_UPDATE):
            datamap[key] = data
        elif action == MappingAction.MAP_REMOVAL:
            datamap.pop(key, None)

    def _emit_to(self, name, slot, signal):
        action, key = self._mapping_controls(signal)

        result = safe_invoke(slot,
                             (signal,),
                             {},
                             "Signal %s slot %s(%s, %r, ...)"%(
                                 name,
                                 slot.__name__,
                                 action.name,
                                 key))
        if asyncio.iscoroutine(result):
            asyncio.create_task(result)

    def _mapping_controls(self, signal) -> tuple[MappingAction, str]:
        try:
            action = MappingAction(signal.mapping_action)
            key = signal.mapping_key
        except (AttributeError, KeyError, ValueError):
            action = MappingAction.MAP_NONE
            key = None

        return (action, key)

