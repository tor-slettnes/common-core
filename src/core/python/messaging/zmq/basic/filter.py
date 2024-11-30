#===============================================================================
## @file filter.py
## @brief ZeroMQ subscription filter insertion/extraction
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
from typing import Tuple

Topic = str

class Filter (bytes):
    STRING_ENCODING = 'utf8'

    @classmethod
    def create_from_data (cls, data: bytes):
        '''
        Extract a new topic filter instance from an existing byte sequence
        '''

        filter_size = cls._extract_filter_size(data)
        return Filter(data[:filter_size])

    @classmethod
    def create_from_topic (cls, topic: str):
        '''
        Create a new filter from a topic string
        '''

        topic_bytes = topic.encode(cls.STRING_ENCODING)
        preamble = cls.encoded_size(len(topic_bytes))
        return Filter(preamble + topic_bytes)

    @classmethod
    def encoded_size(cls, size: int) -> bytes:
        '''
        Encode topic size into a packed bytes sequence, using variable-length
        encoding

        @param size
            Number of bytes needed to represent topic

        @return
            Byte sequence containing variable-length encoding of `size`
        '''
        more = True
        encoded = bytearray()
        while more:
            more = (size > 0x7F)
            encoded.append((size & 0x7F) | (0x00, 0x80)[more])
            size >>= 7
        return encoded

    @classmethod
    def _encoded_size_size(cls, size: int) -> int:
        '''
        Determine how many bytes will be needed to represent a given topic
        size

        @param size
            Number of bytes needed to represent topic
        @return
            Number of bytes needed to represent `size`
        '''
        encoding_size = 1
        while size > 0x7F:
            encoding_size += 1
            size >>= 7
        return encoding_size

    @classmethod
    def _extract_filter_size(cls, data: bytes) -> Tuple[int, int]:
        '''
        Extract topic length from a byte sequence.

        @param data
            Byte sequence from which size is extracted

        @return
            A two-integer tuple `(preamble_size, topic_size)` containing
            the number of bytes needed to represent the topic length, and
            the actual length extracted from those initial bytes.
        '''
        preamble_size = 0
        topic_size = 0
        more = True

        while more and (preamble_size < len(data)):
            byte = data[preamble_size]
            topic_size = (topic_size << 7) | (byte & 0x7F)
            preamble_size += 1
            more = ((byte & 0x80) != 0)

        return (preamble_size, topic_size)

    def topic(self) -> str:
        startpos, length = self._extract_filter_size(self)
        topic_bytes = self[startpos : startpos+length]
        return topic_bytes.decode(self.STRING_ENCODING)

