'''
Python base client for `Upgrade` gRPC service
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

## Common Core modules
from cc.protobuf.wellknown import empty
from cc.messaging.grpc.client import GenericClient

## Upgrade modules
from ..protobuf import (
    PackageSources,  PackageCatalogue, PackageInfo, InstallRequest,
    SourceType, encodeSource, decodeSource,
)

#===============================================================================
## BaseClient

class BaseClient (GenericClient):
    '''
    Client for Upgrade service.

    Methods in this module are simple wrappers around corresponding gRPC calls,
    whose responses are returned unmodified. Depending on the gRPC channel type,
    this may be an `asyncio` coroutine that must be awaited.

    Likely you will not use this module directly, but rather one of the derived
    classes `Client`, `SignalClient`, `AsyncClient`, or `AsyncSignalClient`.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `cc.messaging.grpc.GenericClient` base to instantiate `self.stub`.
    from .upgrade_service_pb2_grpc import UpgradeStub as Stub


    def scan(self, source: SourceType|None = None) -> PackageCatalogue:
        '''
        Explicit scan for available upgrade packages in the specified package
        source if provided, otherwise in the preconfigured/default locations.

        @param source
            Either a HTTP/HTTPS URL or a VFS path specified in the format
            `CONTEXT:PATH`. If no source is specified, perform scan in the
            preconfigured/default locations.

        This call is not required for ongoing upgrade availability
        notifications.  By default, removable devices (e.g. USB drives) are
        scanned on insertion, and online checks are performed at regular
        intervals if an Internet connection is available.
        '''

        return self.stub.Scan(encodeSource(source))


    def list_sources(self) -> PackageSources:
        '''
        List available package sources, whether or not they contain
        applicable packages.
        '''
        return self.stub.ListSources(empty)


    def list_available(self,
                       source: SourceType|None = None
                       ) -> PackageCatalogue:
        '''
        Obtain information about available upgrade packages discovered
        during a prior (implicit or explicit) scan.

        @param source
            Restrict listing to packages discovered from the specified source.

        @returns
            Information about each package discovered.
        '''

        return self.stub.ListAvailable(encodeSource(source))

    def best_available(self,
                        source: SourceType|None = None,
                       ) -> PackageInfo:
        '''
        Return information about the best available upgrade package discovered
        during a prior scan of the specified package source if specified,
        otherwise the best overall candidate discovered across all
        preconfigured/default locations. This will normally be the package with
        the highest version number, with local (VFS) sources preferered over
        remote (URL).
        '''

        return self.stub.BestAvailable(encodeSource(source))


    def install(self,
                source_file: SourceType|None = None,
                force: bool = False,
                ) -> PackageInfo:
        '''
        Install an upgrade from the specified source file if provided,
        otherwise the current "best" package source based on prior scans.  To
        perform an explicit scan, invoke `scan()` before `install()`.

        @param source_file
          An optional HTTP/HTTPS URL or a VFS path to the specified in the
          format `CONTEXT:PATH`, pointing to the exact release package to
          install.

        @param force
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

        return self.stub.Install(request)


    def finalize (self):
        '''
        Finalize a completed upgrade.  This clears the `upgrade_progress`
        signal, and if the upgrade requires a system reboot, do so now.
        '''

        return self.stub.Finalize(empty)
