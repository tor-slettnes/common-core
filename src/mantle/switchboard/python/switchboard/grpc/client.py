'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC
'''

__all__ = ['Client']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Modules within package
from .remote_switch import RemoteSwitch
from ..base.baseboard import SwitchboardBase
from cc.protobuf.switchboard import AddSwitchRequest, RemoveSwitchRequest
from cc.messaging.grpc.signal_client import SignalClient
from cc.core.doc_inherit import doc_inherit

class Client (SwitchboardBase, SignalClient):
    '''
    Switchboard abstract base
    '''

    from cc.generated.switchboard_pb2_grpc import SwitchboardStub as Stub

    def __init__(self,
                 host: str = "",
                 wait_for_ready: bool = True):
        '''
        @param host:
            IP address or resolvable host name of platform server
        '''

        SwitchboardBase.__init__(self)
        SignalClient.__init__(self, host = host, wait_for_ready = wait_for_ready)
        self.start_watching(watch_all=True)

    def _new_switch(self, switch_name: str) -> RemoteSwitch:
        '''Overridden method to obtain a local Switch instance in response to update signals from server'''
        return RemoteSwitch(switch_name, self.stub)

    @doc_inherit
    def add_switch(self, switch_name: str) -> bool:
        req = AddSwitchRequest(switch_name = switch_name)
        return self.stub.add_switch(req).value

    @doc_inherit
    def remove_switch(self, switch_name: str, propagate: bool = True) -> bool:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return self.stub.remove_switch(req).value
