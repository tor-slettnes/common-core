'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC
'''

__all__ = ['Client']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

from cc.core.decorators import doc_inherit
from cc.protobuf.variant import PyValueList, encodeValueList
from cc.messaging.grpc import SignalClient
from ..protobuf import AddSwitchRequest, RemoveSwitchRequest, ImportRequest
from ..base.baseboard import SwitchboardBase
from .remote_switch import RemoteSwitch

class Client (SwitchboardBase, SignalClient):
    '''
    Switchboard abstract base
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `cc.messaging.grpc.GenericClient` base to instantiate `self.stub`.
    from .switchboard_service_pb2_grpc import SwitchboardStub as Stub

    def __init__(self,
                 host: str = "",
                 wait_for_ready: bool = True,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 ):
        '''
        @param host
            IP address or resolvable host name of platform server

        @param product_name
            Name of the product, used to locate corresponding settings files
            (e.g. `grpc-endpoints-PRODUCT.yaml`).

        @param project_name
            Name of code project (e.g. parent code repository). Used to locate
            corresponding settings files (e.g., `grpc-endpoints-PROJECT.yaml`)
        '''

        SignalClient.__init__(self,
                              host = host,
                              wait_for_ready = wait_for_ready,
                              product_name = product_name,
                              project_name = project_name)

        SwitchboardBase.__init__(self)
        self.start_watching(watch_all=True)

    def _new_switch(self,
                    switch_name: str,
                    ) -> RemoteSwitch:
        '''Overridden method to obtain a local Switch instance in response to update signals from server'''
        return RemoteSwitch(switch_name, self.stub)

    @doc_inherit
    def add_switch(self,
                   switch_name: str,
                   ) -> bool:
        req = AddSwitchRequest(switch_name = switch_name)
        return self.stub.AddSwitch(req).value

    @doc_inherit
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> bool:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return self.stub.RemoveSwitch(req).value

    @doc_inherit
    def import_switches(self,
                        declarations: PyValueList) -> int:
        req = ImportRequest(
            declarations = encodeValueList(declarations))
        return self.stub.ImportSwitches(req).import_count


