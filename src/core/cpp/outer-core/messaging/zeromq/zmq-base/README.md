ZeroMQ: Common Wrappers
=======================

This folder contains convenience wrappers for sending and receiving binary/encoded payloads over ZeroMQ:

* [cc::zmq::Endpoint()](zmq-endpoint.h++]): Superclass for everything below, and provides
  - common mechanisms for address/service discovery as well as
  - `send()`/`receive()` rappers for binary payloads.

* [cc::zmq::Responder()](zmq-responder.h++): Spawn a thread to listen for incoming requests.

  Once a request is received it is passed on to the virtual method `process_zmq_request()`, which unless overridden this then invokes the virtual method `process_binary_request()`.  Derived responder/server subclasses should thus override either one of these methods to handle the request and generate a reply, which is then sent back to the requester.

* [cc::zmq::Requester()](zmq-requester.h++): Adds method `send_receive()` to issue a request and then block for a reply from the responder.

* [cc::zmq::Publisher()](zmq-publisher.h++): Provides methods `publish()` and `publish_topic()` to publish binary payloads, with or without a message topic (see below).

* [cc::zmq::Subscriber()](zmq-subscriber.h++): Spawn a thread to listen for publications. Methods `subscribe()` and `subscribe_topic()` are provided to register a single callback method; or in the latter case, a single callback method per matching topic.

  A typical callback method might deserialize the binary payload and then emit the resulting value as a local [signal](../../../../inner-core/common/thread/signaltemplate.h++) to interested parties.

* [cc::zmq::Filter()](zmq-filter.h++): Create, insert, and extract pub/sub topics as ZMQ message filters. These are prepended before the binary payload, and comprise
 - The topic length, encoded as a ProtoBuf-style [variable length integer](https://protobuf.dev/programming-guides/encoding/#varints) (a single byte if the length is below 128), and
 - The message topic in [UTF-8](https://en.wikipedia.org/wiki/UTF-8) format.
