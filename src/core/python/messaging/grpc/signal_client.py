#!/usr/bin/python3 -i
#===============================================================================
## @file signal_client.py
## @brief Client base for gRPC services with a 'Signal' message type
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .client  import Client
from .client_reader import ThreadReader, AsyncReader
from ...protobuf.signal import SignalStore, Slot, cc
from ...protobuf.import_proto import import_proto

from typing import Callable, Sequence, Optional

### Import generated types from `signal.proto`. Symbols will appear within a new
### `cc.signal` namespace.
import_proto('signal', globals())

#===============================================================================
# Client

class SignalClient (Client):
    '''gRPC Signal Client.

    gRPC client with additional functionality to receive streamed Signal
    messages from a gRPC server with a corresponding `watch()` method.

    A sample protobuf service may look something like:

    ```protobuf
    /// @file my_service.proto

    import "signal_types.proto";

    package mycompany.mypackage;

    service MyService
    {
        ...
        rpc watch (cc.signal.Filter) returns (stream MySignal);
    }

    message MySignal
    {
        // Mapping action: one of MAP_ADITION, MAP_UPDATE, MAP_REMOVAL
        cc.signal.MappingAction mapping_action = 1;

        // Mapping key
        string mapping_key = 2;

        oneof signal {
            DataType1 data1 = 8;
            DataType2 data2 = 9;
            ...
        }
    }
    ```

    In your `SignalClient` subclass you would do one of the following:

     * Pass an existing `SignalStore()` instance to `__init__()`:

       ```python
       my_signal_store = protobuf.signal.SignalStore(signal_type = mypackage.MySignal)

       class MyServiceClient (messaging.grpc.SignalClient):
           from my_service_pb2_grpc import MyServiceStub as Stub

           def __init__ (self, ...):
               messaging.grpc.SignalClient.__init__(self, signal_store = my_signal_store)
       ```


     * Alternatively, override the class variable `signal_type` to allow a new
       `SignalStore()` instance to be created for the corresponding ProtoBuf
       message type:

       ```python
       class MyServiceClient (messaging.grpc.SignalClient):
           from my_service_pb2_grpc import MyServiceStub as Stub

           signal_type = mypackage.MySignal
       ```


    Next, you'll need callback handlers to handle the received and re-emitted signals:

    ```python
        def my_data1_simple_handler(data1: mypackage.DataType1):
            """Handle DataType1 signals from server"""
            print("Received data1: ", data1)

        def my_data2_mapping_handler(signal: mypackage.Signal):
            """Handle DataType2 mapping signals from server"""
             print("Received DataType2:",
                   "mapping action:", signal.mapping_action,
                   "mapping key:", signal.mapping_key,
                   "data2:", signal.data2)

    ```

    Now it's time to instantiate your subclass and connect the above callback
    handlers to the appropriate slots in its signal store:

    ```python
        my_client = MyServiceClient()

        my_client.signal_store.connect_signal_data('data1', my_data1_simple_handler)
        my_client.signal_store.connect_signal('data2', my_data2_mapping_handler)

    ```

    Finally, once you have connected handlers for all desired slots, start
    monitoring signals from the server:

    ```python
        client.start_watching()
    ```

    This will invoke the gRPC `watch()` method in a new thread, with a
    Signal.Filter input based on which signal slots were previously connected
    to one or more handlers.

    '''

    ### Subclasses should override this to the appropriate signal type.
    signal_type = None


    #===========================================================================
    # Instance methods

    def __init__(self,
                 host: str,
                 wait_for_ready : bool = False,
                 use_asyncio : bool = False,
                 signal_store : SignalStore = None,
                 watch_all: bool = False,
                 use_cache: bool = True,
                 **kwargs):
        '''Start a new signal client instance.

        @param[in] host
            IP address or resolvable host name of platform server

        @param[in] wait_for_ready
            If a connection attempt fails, keep retrying until successful.
            This value may be overriden per call.

        @param[in] asyncio
            Use Python AsyncIO.  Effectively this performs calls within a
            `grpc.aio.Channel` instance, rather than the default `grpc.Channel`.
            Additionally, the `call()` method uses AsyncIO semantics to capture
            any exceptions.

        @param[in] signal_store
            Use an existing `SignalStore()` instance instead of creating a new
            one.  This can be useful if signals are (received and) emitted from
            both this client and other parts of your code, for instance other
            messaging endpoints.  If not provided, the `signal_type` class
            attribute must be overridden to a suitable ProtoBuf `Signal` type.

        @param[in] watch_all
            Watch all signals (specify an empty filter to server), even if
            not connected to slots. This is useful in order to populate the
            local signal cache, which can later be queried. A side effect
            is that the watching thread automatically starts once instantiated.

        @param[in] use_cache
            Retain the most recent data value of each signal received from the
            server.  If the signal includes a `key` field (i.e., if it is a
            `MappingSignal` instance), keep the most recent data value per key.
            These values can later be queried using `get_cached()`.

        '''

        if not signal_store:
            assert self.signal_type is not None, \
                   ('SignalClient() subclass %s() must either pass in a '
                    '`SignalStore()` instance or set the `signal_type`'
                    'class variable to an appropriate ProtoBuf Signal() type'%
                    (type(self).__name__))

            signal_store = SignalStore(signal_type=self.signal_type, use_cache=use_cache)

        self.signal_store = signal_store

        Client.__init__(self, host,
                        wait_for_ready = wait_for_ready,
                        use_asyncio = use_asyncio,
                        **kwargs)

        self.reader = (ThreadReader, AsyncReader)[self.use_asyncio]()
        if watch_all:
            self.start_watching(True)


    def signal_filter(self, watch_all):
        if watch_all:
            return cc.signal.Filter()
        else:
            indexmap   = self.signal_store.signal_fields()
            indices    = [indexmap.get(slot) for slot in self.signal_store.slots]
            return cc.signal.Filter(polarity=True, index=filter(None, indices))


    def start_notify_signals(self,
                             callback: Slot,
                             signals: Sequence[str]|None = None):
        '''Connect a callback method (slot) to specific signals, or all signals if not specified.
        '''

        if signals:
            for signal in signals:
                self.signal_store.connect_signal(signal, callback)
        else:
            self.signal_store.connect_all(callback)

        self.start_watching()

    def stop_notify_signals(self,
                            callback: Optional[Slot] = None,
                            signals: Sequence[str]|None = None):
        '''Disconnect from the specified signal, or all signals if not specified.'''

        if signals:
            for signal in signals:
                self.signal_store.disconnect_signal(signal, callback)
        else:
            self.signal_store.disconnect_all(callback)


    def start_watching(self, watch_all: bool = True):
        '''Start watching for signals.

        If `watch_all` is `True`, watch all signals, not just those that were
        previously connected to slots.  This is mainly useful if not all
        intended signals are connected yet.

        This spawns a new thread (or task if using AsyncIO) to stream signal
        messages from the service.  Specifically, it invokes the `watch()` RPC
        method to stream back Signal messages, which are then passed on to the
        `messaging.base.SignalStore()` instance that was provided to this
        client.  From there they are emitted locally.

        '''

        if not self.reader.active():
            stream = self.watch(self.signal_filter(watch_all))
            return self.reader.start(stream, self.signal_store.emit)

    def stop_watching(self):
        '''Stop watching signals from server'''

        self.reader.stop()


    def watch(self, signal_filter : cc.signal.Filter = cc.signal.Filter()):
        return self.stub.watch(signal_filter, wait_for_ready=True)
