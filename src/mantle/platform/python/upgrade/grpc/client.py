'''
Python client for `Upgrade` gRPC service
'''

__all__ = ['Client']
__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Modules within package
from cc.protobuf.wellknown import empty
from cc.protobuf.upgrade import Signal, \
    PackageSource, SourceType, encodeSource, decodeSource, \
    PackageCatalogue, PackageInfo, \
    InstallRequest
from cc.messaging.grpc import SignalClient

### Standard Python modules
from typing import Mapping, Optional
import io

#===============================================================================
## Client

class Client (SignalClient):
    '''
    Client for Upgrade service.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.upgrade_pb2_grpc import UpgradeStub as Stub

    ## `signal_type` is used to construct a `cc.protobuf.SignalStore` instance,
    ## which serves as a clearing house for emitting and receiving messages.
    signal_type = Signal

    Signals = (SIGNAL_SCAN_PROGRESS, SIGNAL_UPGRADE_AVAILABLE,
               SIGNAL_UPGRADE_PENDING, SIGNAL_UPGRADE_PROGRESS) \
        = ('scan_progress', 'upgrade_available',
           'upgrade_pending', 'upgrade_progress')


    def scan(self, source: Optional[SourceType] = None):
        '''
        Explicit scan for available upgrade packages in the specified package
        source if provided, otherwise in the preconfigured/default locations.

        @param source:
            Either a HTTP/HTTPS URL or a VFS path specified in the format
            `CONTEXT:PATH`. If no source is specified, perform scan in the
            preconfigured/default locations.

        This call returns immediately, without waiting for the results of the
        scan. To observe scan progress and results, see `start_notify_signals()`.

        This call is not required for ongoing upgrade availability
        notifications.  By default, removable devices (e.g. USB drives) are
        scanned on insertion, and online checks are performed at regular
        intervals if an Internet connection is available.
        '''

        return self.stub.scan(encodeSource(source))


    def list_sources(self) -> list[PackageSource]:
        '''
        List available package sources, whether or not they contain
        applicable packages.
        '''
        return self.stub.list_sources()

    def list_available(self,
                        source: Optional[SourceType] = None) \
                        -> list[PackageInfo]:
        '''
        Obtain information about available upgrade packages discovered
        during a prior (implicit or explicit) scan.

        @param source:
            Restrict listing to packages discovered from the specified source.

        @returns
            Information about each package discovered.
        '''

        return self.stub.list_available(encodeSource(source)).packages

    def best_available(self,
                        source: Optional[SourceType] = None) \
                        -> PackageInfo:
        '''
        Return information about the best available upgrade package discovered
        during a prior scan of the specified package source if specified,
        otherwise the best overall candidate discovered across all
        preconfigured/default locations. This will normally be the package with
        the highest version number, with local (VFS) sources preferered over
        remote (URL).
        '''

        return self.stub.best_available(encodeSource(source))


    def install(self,
                source_file: Optional[SourceType] = None,
                force: bool = False) \
                -> PackageInfo:
        '''
        Install an upgrade from the specified source file if provided,
        otherwise the current "best" package source based on prior scans.  To
        perform an explicit scan, invoke `scan()` before `install()`.

        @param source_file:
          An optional HTTP/HTTPS URL or a VFS path to the specified in the
          format `CONTEXT:PATH`, pointing to the exact release package to
          install.

        @param force:
          whether to install the package even if its package_info declares a
          different product name than what is currently installed or its version
          number is older than the currently installed release.

        This call returns immediately with information about the package being
        installed. To monitor the upgrade progress and result use the `watch()`
        method below.
        '''

        request = InstallRequest(
            source = encodeSource(source_file, is_file=True),
            force = force)

        print("Install request: %s"%(request,))

        return self.stub.install(request)


    def finalize (self):
        '''
        Finalize a completed upgrade.  This clears the `upgrade_progress`
        signal, and if the upgrade requires a system reboot, do so now.
        '''

        return self.stub.finalize(empty)



if __name__ == '__main__':
    upgrade = Client()
