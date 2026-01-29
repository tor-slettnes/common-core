'''
Mix-in class to add signal listener to VirtualFileSystem gRPC service
clients.
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'


### Standard Python modules
from typing import Mapping

### Common Core modules
from cc.messaging.grpc.client import SignalClient as SignalClientBase

### VFS modules
from ..protobuf import (
    Signal, ContextSpec,
)

#===============================================================================
# SignalMixIn

class SignalMixIn (SignalClientBase):
    '''
    Mix-in class to add signal listener to VirtualFileSystem gRPC service
    clients.

    To subscribe to one or more of these signals, connect your own callback
    handler using one of the provided `connect_*()` methods.  Additionally,
    property methods are provided to obtain recevied information from the local
    signal cache.
    '''

    ## `signal_type` is used to construct a `cc.protobuf.signal.SignalStore`
    ## instance, which serves as a clearing house for emitting and receiving
    ## messages.
    signal_type = Signal

    @property
    def contexts(self) -> Mapping[str, ContextSpec]:
        return self.signal_store.get_cached_map('context')

    @property
    def open_contexts(self) -> Mapping[str, ContextSpec]:
        return self.signal_store.get_cached_map('context_in_use')

