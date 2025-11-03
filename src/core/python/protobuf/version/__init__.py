'''
Package `cc.protobuf.version` - Structure version container
'''

from .version_pb2 import Version, ComponentVersions
from .version import version, to_string, from_string, is_compatible
