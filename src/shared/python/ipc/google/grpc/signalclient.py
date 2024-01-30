#!/usr/bin/python3 -i
#===============================================================================
## @file signalclient.py
## @brief Client base for gRPC services with a 'Signal' message type
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from safe_invoke              import safe_invoke
from typing                   import Optional, Callable, Union
from .client                  import Client, ArgParser, ProtoBuf, CC
from ...common.signalreceiver import SignalReceiver, MappingChange, \
    SimpleSignalHandler, MappingSignalHandler

import threading, time


#===============================================================================
# Client

class SignalClient (Client, SignalReceiver):
    '''gRPC Signal Client.

    Derived from `Client` and 'SignalReceiver', with additional functionality to
    receive streamed Signal messages from a gRPC server with a corresponding
    `watch()` method.

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
        // Mapping change: one of MAP_ADITION, MAP_UPDATE, MAP_REMOVAL
        CC.Signal.MappingChange change = 1;

        // Mapping key
        string key = 2;

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
        def my_signal_handler(change: CC.Signal.MappingChange,
                              key   : str,
                              data  : DataType1):
           """Handle DataType1 signals from server"""
    ```

    Then, associate this method with a signal slot.

    ```
        client = SignalClient(host, signal_type = CC.mypackage.MySignal, ...)
        client.connect_signal('data1', my_signal_handler)
    ```

    Finally, once you have connected handlers for all desired slots,
    start monitoring signals from the server:

    ```
        client.start_watching()
    ```

    This will invoke the gRPC `watch()` method in a new thread, with a
    Signal.Filter input based on which signal slots were previously connected
    to one or more handlers.

    '''


    #===========================================================================
    # Instance methods

    def __init__(self,
                 host: str,
                 signal_type: ProtoBuf.Message = None,
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
        Client.__init__(self, host, **kwargs)
        SignalReceiver.__init__(self,
                                use_cache = use_cache,
                                completion_timeout = completion_timeout,
                                signal_type = signal_type)

        self._watcherThread = None
        if watch_all:
            self.start_watching(True)



    def start_watching(self, watch_all: bool = True):
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


    def _watcher(self, watch_all):
        try:
            sf = self.signal_filter(watch_all)
            for msg in self._wrap(
                    self.stub.watch, sf, wait_for_ready = True):
                self.process_signal_message(msg)
                if not self._watcherThread:
                    break
        finally:
            self._watcherThread = None


