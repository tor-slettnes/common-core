#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file requesthandler.py
## @brief ZeroMQ RPC request handler using ProtoBuf payloads
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ...google.protobuf import CC, ProtoBuf

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

    * The method name must start with "`handle_`"; the remainder of its name
      maps directly to the expected `method_name` field of the `Request`.

    * It should take one single ProtoBuf message as input argument, and
      include a Python annotation to indicate the specific message type.
      This message type is used to deserialize the input.

    * It should return a single ProtoBuf message, which is then serialized
      and sent back in the `Reply` message to the client.

    ### Example:

    ```
    class MyRequstHandler (ipc.zmq.protobuf.requesthandler.RequestHandler):

        interface_name = 'My RPC Service'

        def handle_my_request(self, arg: MyProtoBufRequestType) -> MyProtoBufReplyType:
            # Do something here
            return MyProtoBufReplyType(myfield='my value', ...)
    ```

    '''

    def __init__ (self, interface_name : str):
        self.interface_name = interface_name

    def process_method_request(self,
                               request : CC.RR.Request,
                               reply   : CC.RR.Reply):

        try:
            handler = getattr(self, 'handle_' + request.method_name)
        except AttributeError:
            self.insert_error(
                reply,
                CC.RR.STATUS_INVALID,
                'Requested method not found',
                symbol = 'NotFound',
                flow = CC.Status.FLOW_CANCELLED,
                attributes = dict(
                    interface_name = request.interface_name,
                    method_name = request.method_name
                )
            )
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

        except (KeyError, IndexError, AttributeError):
            self.insert_error(
                reply,
                CC.RR.STATUS_FAILED,
                "Handler method does not have an input argument "
                "with a ProtoBuf message annottation",
                symbol = 'InvalidHandlerMethod',
                flow = CC.Status.FLOW_CANCELLED,
                attributes = dict(
                    interface_name = self.interface_name,
                    method_name = handler.__name__
                )
            )

        else:
            self._invoke_handler(handler, arg, reply)



    def _invoke_handler(self,
                        handler: Callable[[ProtoBuf.Message], ProtoBuf.Message],
                        arg    : ProtoBuf.Message,
                        reply  : CC.RR.Reply):

        try:
            result = handler(arg)
        except Exception as e:
            self.insert_error(
                reply,
                CC.RR.STATUS_FAILED,
                str(e),
                symbol = type(e).__name__,
                flow = CC.Status.FLOW_ABORTED,
                attributes = dict(
                    exception_args = e.args
                )
            )
        else:
            reply.serialized_proto = result.SerializeToString()


    @classmethod
    def insert_error(self,
                     reply      : CC.RR.Reply,
                     code       : CC.RR.StatusCode,
                     text       : str,
                     symbol     : str = '',
                     flow       : CC.Status.Flow = CC.Status.FLOW_NONE,
                     attributes : dict = {}):

        reply.status.code = code
        reply.status.details.text = text
        reply.status.details.domain = CC.Status.DOMAIN_APPLICATION
        reply.status.details.symbol = symbol
        reply.status.details.level = CC.Status.LEVEL_FAILED
        reply.status.details.flow = flow
        CC.encodeToValueList(attributes, reply.status.details.attributes)
