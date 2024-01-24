ZeroMQ messaging
================

These folders contain convenience/standardization wrappers for messaging over [ZeroMQ](https://zeromq.org):

* [zmq-common](zmq-common): Peer/address lookup, bind/connect, send/receive, request/reply and pub/sub wrappers (incl topic insertion/filtering/extraction)
* [zmq-protobuf](zmq-protobuf): ZMQ communciation with standardized ProtoBuf messages, including RPC client/server wrappers and event propagation via pub/sub
* [zmq-pypickle](zmq-pypickle): ZMQ communication with serialized Python (pickle) payloads [TBD].
