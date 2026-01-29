'''
Mix-in class to receive passive updates from `Upgrade` gRPC service
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Common Core modules
from cc.messaging.grpc.client import SignalClient as SignalClientBase

### Upgrade modules
from ..protobuf import Signal, ScanProgress, PackageInfo, UpgradeProgress

class SignalMixIn (SignalClientBase):
    '''
    Mix-in class to add signal listener to `Upgrade` gRPC service clients.

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
    def scan_progress(self) -> ScanProgress:
        return self.signal_store.get_cached_signal(
            'scan_progress',
            fallback = ScanProgress,
        )

    @property
    def upgrade_available(self) -> PackageInfo:
        return self.signal_store.get_cached_signal(
            'upgrade_available',
            fallback = PackageInfo,
        )

    @property
    def upgrade_pending(self) -> PackageInfo:
        return self.signal_store.get_cached_signal(
            'upgrade_pending',
            fallback = PackageInfo,
        )

    @property
    def upgrade_progress(self) -> UpgradeProgress:
        return self.signal_store.get_cached_signal(
            'upgrade_progress',
            fallback = UpgradeProgress,
        )
