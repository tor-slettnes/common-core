'''
SignalStore implementation that caches the most recent emitted signal, or in
the case of mapping signals, the latest emission for each key/value pair in the
map.  This cache is subsequently replayed when new consumer invokes one of the
`connect*()` methods.  Effectively this eliminates the "late subscriber" issue,
where the consumer would otherwise have to make explicit out-of-band queries to
obtain the current state or data set.
'''

docformat = 'javadoc en'
author = 'Tor Slettnes'

### Standard Python modules
import threading

### Modules withn package
from cc.core.decorators import override, doc_inherit
from .signalstore import SignalStore, Message, MappingAction, SignalMessage, Slot

#===============================================================================
# CachingSignalStore class

class CachingSignalStore (SignalStore):
    '''
    Specialization of `SignalStore` that caches the most latest emission of
    each signal -- or in the case of mapping signals, the latest emission for
    each key/value pair in the map.  This cache is subsequently replayed when
    new consumer invokes one of the `connect*()` methods.  Effectively this
    eliminates the "late subscriber" issue, where the consumer would otherwise
    have to make explicit out-of-band queries to obtain the current state or
    data set.  (This should however not be used for stateful controls, where the
    consumer takes stateful actions based on received signals, because the
    action would be repeated after any and all disconnect/reconnnect cycles).
    '''

    @override
    def init_signals(self):
        SignalStore.init_signals(self)

        self._cache = {}

    @override
    def connect_all(self,
                    slot: Slot):
        '''
        Connect a handler to _all_ signals in this store.
        '''

        SignalStore.connect_all(self, slot)
        for name in self.signal_names():
            self.emit_cached_to(name, slot)

    @override
    def connect_signal(self,
                       name: str,
                       slot: Slot):

        SignalStore.connect_signal(self, name, slot)
        self.emit_cached_to(name, slot)


    @override
    def emit(self, msg: Message):
        action, key = self._mapping_controls(msg);

        if signal_name := self.signal_name(msg):
            self._update_cache(signal_name, action, key, msg)

        SignalStore.emit(self, msg)

    @override
    def emit_mapping(self,
                     signal_name  : str,
                     key          : str,
                     *,
                     value        : Message|None = None,
                     action       : MappingAction|None = None):
        '''
        Construct and emit a `Signal` message, with

        * `mapping_key` set to `key`

        * `mapping_action` set to `action` if specified, or else derived
          from the following table based on
          - whether `value` is provided, and
          - whether `key` already maps to an existing value in the signal cache,
            and whether that value identical to the provided `value`

        * the field indicated by `signal_name` set to `value`.

        +------------------+-----------------------------+-------------------+
        | provided `value` | previous value              | `mapping_action`  |
        +------------------+-----------------------------+-------------------+
        | No               | No                          | `None`            |
        | No               | Yes                         | `MAP_REMOVAL`     |
        | Yes              | No                          | `MAP_ADDITION`    |
        | Yes              | Yes, different from `value` | `MAP_UPDATE`      |
        | Yes              | Yes, identical to `value`   | `None`            |
        +------------------+-----------------------------+-------------------+

        If after this `mapping_action` remains `None`, no signal is emitted.
        '''

        if key and not action:
            cached_value = self.get_cached_signal(signal_name, key, wait_complete=False)

            action = (
                None if value is None and cached_value is None
                else MappingAction.REMOVAL if value is None
                else MappingAction.ADDITION if cached_value is None
                else MappingAction.UPDATE if value != cached_value
                else None
            )

        if action:
            SignalStore.emit_mapping(self,
                                     signal_name = signal_name,
                                     action = action,
                                     key = key,
                                     value = value)


    def emit_map_update(self,
                        signal_name : str,
                        updated_map : dict[str, Message],
                        empty_value: Message|None = None):
        '''
        Construct and emit multiple mapping signals as required to bring the
        signal cache in sync with `updated_map`.

        If `empty_value` is provided, this will be used instead of the
        currently-cached value for REMOVAL signals.
        '''

        current_map = self.get_cached_map(signal_name, False)
        for key, current_value in current_map.items():
            if not key in updated_map:
                self.emit_mapping(
                    signal_name,
                    action = MappingAction.REMOVAL,
                    key = key,
                    value = (current_value if empty_value is None
                             else empty_value))

        for key, updated_value in updated_map.items():
            self.emit_mapping(
                signal_name,
                action = None,
                key = key,
                value = updated_value)


    def emit_cached_to(self, signal_name: str, slot: Slot):
        '''
        Emit a cached (previously emitted) signal to a specific slot/receiver.
        '''

        if self._cache:
            for key, signal in self.get_cached_signal_messages(signal_name).items():
                self._emit_to(signal_name, slot, signal)


    def get_cached_map(self,
                       signal_name: str,
                       wait_complete: bool = True,
                       ) -> dict[str, Message]:
        '''
        Get a specific signal map from the local cache.

        @param signal_name:
            Signal name, corresponding to a field of the Signal message
            streamed from the server's `watch()` method.

        @param wait_complete
            If the specified signal does not yet exist in the local cache, wait
            until a initial completion event has been received from the server.

        @exception KeyError
            The specified `signal_name` is not known

        @returns
            The cached value map, or None if the specified mapping signal has
            yet not been received.
        '''

        signal_map = self.get_cached_signal_messages(signal_name, wait_complete)
        return {key: getattr(value, signal_name) for (key, value) in signal_map.items()}


    def get_cached_signal(self,
                          signal_name: str,
                          mapping_key: str|None = None,
                          wait_complete: bool = True,
                          fallback: Message|None = None,
                          ) -> Message:
        '''
        Get a specific signal from the local cache.

        @param signal_name
            Signal name, corresponding to a field of the Signal message
            streamed from the server's `watch()` method.

        @param mapping_key
            Mapping key used to look up a specific event within the
            signal cache.  Leave this as `None` for unmapped signals.

        @param wait_complete
            If the specified signal does not yet exist in the local cache, wait
            until a initial completion event has been received from the server.

        @param fallback
            Value to return if the requested signal has not yet been received.
            If provided, this will normally be the expected ProtoBuf message
            type, or an instance thereof.  In the former case, a new empty
            instance is returned.

        @exception KeyError
            The specified `signal_name` is not known

        @returns
            The cached value, or None if the specified data signal has yet not
            been received.
        '''

        try:
            signal = self.get_cached_signal_messages(signal_name, wait_complete)[mapping_key]
        except KeyError:
            return fallback() if isinstance(fallback, type) else fallback
        else:
            return getattr(signal, signal_name)


    def get_cached_signal_messages(self,
                                   signal_name: str,
                                   wait_complete: bool = True,
                                   ) -> dict[str, SignalMessage]:
        '''
        Get a specific signal map from the local cache.

        @param signal_name
            Signal name, corresponding to a field of the Signal message
            streamed from the server's `watch()` method.

        @param wait_complete
            If the specified signal does not yet exist in the local cache, wait
            until a initial completion event has been received from the server.

        @exception KeyError
            The specified `signal_name` is not known

        @returns
            The cached value map, or None if the specified mapping signal has
            yet not been received.
        '''

        try:
            return  self._cache[signal_name]
        except KeyError:
            if signal_name in self.descriptor().fields_by_name:
                if wait_complete:
                    self.wait_complete()
                return self._cache.get(signal_name, {})
            else:
                raise


    def _update_cache(self, signal_name, action, key, msg):
        datamap = self._cache.setdefault(signal_name, {})
        if action == MappingAction.REMOVAL:
            datamap.pop(key, None)
        else:
            #datamap[key] = getattr(msg, signal_name)
            datamap[key] = msg


