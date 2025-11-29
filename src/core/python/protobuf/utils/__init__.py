'''
Package `cc.protobuf.utils` - miscellaneous ProtoBuf utility methods
'''

from .enum import proto_enum
from .message import check_message_type, \
    messageToDict, dictToMessage, \
    print_message, print_dict
