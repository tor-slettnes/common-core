#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file requesthandler.py
## @brief ZeroMQ RPC request handler using ProtoBuf payloads
## @author Tor Slettnes
#===============================================================================

### Modules within package
from .error import Error
from cc.protobuf.wellknown import Empty
from cc.protobuf.status import Error as Event
from cc.protobuf.variant import keyValueMap
from cc.protobuf.request_reply import Request, Reply, StatusCode

### Third-party modules
from google.protobuf.message import Message

### Standard Python modules
from typing import Callable
from inspect import getfullargspec


class RequestHandler:
    '''
    Handle RPC requests received as ProtoBuf
    `cc.protobuf.request_reply.Request` messages.

    The `Request` message is processed as follows:

    * The `interface_name` corresponds to this handler instance.

    * The `method_name` is used to look up a handler method within this class.
      This handler method should take a single ProtoBuf message as input,
      and return a single ProtoBuf message.

    * The `param` field is deserialized as a ProtoBuf message based on the
      annotated signature of the handler method.

    * The handler method is invoked with the deserialized input argument.

    * The return value from the handler method is serialized and incorportaed
      into the `param` field of the `Reply` message which will be sent back to
      the caller.

    * If the handler method raises an exception, the exception data is
      captured into the `status` field of the `Reply` message.

    * The resulting `Reply` message is sent back to the client.

    `Request` messages and generate corresponding `Reply`


    Subclasses should define handler methods with the following criteria:

    * The method name maps directly to the `method_name` field of the `Request`.

    * It should take one single ProtoBuf message as input argument, and
      include a Python annotation to indicate the specific message type.
      This message type is used to deserialize the input.

    * It should return a single ProtoBuf message, which is then serialized
      and sent back in the `Reply` message to the client.

    ### Example

      ```python
      from messaging.zmq.protobuf.requesthandler import RequestHandler

      class MyRequstHandler (RequestHandler):

          interface_name = 'My RPC Service'

          def my_request(self, arg: MyProtoBufRequestType) -> MyProtoBufReplyType:
              # Do something here
              return MyProtoBufReplyType(myfield='my value', ...)
      ```
    '''

    def __init__ (self, interface_name : str):
        self.interface_name = interface_name

    def __call__(self,
                 request : Request,
                 reply   : Reply):

        try:
            handler = getattr(self, request.method_name)
        except AttributeError:
            Error(StatusCode.INVALID,
                  Event(text = 'Requested method not found',
                        symbol = 'NotFound',
                        attributes = keyValueMap(
                            interface_name = request.interface_name,
                            method_name = request.method_name)
                        )
                  ).add_to_reply(reply)
        else:
            self._process_handler_invocation(handler, request, reply)

        return reply

    def _process_handler_invocation(self,
                        handler: Callable[[Message], Message],
                        request: Request,
                        reply  : Reply):

        argspec = getfullargspec(handler)
        try:
            argname = argspec.args[1] # Bypass the 1st argument (self)
            annotated_type = argspec.annotations[argname]
            arg = annotated_type.FromString(request.param.serialized_proto)

        except IndexError: # Handler takes no arguments
            self._invoke_handler(handler, (), reply)

        except (KeyError, IndexError, AttributeError):
            Error(StatusCode.FAILED,
                  Event(
                      text = "Handler method does not have an input argument "
                      "with a ProtoBuf message annotation",
                      symbol = 'InvalidHandlerMethod',
                      attributes = keyValueMap(
                          interface_name = self.interface_name,
                          method_name = handler.__name__
                      )
                  )
            ).add_to_reply(reply)

        else:
            self._invoke_handler(handler, (arg,), reply)



    def _invoke_handler(self,
                        handler: Callable[[Message], Message],
                        args   : tuple,
                        reply  : Reply):

        try:
            result = handler(*args)
        except Exception as e:
            Error(StatusCode.FAILED,
                  Event(text = str(e),
                        symbol = type(e).__name__,
                        attributes = keyValueMap(
                            exception_args = e.args)
                        )
                  ).add_to_reply(reply)
        else:
            if result is None:
                result = Empty()
            reply.param.serialized_proto = result.SerializeToString()
