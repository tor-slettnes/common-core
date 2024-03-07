ZeroMQ: ProtoBuf wrappers
=========================

This folder contains utiltity methods for sending and receiving Protocol Buffer messages over ZeroMQ:


Publish/Subscribe
-----------------

* [cc::zmq::ProtoBufPublisher()](zmq-protobuf-publisher.h++) extends
  [cc::zmq::Publisher()](../zmq-common/zmq-publisher.h++) with an overloaded
  `publish()` method to serialize and publish ProtoBuf messages.

* [cc::zmq::ProtoBufSubscriber()](zmq-protobuf-subscriber.h++) correspondingly
  builds on [cc::zmq::Subscriber()](../zmq-common/zmq-subscriber.h++) with a
  `subscribe()` template method.

  Rather than subscribing the provided callback method directly, this subscribes
  a wrapper method that deserializes the received binary payload to the
  specified ProtoBuf message type, then passes the result on to the originally
  provided callback method.

* [cc::zmq::ProtoBufSignalSubscriber<>()](zmq-protobuf-signalsubscriber.h++) is
  a template class that builds on the preceding as well as
  [cc::protobuf::SignalReceiver()](../../../dataformats/protobuf/protobuf-signalreceiver.h++) to
  *implicitly* subscribe to specialized `Signal()` messages, as described in
  [signal_types.proto](../../../../../proto/signal_types.proto).

  The demultiplexed payload from incoming `Signal()` messages are then passed on
  to registered handler methods, which then decode the ProtoBuf message into a
  corresponding local data type and emit the result as a local
  [signal](../../../../inner-core/common/thread/signaltemplate.h++).

  If this sounds esoteric and higly specialized, just know that the result is a
  convenient and general way to transparently capture and publish arbitrary
  event data in the process space of a (remote) producer, then subscribe and
  re-emit that same event locally within a consumer process.


Remote Procedure Calls
----------------------

These modules implement a simple RPC scheme using ProtoBuf container messages
defined in [request_reply.proto](../../../../../proto/request_reply.proto).
A RPC call is made up of:

 * A `Request` message sent from the client to the server, containing:

   - Client-provided `client_id` and `request_id` fields to uniquely match each
     call with the resulting response

   - An `interface_name` and `method_name` to specify handler object/method

   - A multiplexed `param` field contianing request parameter(s), encoded as either
      - a (possibly nested) [variant::Value()](../../../../inner-core/common/types/variant-value.h++), or
      - a serialized ProtoBuf message (similar to gRPC).

 * A corresponding `Reply` returned from the server to the client, containing:

   - The reflected `client_id` and `request_id` from the `Request`

   - A `status` field, comprising a status `code` and possibly error `details`

   - A multiplexed `param` field containing response parameter(s), encoded as above.


The following C++ modules are provided:

* [cc::zmq::ProtoBufClient()](zmq-protobuf-client.h++), which connects to a remote
  server and communicates with a specific `interface_name`. At the highest level
  this class provides two overloaded `call()` methods:

    - A `call<>()` method template to invoke a remote method using ProtoBuf
      payloads.  The template parameter is the expected response type (which
      should be a ProtoBuf message type), and is used to deserialize the binary
      response parameter from the server.

    - A plain `call()` method that invokes a remote method using
      `variant::Value()` request/repsonse parameters.

  Both of these methods take the remote method name as the first input, followed
  by the encoded parameter as just described.  If the call is successful the
  return value is the deserialized response from the server, otherwise a
  `[ProtoBufError](zmq-protobuf-error.h++)` exception is thrown.

* [cc::zmq::ProtoBufServer()](zmq-protobuf-server.h++), which binds to a local
  interface and listens for incoming `Request` messages.

  Once a request arrives it is forwarded according to its `interface_name` field
  to a corresponding `RequestHandler()` instance. The resulting `Response`
  message from the handler is then sent back to the requesting client.

  If the incoming request cannot be deserialized or if the requested
  `interface_name` is not registered, an error response is returned to the
  client.

* [cc::zmq::ProtoBufRequestHandler()](zmq-protobuf-requesthandler.h++), which
  handles requests for a specific RPC *interface*.

  Subclasses will normally use the provided `add_handler<>()` method template to
  register handler methods for specific `method_name` requests. The template
  parameter `RequestType` is used to deserialize the request parameter before
  invoking this handler, and correspondingly `ReplyType` is used to serialize
  the reply from the handler back to the client. Typically these types are
  determined implicitly by the compiler.

* [cc::zmq::ProtoBufError()](zmq-protobuf-error.h++), an internal convenience
  container to represent errors generated by the server when processing client
  requests.  On the client side it maps the embedded status code from the reply
  message to suitable exceptions from the
  [exceptions.h++](../../../../inner-core/common/status/exceptions.h++)
  module; these are then thrown back to the caller.
