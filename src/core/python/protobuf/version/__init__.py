'''
Package `cc.protobuf.version` - Version structure container
'''

### Generated from `.../protos/cc/protobuf/version/version.proto`
from .version_pb2 import Version, ComponentVersions

from .version import (
    create_version,
    version_to_string, version_from_string,
    is_compatible_version,
)
