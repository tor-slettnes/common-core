#!/usr/bin/python3 -i
#===============================================================================
## @file grpc_signalclient.py
## @brief Client base for services with a 'Signal' message type
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import threading, time

from typing         import Optional, Callable, Union
from grpc_client    import BaseClient, ArgParser, ProtoBuf, CC
from scalar_types   import enums
from safe_invoke    import safe_invoke


#===============================================================================
# Enumerated values

MappingChange = enums(CC.Signal.MappingChange)


#===============================================================================
# Client

class SignalClient (BaseClient):
    '''gRPC Signal Client.

    Derived from `BaseClient`, but with additional methods to handle Signal
    messages streamed from various services with a corresponding `watch()`
    method.

    A sample protobuf service may look something like:

    ```protobuf

    import "signal_types.proto";

    package CC.mypackage;

    service MyService
    {
        ...
        rpc watch (CC.Signal.Filter) returns (stream MySignal);
    }

    message MySignal
    {
        oneof signal {
            DataType1 data1 = 8;
            DataType2 data2 = 9;
            ...
        }
    }
    ```

    To receive such signals from the server, you will normally create
    a callback function of the form:

    ```
        def my_signal_handler(signal: DataType1):
           """Handle DataType1 signals from server"""
    ```

    Then, associate this method with a signal slot.

    ```
        client = SignalClient(host, CC.mypackage.MySignal, ...)
        client.connect_signal("data1", my_callback_handler, "data1")
    ```

    Finally, once you have connected handlers for all desired slots,
    start monitoring signals from the server:

    ```
        client.startWatching()
    ```

    This will invoke the gRPC `watch()` method in a new thread, with a
    Signal.Filter input based on which signal slots were previously connected
    to one or more handlers.
    '''

    SignalType = None

    #===========================================================================
    # Instance methods

    def __init__(self,
                 host: str,
                 SignalType: ProtoBuf.Message = None,
                 watch_all: bool = False,
                 use_cache: bool = False,
                 completion_timeout: float = 3.0,
                 **kwargs):
        '''Start a new signal client instance.

        @param[in] host
            IP address or resolvable host name of platform server

        @param[in] watch_all
            Watch all signals (specify an empty filter to server), even if
            not connected to slots. This is useful in order to populate the
            local signal cache, which can later be queried. A side effect
            is that the watching thread automatically starts once instantiated.

        @param[in] use_cache
            Retain the most recent data value of each signal received from the
            server.  If the signal includes a `key` field (i.e., if it is a
            `MappedSignal` instance), keep the most recent data value per key.
            These values can later be queried using `get_cached()`.

        @param[in] completion_timeout
            How long to allow for the server's signal cache to be received,
            once watching.  Any queries may block until this deadline has expired.

        '''
        BaseClient.__init__(self, host, **kwargs)

        if SignalType:
            self.SignalType = SignalType

        assert self.SignalType,\
            ("%s instance did not define SignalType, either by "
             "declaring a static member or passing it as argument "
             "to base SignalClient()"%(type(self).__name__,))

        self._slots               = {}
        self._watcherThread       = None
        self._completion_event    = threading.Event()
        self._completion_mutex    = threading.Lock()
        self._completion_timeout  = completion_timeout
        self._completion_deadline = None
        self._deadline_expired    = False

        if use_cache:
            self._cache = {}
        else:
            self._cache = None

        if watch_all:
            self.start_watching(True)


    #===========================================================================
    # Class methods

    def descriptor (self):
        return self.SignalType.DESCRIPTOR

    def field_name (self, fieldnumber: int) -> str:
        '''
        Obtain the signal name associated with the specified field number.

        @return
            Number associated with the field name of the Signal message.

        @exception KeyError
            The specified field number does not exist
        '''
        return self.descriptor().fields_by_number[fieldnumber].name

    def field_number (self, fieldname: str) -> int:
        '''
        Return the field number associated with the specified signal name.

        @return
            Number associated with the field name of the Signal message.

        @exception KeyError
            The speicfied field does not exist.
        '''

        return self.descriptor().fields_by_name[signalname].number

    def signal_fields (self) -> dict:
        '''Return a dictionary of signal names to corresponding field numbers
        within the protobuf Signal message'''

        items = [(f.name, f.number)
                 for f in self.descriptor().oneofs[0].fields]

        return dict(items)

    def signal_names (self) -> list:
        '''
        Return a list of signal/slot names in the protobuf Signal message
        for this service.
        '''
        return [f.name for f in self.descriptor().oneofs[0].fields]


    def get_cached (self,
                    signalname: str,
                    timeout: float=3) -> ProtoBuf.Message:
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
            return self._cache.get(signalname)

        else:
            return self._cache[signalname]


    def connect_signal (self,
                        name: str,
                        handler: Optional[Callable],
                        prepend: Union[tuple, object]=(),
                        append: Union[tuple, object]=(),
                        kwargs: dict={}):
        '''Connect a callback handler to a specific signal slot.

        @param[in] name
            Slot name, corresponding to a field of the Signal message
            streamed from the server's `watch()` method.

        @param[in] handler
            A callable handler (e.g. a function) that accepts the signal payload
            as its first argument.

        @param[in] prepend
            Additional argument(s) to be prepended before the payload when
            invoking the callback handler. This may be e.g. the name or the
            signal or a unique reference.

        @param[in] append
            Additional argument(s) to be appended when invoking the callback
            handler.

        @param[in] kwargs
            Additional named arguments to be provided to the callback handler.

        @note
            The callbacks occur in a dedicated watcher thread, spawned
            by a subsequent `start_watching()` invocation.

        '''
        assert callable(handler), \
            "Handler must be a callable object, like a function"

        assert name in self.signal_names(), \
            "Message type %s does not have a %r field"%(self.SignalType.__name__, name)

        if not isinstance(prepend, tuple):
            prepend = (prepend,)

        if not isinstance(append, tuple):
            append = (append,)

        self._slots.setdefault(name, []).append((handler, prepend, append, kwargs))


    def disconnect_signal (self, name: str, handler: Optional[Callable]=None):
        '''Disconnect a signal handler.

        @param[in] name
           Signal slot name

        @param[in] handler
           Signal handler. If not provided, remove all handlers from the
           specified signal slot.

        Returns True if the handler was found and removed; False otherwise.
        '''

        if handler:
            try:
                slots = self._slots[name]
            except KeyError:
                return False
            else:
                for (idx, slot) in enumerate(slots):
                    if slot[0] == handler:
                        del slots[idx]
                        if not slots:
                            self._slots.pop(name, None)
                        return True
                else:
                    return False
        else:
            return bool(self._slots.pop(name, None))

    def start_watching (self, watch_all: bool = True):
        '''
        Start watching for signals. This spawns a new thread to stream signal
        messages from the service. Specifically, it invokes the `watch()` RPC
        method with a `CC.Signal.Filter` input based on which signal slots were
        previosly connected to one or more handlers.
        '''

        if not self._watcherThread:
            self._completion_deadline = time.time() + self._completion_timeout

            t = threading.Thread(None, self._watcher, 'Watcher thread', (watch_all,))
            t.setDaemon(True)
            self._watcherThread = t

            t.start()

        return self._watcherThread

    def stop_watching(self):
        '''Stop watching change notifications from server'''

        self._watcherThread = None

    def wait_complete (self) -> bool:
        '''
        Wait for all currently mapped signals to be received from the server.
        May be invoked after `start_watching()` to ensure the local cache is
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


    def _watcher(self, watch_all):
        try:
            sf = self._signal_filter(watch_all)
            for msg in self._wrap(
                    self.stub.watch, sf, wait_for_ready = True):
                self._on_signal(msg)
                if not self._watcherThread:
                    break
        finally:
            self._watcherThread = None

    def _signal_filter (self, watch_all):
        if watch_all:
            return CC.Signal.Filter()
        else:
            indexmap   = self.signal_fields()
            indices    = [indexmap.get(slot, 0) for slot in self._slots]
            return CC.Signal.Filter(polarity=True, index=filter(None, indices))

    def _on_signal (self, msg):
        signalname, data = self._signal_item(msg)
        if signalname:
            if isinstance(self._cache, dict):
                self._cache[signalname] = data

            for callback, prepend, append, kwargs in self._slots.get(signalname, []):
                safe_invoke(callback,
                            prepend+(data,)+append,
                            kwargs,
                            "Signal %s slot %s()"%(signalname, callback.__name__))
        else:
            self._completion_event.set()

    def _signal_item (self, msg):
        selector = msg.DESCRIPTOR.oneofs[0].name
        slot = msg.WhichOneof(selector) or ""
        data = getattr(msg, slot, None)
        return (slot, data)


class MappedSignalClient (SignalClient):
    '''gRPC MappedSignal Client.

    Derived from @sa SignalClient, but intended for protobuf Signal types
    modelled after the C++ MappedSignal class, with a `change` and `key`
    attribute. A typical message may look something like:

    ```protobuf

    import "signal_types.proto";

    message MySignal
    {
        signal.MappingChange change = 1;
        string key = 2;

        oneof signal {
            DataType1 data1 = 8;
            DataType2 data2 = 9;
            ...
        }
    }
    ```
    '''

    def connect_signal (self,
                        name: str,
                        handler: Optional[Callable],
                        prepend: Union[tuple, object]=(),
                        append: Union[tuple, object]=(),
                        kwargs: dict={}):

        '''Connect a callback handler to a specific signal slot.

        @param[in] name
            Slot name, corresponding to a field of the Signal message
            streamed from the server's `watch()` method.

        @param[in] handler
            A callable handler (e.g. a function) that accepts as arguments:
              - The mapping change (MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL)
              - The mapping key
              - The signal payload.

        @param[in] prepend
            Additional argument(s) to be prepended before the above three
            when invoking the callback handler. This may be e.g. the name or the
            signal or a unique reference.

        @param[in] append
            Additional argument(s) to be appended when invoking the callback
            handler.

        @param[in] args
            Additional arguments to be prepended before the above three
            when invoking the callback handler. This may be e.g. the name
            or the signal or a unique reference.

        @param[in] kwargs
            Additional named arguments to be provided to the callback handler.

        @note
            The callbacks occur in a dedicated watcher thread, spawned
            by a subsequent `start_watching()` invocation.

        '''
        return super()(name, handler, prepend, append, kwargs)


    def _on_signal (self, msg):
        signalname, data = self._signal_item(msg)
        try:
            change = CC.Signal.MappingChange[msg.change]
        except IndexError:
            change = msg.change

        if signalname:
            self._on_mapped_update(signalname, change, msg.key, data)

            for callback, prepend, append, kwargs in self._slots.get(signalname, []):
                safe_invoke(callback,
                            prepend+(change, msg.key, data)+append,
                            kwargs,
                            "Signal %s slot %s()"%(signalname, callback.__name__))
        else:
            # A mapped signal with no change indicates that the map is complete.
            self._completion_event.set()


    def _on_mapped_update (self, signalname, change, key, data):
        datamap = self._cache.setdefault(signalname, {})
        if change in (CC.signal.MAP_ADDITION, CC.signal.MAP_UPDATE):
            datamap[key] = data
        elif change == CC.signal.MAP_REMOVAL:
            datamap.pop(key, None)

