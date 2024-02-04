#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file requesthandler.py
## @brief ZeroMQ RPC request handler using ProtoBuf payloads
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ...google.protobuf import CC, ProtoBuf
from .error             import Error

from typing  import Callable
from inspect import getfullargspec

class RequestHandler (object):
    '''Handle RPC requests received as ProtoBuf `CC.RR.Request` messages.

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

    ### Example:

    ```
    from ipc.zmq.protobuf.requesthandler import RequestHandler

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
                 request : CC.RR.Request,
                 reply   : CC.RR.Reply):

        try:
            handler = getattr(self, request.method_name)
        except AttributeError:
            Error(CC.RR.STATUS_INVALID,
                  CC.Status.Details(
                      text = 'Requested method not found',
                      symbol = 'NotFound',
                      flow = CC.Status.FLOW_CANCELLED,
                      attributes = CC.ValueList(
                          interface_name = request.interface_name,
                          method_name = request.method_name
                      )
                  )
            ).add_to_reply(reply)
        else:
            self._process_handler_invocation(handler, request, reply)

        return reply

    def _process_handler_invocation(self,
                        handler: Callable[[ProtoBuf.Message], ProtoBuf.Message],
                        request: CC.RR.Request,
                        reply  : CC.RR.Reply):

        argspec = getfullargspec(handler)
        try:
            argname = argspec.args[1] # Bypass the 1st argument (self)
            annotated_type = argspec.annotations[argname]
            arg = annotated_type.FromString(request.param.serialized_proto)

        except IndexError: # Handler takes no arguments
            self._invoke_handler(handler, (), reply)

        except (KeyError, IndexError, AttributeError):
            Error(CC.RR.STATUS_FAILED,
                  CC.Status.Details(
                      text = "Handler method does not have an input argument "
                      "with a ProtoBuf message annottation",
                      symbol = 'InvalidHandlerMethod',
                      flow = CC.Status.FLOW_CANCELLED,
                      attributes = CC.valueList(
                          interface_name = self.interface_name,
                          method_name = handler.__name__
                      )
                  )
            ).add_to_reply(reply)

        else:
            self._invoke_handler(handler, (arg,), reply)



    def _invoke_handler(self,
                        handler: Callable[[ProtoBuf.Message], ProtoBuf.Message],
                        args   : tuple,
                        reply  : CC.RR.Reply):

        try:
            result = handler(*args)
        except Exception as e:
            Error(CC.RR.STATUS_FAILED,
                  CC.Status.Details(
                      text = str(e),
                      symbol = type(e).__name__,
                      flow = CC.Status.FLOW_ABORTED,
                      attributes = CC.valueList(
                          exception_args = e.args
                      )
                  )
            ).add_to_reply(reply)
        else:
            if result is None:
                result = ProtoBuf.Empty()
            reply.param.serialized_proto = result.SerializeToString()
