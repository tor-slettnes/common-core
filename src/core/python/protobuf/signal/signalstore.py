'''
Implementation of signal/slot pattern using ProtoBuf messages as payload.
Emitted signals can be serialized and propagated over transports such as gRPC,
thereby emulating the pub/sub pattern.
'''

docformat = 'javadoc en'
author = 'Tor Slettnes'

### Standard Python modules
from typing import Optional, Callable, Mapping, Union, Sequence
import threading
import asyncio

### Modules withn package
from ...core.invocation import safe_invoke
from ..utils import proto_enum
from .signal_pb2 import Filter, MappingAction

### Third-party modules
from google.protobuf.message import Message
from google.protobuf.descriptor import Descriptor, FieldDescriptor


#===============================================================================
# Annotation types

MappingAction = proto_enum(MappingAction)
SignalMessage = Message
Slot = Callable[[Message], None]

#===============================================================================
# SignalStore class

class SignalStore:
    '''
    Implementation of signal/slot pattern using ProtoBuf messages as
    payload. Emitted signals can be serialized and propagated over transports
    such as gRPC, thereby emulating the pub/sub pattern.

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

    Correspondingly, to emit such a signal, use `emit()` or `emit_event()`. The
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
    To emit a mapping signal, use either `emit()` or `emit_mapping()`, below.
    At present, only a single mapping key is supported, and it must be a string.

    Finally, a `CachingSignalStore()` class derived from this one is also
    available; see `cachingsignalstore.py`. This flavor retains the latest
    emitted signal (or in the case of a mapping signal, the latest emitted
    update for each mapping key) in memory, and then replays this cache in
    response to future `connect*()` calls. Effectively this eliminates the "late
    observer" issue, where the consumer would otherwise have to make explicit
    out-of-band queries to obtain the current state or data set.
    '''

    ## `signal_type` should be set in the final subclass, and is used to decode
    ##  ProtoBuf messages streamed back from the server's `watch()` method.
    signal_type = None

    ## Wildcard used to connect to all signals
    ALL_SIGNALS = '*'

    def __init__(self, signal_type : type = None):
        '''
        Instance initialization.

        The `signal_type` argument specifies what ProtoBuf message type to use
        to store/propagate signals.  This type should at minium contain a
        `oneof` selector with one or more signal alternatives, and optionally
        mapping fields `mapping_action` and `mapping_key`. For details see
        the `SignalStore` class description.
        '''

        if signal_type:
            self.signal_type = signal_type

        self.init_signals()

    def init_signals(self):

        assert self.signal_type,\
            ("%s instance must define `signal_type`, either by "
             "declaring a static member or passing it as argument "
             "to SignalStore.__init__()"%(type(self).__name__,))

        assert isinstance(self.signal_type, type) and issubclass(self.signal_type, Message), \
            'Signal type must be derived from %s, not %s'% \
            (Message.__name__, self.signal_type)

        self.slots = {}


    def descriptor(self) -> Descriptor:
        '''
        Return the ProtoBuf message descriptor for the underlying Signal
        message associated with this store instance.
        '''
        return self.signal_type.DESCRIPTOR

    def signal_fields(self) -> Sequence[FieldDescriptor]:
        '''
        Return the ProtoBuf field descriptors for each signal field within
        the underlying Signal message associated with this store instance.
        '''
        return self.descriptor().oneofs[0].fields

    def field_name(self, fieldnumber: int) -> str:
        '''
        Obtain the signal name associated with the specified field number.

        @returns
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

        @returns
            Number associated with the field name of the Signal message.

        @exception KeyError
            The specified field does not exist.
        '''

        return self.descriptor().fields_by_name[signalname].number

    def signal_field_numbers(self, filter: Filter = Filter()) -> dict[str, int]:
        '''
        Return a dictionary of signal names to corresponding field numbers
        within the protobuf Signal message

        @param filter
            Optional field selection
        '''

        polarity = bool(filter.polarity)
        indices  = set(filter.indices)

        return {f.name: f.number
                for f in self.descriptor().oneofs[0].fields
                if polarity == (f.number in indices)}

    def signal_names(self, filter: Filter = Filter()) -> list[str]:
        '''
        Return a list of signal/slot names in the ProtoBuf Signal message
        for this store.

        @param filter
            Optional field selection
        '''
        polarity = bool(filter.polarity)
        indices  = set(filter.indices)

        return [f.name
                for f in self.descriptor().oneofs[0].fields
                if polarity == (f.number in indices)]


    def signal_name(self, msg: SignalMessage):
        '''
        Return the name of the signal currently selected within the provided
        Protobuf Signal message.
        '''
        selector = msg.DESCRIPTOR.oneofs[0].name
        return msg.WhichOneof(selector) or ""

    def connect_all(self,
                    slot: Slot):
        '''
        Connect a handler to _all_ signals in this store.
        '''

        self.slots.setdefault(self.ALL_SIGNALS, []).append(slot)


    def disconnect_all(self,
                       slot: Optional[Slot] = None):
        '''
        Disconnect handlers that are connected to _all_ signals.
        '''

        if slot:
            self.disconnect_signal(None, slot)
        else:
            self.slots.pop(self.ALL_SIGNALS, None)


    def connect_signal(self,
                       name: str,
                       slot: Slot):

        '''
        Connect a callback handler (slot) to receive emitted `Signal`
        messages.

        @param name:
            Signal name, corresponding to a field within a `oneof`
            block of the signal message.

        @param slot:
            A callable handler (e.g. a function) that accepts the Signal
            instance as its first and only required argument.

        This variant is mainly suitable for mapping signals, since the entire
        Signal message including the `mapping_action` and `mapping_key` fields
        are passed on to the receiver.  For simple signals where these fields
        are unused, consider instead `connect_signal_data()` which passes on
        just the extracted payload from the named signal.
        '''

        assert callable(slot), \
            "Slot must be a callable object, like a function"

        if not name in self.signal_names():
            raise AttributeError(
                "Message type %s does not have a %r field" %
                (self.signal_type.__name__, name))

        self.slots.setdefault(name, []).append(slot)


    def disconnect_signal(self,
                          name: str,
                          slot: Optional[Slot] = None):
        '''
        Disconnect a simple handler from the specified signal.

        @param name:
           Signal name

        @param slot:
           Signal handler. If not provided, remove all handlers from the
           specified signal slot.

        Returns `True` if the handler was found and removed; `False` otherwise.
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

        '''
        Connect a callback handler (slot) to a receive just the extracted
        payload from emitted signals.  This is mainly suitable for simple
        (event) signals rather than mapping signals (where the receiver also
        needs the `mapping_action` and `mapping_key` fields from the original
        `Signal` container).

        @param name:
            Signal name, corresponding to a field of the Signal message.

        @param slot:
            A callable handler (e.g. a function) that accepts the extracted
            signal data as its first and only required argument.
        '''

        assert callable(slot), \
            "Slot must be a callable object, like a function"

        if not name in self.signal_names():
            raise AttributeError(
                "Message type %s does not have a %r field"
                %(self.signal_type.__name__, name))

        self.connect_signal(
            name,
            lambda signal: slot(getattr(signal, name)))


    def signal_filter(self, match_all: bool = False) -> Filter:
        '''
        Return a ProtoBuf Filter message that may be provided to peer in
        order to capture applicable Signal messages into this store.

        The filter matches all available signals if `match_all` is True,
        otherwise it matches signals that are currently connected.
        '''
        if match_all or self.ALL_SIGNALS in self.slots:
            return Filter(polarity=False)
        else:
            return Filter(
                polarity = True,
                indices = [self.field_number(signal_name)
                           for signal_name in self.slots
                           if signal_name is not self.ALL_SIGNALS],
            )


    def disconnect_signal_data(self,
                               name: str,
                               slot: Optional[Slot] = None):
        '''
        Disconnect a callback handler (slot) from a specific simple signal.

        @param name Signal name, corresponding to a field of the Signal
            message.

        @param slot:
            A callable handler (e.g. a function) that accepts the
            extracted signal data as its first and only required argument.
        '''

        if slot:
            _slot = (lambda signal: slot(getattr(signal, name)))
        else:
            _slot = None

        self.disconnect_signal(name, _slot)


    def emit(self, msg: Message):
        '''
        Emit a preconstructed `Signal` message to connected slots, i.e.
        invoke registered callback handlers with this message as their sole
        argument.

        See also:
          - `emit_event()`, which wraps this by constructing the required
             simple Signal message from the signal name and associated value, and

          - `emit_mapping()` which constructs a mapping signal (with fields
            `mapping_action` and `mapping_key` as described above).
        '''

        if signal_name := self.signal_name(msg):
            ## Invoke each slot that was connected to this signal by name
            for callback in self.slots.get(signal_name, []):
                self._emit_to(signal_name, callback, msg)

            ## Invoke each slot that was connected to `all` signals
            for callback in self.slots.get(self.ALL_SIGNALS, []):
                self._emit_to(signal_name, callback, msg)

    def emit_event(self,
                   signal_name : str,
                   value       : Message):

        '''
        Construct and emit a `Signal` message as described above, with the
        field indicated by `signal_name` set to `value`.
        '''

        signal = self.signal_type(**{signal_name: value})
        self.emit(signal)

    def emit_mapping(self,
                     signal_name  : str,
                     action       : MappingAction,
                     key          : str,
                     value        : Message):
        '''
        Construct and emit a `Signal` message as described above, with
         * `mapping_key` set to `key`
         * `mapping_action` set to `action`
         * the field indicated by `signal_name` set to `value`.
        '''

        signal = self.signal_type(mapping_action = action,
                                  mapping_key = key,
                                  **{signal_name: value})
        self.emit(signal)


    @classmethod
    def is_addition_or_update(cls, action: MappingAction):
        return mapping_action in (
            MappingAction.ADDITION,
            MappingAction.UPDATE,
        )

    @classmethod
    def is_removal(cls, action: MappingAction):
        return (mapping_action == MappingAction.REMOVAL)

    @classmethod
    def is_empty(cls, value: Message|None):
        return not value or (value.ByteSize() == 0)

    def _emit_to(self,
                 signal_name : str,
                 slot: Slot,
                 signal: SignalMessage):

        action, key = self._mapping_controls(signal)
        result = safe_invoke(slot,
                             (signal,),
                             {},
                             "Signal %s slot %s(%s, %r, ...)"%(
                                 signal_name,
                                 slot.__name__,
                                 action.name,
                                 key))
        if asyncio.iscoroutine(result):
            asyncio.create_task(result)

    def _mapping_controls(self, signal: SignalMessage) -> tuple[MappingAction, str]:
        try:
            action = MappingAction(signal.mapping_action)
            key = signal.mapping_key or None
        except (AttributeError, KeyError, ValueError):
            action = MappingAction.NONE
            key = None

        return (action, key)
