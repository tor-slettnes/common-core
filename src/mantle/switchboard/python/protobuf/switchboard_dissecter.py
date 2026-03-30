'''
switchoard_decoder.py: Specialized MessageDissecter with decoders for Switchboard types
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

from cc.protobuf.wellknown import Message
from cc.protobuf.dissecter import MessageDissecter
from cc.platform.switchboard.protobuf import (
    SwitchIdentifiers,
    SpecificationMap, LocalizationMap, DependencyMap, DependencyStatusMap,
    InterceptorMap, StatusMap, ErrorMap, SwitchMap,
    SetTargetResponse, SetAttributesResponse, GetAttributesResponse,
    ImportResponse, ExportResponse,
)

class SwitchboardDissecter (MessageDissecter):
    '''
    Specialized MessageDissecter with decoders for Switchboard ProtoBuf types
    '''

    def register_decoders(self):
        super().register_decoders()
        self.register_decoder(SwitchIdentifiers, self._decode_switch_identifiers)
        self.register_decoder(SpecificationMap, self._decode_map)
        self.register_decoder(LocalizationMap, self._decode_map)
        self.register_decoder(DependencyMap, self._decode_map)
        self.register_decoder(DependencyStatusMap, self._decode_map)
        self.register_decoder(InterceptorMap, self._decode_map)
        self.register_decoder(StatusMap, self._decode_map)
        self.register_decoder(ErrorMap, self._decode_map)
        self.register_decoder(SwitchMap, self._decode_map)
        self.register_decoder(SetTargetResponse, self._decode_set_target_response)
        self.register_decoder(SetAttributesResponse, self._decode_set_attributes_response)
        self.register_decoder(GetAttributesResponse, self._decode_get_attributes_response)
        self.register_decoder(ImportResponse, self._decode_import_response)
        self.register_decoder(ExportResponse, self._decode_export_response)

    def _decode_map(self, msg: Message) -> object:
        return self.decode(msg.map)

    def _decode_switch_identifiers(self, msg: SwitchIdentifiers) -> list[str]:
        return msg.switch_names

    def _decode_set_target_response(self, msg: SetTargetResponse) -> list[str]:
        return msg.updated

    def _decode_set_attributes_response(self, msg: SetAttributesResponse) -> list[str]:
        return msg.updated

    def _decode_get_attributes_response(self, msg: GetAttributesResponse) -> object:
        return self.decode(msg.attributes)

    def _decode_import_response(self, msg: ImportResponse) -> int:
        return msg.import_count

    def _decode_export_response(self, msg: ExportResponse) -> dict:
        return self.decode(msg.declarations)

switchboard_dissecter = SwitchboardDissecter()
