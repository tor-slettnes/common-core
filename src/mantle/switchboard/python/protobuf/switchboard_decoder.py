'''
switchoard_decoder.py: Specialized MessageDissecter with decoders for Switchboard types
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

from cc.protobuf.wellknown import Message
from cc.protobuf.dissecter import MessageDissecter

class SwitchboardDecoder (MessageDissecter):
    '''
    Specialized MessageDissecter with decoders for Switchboard types
    '''

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.register_decoder(
            'cc.platform.switchboard.SpecificationMap',
            self.decode_map)

        self.register_decoder(
            'cc.platform.switchboard.LocalizationMap',
            self.decode_map)

        self.register_decoder(
            'cc.platform.switchboard.DependencyMap',
            self.decode_map)

        self.register_decoder(
            'cc.platform.switchboard.DependencyStatusMap',
            self.decode_map)

        self.register_decoder(
            'cc.platform.switchboard.InterceptorMap',
            self.decode_map)

        self.register_decoder(
            'cc.platform.switchboard.StatusMap',
            self.decode_map)

        self.register_decoder(
            'cc.platform.switchboard.ErrorMap',
            self.decode_map)

        self.register_decoder(
            'cc.platform.switchboard.SwitchMap',
            self.decode_map)


    def decode_map(self, msg: Message) -> object:
        return self.decode(msg.map)


switchboard_decoder = SwitchboardDecoder()
