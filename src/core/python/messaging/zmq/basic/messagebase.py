#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagebase.py
## @brief Base for MessageWriter and MessageHandler
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

class MessageBase:
    '''
    Base for MessageWriter and MessageHandler
    '''

    def encode_topic(self, topic: str|bytes|bytearray|None) -> bytes:
        if topic is None:
            return None
        elif isinstance(topic, bytes):
            return topic
        elif isinstance(topic, bytearray):
             return bytes(topic)
        elif isinstance(topic, str):
            return topic.encode("UTF-8")
        else:
            raise TypeError("expected bytes, bytearray, or string")
