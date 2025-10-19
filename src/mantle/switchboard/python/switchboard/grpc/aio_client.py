'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC.  This flavor implements Python AsyncIO semantics.
'''

__all__ = ['Client']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Modules within package
from .aio_remote_switch import AsyncRemoteSwitch
from ..base.baseboard import SwitchboardBase
from cc.protobuf.switchboard import AddSwitchRequest, RemoveSwitchRequest
from cc.messaging.grpc.signal_client import SignalClient
from cc.core.doc_inherit import doc_inherit

class AsyncClient (SwitchboardBase, SignalClient):
    '''
    Switchboard abstract base
    '''

    from cc.protobuf.switchboard.switchboard_pb2_grpc import SwitchboardStub as Stub

    def __init__(self,
                 host: str = "",
                 wait_for_ready: bool = True,
                 watch_all: bool = True):
        '''
        @param host:
            IP address or resolvable host name of platform server
        '''

        SwitchboardBase.__init__(self)
        SignalClient.__init__(self,
                              host = host,
                              use_asyncio = True,
                              wait_for_ready = wait_for_ready,
                              watch_all = watch_all)

    def _new_switch(self, switch_name: str) -> AsyncRemoteSwitch:
        '''Overridden method to obtain a local Switch instance in response to update signals from server'''
        return AsyncRemoteSwitch(switch_name, self.stub)

    @doc_inherit
    async def add_switch(self, switch_name: str) -> bool:
        req = AddSwitchRequest(switch_name = switch_name)
        return (await self.stub.add_switch(req)).value

    @doc_inherit
    async def remove_switch(self, switch_name: str, propagate: bool = True) -> bool:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return (await self.stub.remove_switch(req)).value


