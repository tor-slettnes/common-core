'''
Package cc.protobuf.dissecter - Build ProtoBuf messages from native Python values.
'''

from .dissecter import (
    MessageDissecter, dissecter,
    decode_message, decode_to_dict, decode_response,
)
