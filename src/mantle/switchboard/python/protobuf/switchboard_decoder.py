'''
switchoard_decoder.py: Specialized MessageDissecter with decoders for Switchboard types
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

from cc.protobuf.wellknown import Message
from cc.protobuf.dissecter import MessageDissecter
from cc.platform.switchboard.protobuf import (
    SpecificationMap, LocalizationMap, DependencyMap, DependencyStatusMap,
    InterceptorMap, StatusMap, ErrorMap, SwitchMap,
)

class SwitchboardDecoder (MessageDissecter):
    '''
    Specialized MessageDissecter with decoders for Switchboard types
    '''

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.register_decoder(SpecificationMap, self.decode_map)
        self.register_decoder(LocalizationMap, self.decode_map)
        self.register_decoder(DependencyMap, self.decode_map)
        self.register_decoder(DependencyStatusMap, self.decode_map)
        self.register_decoder(InterceptorMap, self.decode_map)
        self.register_decoder(StatusMap, self.decode_map)
        self.register_decoder(ErrorMap, self.decode_map)
        self.register_decoder(SwitchMap, self.decode_map)


    def decode_map(self, msg: Message) -> object:
        return self.decode(msg.map)


switchboard_decoder = SwitchboardDecoder()
