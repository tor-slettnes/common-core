#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalreceiver.py
## @brief Mix-in base to handle ProtoBuf signal messages
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..google.protobuf import CC, ProtoBuf
from scalar_types      import enums
from safe_invoke       import safe_invoke
from typing            import Optional, Callable

import threading

#===============================================================================
# Enumerated values

MappingChange = enums(CC.Signal.MappingChange, globals())


#===============================================================================
# Annotation types

SimpleSignalHandler = Callable[[ProtoBuf.Message], None]
MappingSignalHandler = Callable[[CC.Signal.MappingChange, str, ProtoBuf.Message], None]


#===============================================================================
# SignalReceiver class

class SignalReceiver (object):
    signal_type = None

    def __init__(self,
                 use_cache          : bool = False,
                 completion_timeout : float = 3.0,
                 signal_type        : ProtoBuf.Message = None):

        if signal_type:
            self.signal_type = signal_type

        self.init_signals(use_cache, completion_timeout)

    def init_signals(self,
                     use_cache: bool,
                     completion_timeout : float):

        assert self.signal_type,\
            ("%s instance must define `signal_type`, either by "
             "declaring a static member or passing it as argument "
             "to SignalSubscriber.__init__()"%(type(self).__name__,))

        self._slots               = {}
        self._completion_event    = threading.Event()
        self._completion_mutex    = threading.Lock()
        self._completion_timeout  = completion_timeout
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
        return self.descriptor().fields_by_number[fieldnumber].name

    def field_number(self, fieldname: str) -> int:
        '''
        Return the field number associated with the specified signal name.

        @return
            Number associated with the field name of the Signal message.

        @exception KeyError
            The speicfied field does not exist.
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


    def get_cached(self,
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


    def connect_signal(self,
                        name: str,
                        slot: SimpleSignalHandler):
        '''Connect a callback handler (slot) to a specific simple signal.

        @param[in] name
            Signal name, corresponding to a field of the Signal message.

        @param[in] slot
            A callable handler (e.g. a function) that accepts the signal payload
            as its first and only required argument.
        '''

        self.connect_mapping_signal(
            name,
            lambda mapping_type, mapping_key, data: slot(data))


    def disconnect_signal(self,
                          name: str,
                          slot: Optional[SimpleSignalHandler] = None):
        '''Disconnect a simple handler from the specified signal.

        @param[in] name
           Signal name

        @param[in] slot
           Signal handler. If not provided, remove all handlers from the
           specified signal slot.

        Returns True if the handler was found and removed; False otherwise.
        '''

        args = [name]
        if slot:
            args.append(lambda mapping_type, mapping_key, data: slot(data))
        self.disconnect_mapping_signal(*args)


    def connect_mapping_signal(self,
                               name: str,
                               slot: MappingSignalHandler):
        '''Connect a callback handler to a specific mapping signal.

        @param[in] name
            Signal name, corresponding to a field of the received Signal message.

        @param[in] slot
            A callable handler (e.g. a function) that accepts exactly three arguments:
             * The mapping type (MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL)
             * The mapping key
             * The signal data
        '''

        assert callable(slot), \
            "Slot must be a callable object, like a function"

        assert name in self.signal_names(), \
            "Message type %s does not have a %r field"%(self.signal_type.__name__, name)

        self._slots.setdefault(name, []).append(slot)


    def disconnect_mapping_signal(self,
                                  name: str,
                                  slot: Optional[MappingSignalHandler] = None):
        '''Disconnect a mapping signal handler from the specified signal .

        @param[in] name
           Signal name

        @param[in] slot
           Signal handler. If not provided, remove all handlers from the
           specified signal slot.

        Returns True if the handler was found and removed; False otherwise.
        '''

        if slot:
            try:
                slots = self._slots[name]
                slots.remove(slot)
            except (KeyError, ValueError):
                return False
            except ValueError:
                if not slots:
                    self._slots.pop(name, None)
                return True
        else:
            return bool(self._slots.pop(name, None))


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


    def signal_filter(self, watch_all):
        if watch_all:
            return CC.Signal.Filter()
        else:
            indexmap   = self.signal_fields()
            indices    = [indexmap.get(slot, 0) for slot in self._slots]
            return CC.Signal.Filter(polarity=True, index=filter(None, indices))


    def process_signal_message(self, msg):
        try:
            change = MappingChange[msg.change]
        except IndexError:
            change = msg.change

        name, data = self._signal_item(msg)

        if name:
            if isinstance(self._cache, dict):
                self._update_cache(name, change, msg.key, data)

            for callback in self._slots.get(name, []):
                safe_invoke(callback,
                            (change, msg.key, data),
                            {},
                            "Signal %s slot %s()"%(name, callback.__name__))

        if change == CC.Signal.MAP_NONE:
            self._completion_event.set()

    def _signal_item(self, msg):
        selector = msg.DESCRIPTOR.oneofs[0].name
        name = msg.WhichOneof(selector) or ""
        data = getattr(msg, name, None)
        return (name, data)

    def _update_cache(self, signalname, change, key, data):
        datamap = self._cache.setdefault(signalname, {})
        if change in (CC.signal.MAP_ADDITION, CC.signal.MAP_UPDATE):
            datamap[key] = data
        elif change == CC.signal.MAP_REMOVAL:
            datamap.pop(key, None)
