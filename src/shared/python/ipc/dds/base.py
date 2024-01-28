#!/usr/bin/python3
#===============================================================================
## @file dds_base.py
## @brief Support for RTI DDS - base class
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from typing import Optional, Union, NewType, IO

import io
import os

import rti.connextdds as dds
import paths

Path = NewType('Path', str)

class Base (object):
    '''
    DDS Base class
    '''

    ### Map of QoS providers that can be shared amongst instances
    qos_providers = {}

    ### Map of domain participants that can be shared amongst instances
    domain_participants = {}

    ### Topics per participant
    topics = {}

    ### Default domain ID
    DOMAIN_ID = 0

    def __init__(self,
                 provider : Union[dds.QosProvider, Path, IO],
                 domain_id : Optional[int] = None):

        self.qos_provider = self._get_qos_provider(provider)
        self.domain_participant = self._get_domain_participant(domain_id)


    @classmethod
    def _get_qos_provider(cls, provider: Union[dds.QosProvider, Path, IO]) -> dds.QosProvider:
        if isinstance(provider, dds.QosProvider):
            return provider

        if isinstance(provider, io.IOBase):
            xml_path = provider.name

        elif isinstance(provider, str):
            xml_path = provider
            if not os.path.isabs(provider):
                base, xml_dir = paths.locateDominatingPath('share/xml')
                if xml_dir:
                    xml_path = os.path.join(xml_dir, xml_path)
                stem, ext = os.path.splitext(xml_path)
                if not ext:
                    xml_path += ".xml"

        else:
            raise TypeError("Argument 'provider' must be an existing QoS provider, "
                            "a string, or a file object, not %r"%(provider,))

        abspath = os.path.abspath(xml_path)
        try:
            return cls.qos_providers[abspath]
        except KeyError:
            print("Creating QosProvider(%r)"%(abspath,))
            try:
                return cls.qos_providers.setdefault(
                    abspath,
                    dds.QosProvider(abspath))
            except dds.Error as e:
                raise e from None

    @classmethod
    def _get_domain_participant(cls, domain_id: int) -> dds.DomainParticipant:
        if domain_id is None:
            domain_id = cls.DOMAIN_ID
        try:
            return cls.domain_participants[domain_id]
        except KeyError:
            return cls.domain_participants.setdefault(
                domain_id,
                dds.DomainParticipant(domain_id))


    def get_topic(self,
                  name: str,
                  data_type: Union[str, dds.StructType]) -> dds.DynamicData.Topic:

        if isinstance(data_type, str):
            data_type = self.qos_provider.type(data_type)

        try:
            return Base.topics[self.domain_participant.domain_id][name]
        except KeyError:
            topics = Base.topics.setdefault(self.domain_participant.domain_id, {})
            topic = topics[name] \
                = dds.DynamicData.Topic(self.domain_participant, name, data_type)
            return topic


{
