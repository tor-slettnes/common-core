'''
generic_wrapper.py - Convencience class for creating a standard/generic gRPC server
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"

### Third-party modules
import grpc

### Common Core modules
from cc.core.decorators import override

### Modules within package
from .base_wrapper import ServerWrapperBase

class ServerWrapper (ServerWrapperBase):
    '''
    Convencience class for creating a standard/generic gRPC server.
    '''

    @override
    def create_server(self, *args, **kwargs) -> grpc.Server:
        return grpc.server(*args, **kwargs)
