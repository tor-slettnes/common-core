#!/usr/bin/echo Do not invoke directly.
##
##===============================================================================n
## @file grpc_server.py
## @brief Interactive wrapper for access to Instrument Services via gRPC
## @author Tor Slettnes <tor@slett.net>
##
#===============================================================================

from concurrent import futures
import grpc
import grpc_base

class BaseServer (grpc_base.ServiceBase):
    ## Derived classes must define 'add_SERVICENAME_to_server' here, e.g.
    #from SERVICENMODULE import add_SERVICENAME_to_server

    def __init__ (self, address="", servicename=None):
        ServiceBase.__init__ (self, servicename or type(self).__name__)
        self.address = self._realaddress(address, "interface", "port", "[::]", 8080)

    def add_to_server (self, server):
        methodname = 'add_%sServicer_to_server'%(self.servicename,)
        try:
            add = getattr(type(self), methodname)
        except AttributeError as e:
            raise AttributeError("Class %r does not contain symbol %r. \n"
                                 "Please add the following line in it:\n"
                                 ">>> from MODULENAME_pb2_grpc import %s" %
                                 (type(self).__name__, methodname, methodname))

        add(self, server)
        server.add_insecure_port(self.address)


def server (*servicers):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    for servicer in servicers:
        servicer.add_to_server(server)
    return server
