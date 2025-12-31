#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client_interceptor.py
## @brief gRPC client interceptor to decode custom error details
## @author Tor Slettnes
#===============================================================================

from .status import DetailedError

### Third-party modules
import grpc
import sys

class ClientInterceptorBase(object):
    '''
    Base for both standard and asyncio gRPC client interceptors
    '''

    def __init__ (self,
                  wait_for_ready: bool = False,
                  intercept_errors: bool = True):
        self.wait_for_ready = wait_for_ready
        self.intercept_errors = intercept_errors

    def intercept_unary_unary(self, continuation, client_call_details, request):
        return self._intercept_response(continuation, client_call_details, request)

    def intercept_unary_stream(self, continuation, client_call_details, request):
        return self._intercept_stream(continuation, client_call_details, request)

    def intercept_stream_unary(self, continuation, client_call_details, request_iterator):
        return self._intercept_response(continuation, client_call_details, request_iterator)

    def intercept_stream_stream(self, continuation, client_call_details, request_iterator):
        return self._intercept_stream(continuation, client_call_details, request_iterator)


class ClientInterceptor(ClientInterceptorBase,
                        grpc.UnaryUnaryClientInterceptor,
                        grpc.UnaryStreamClientInterceptor,
                        grpc.StreamUnaryClientInterceptor,
                        grpc.StreamStreamClientInterceptor):
    '''
    gRPC client interceptor, performing these tasks:

     * Updates `call_details` object of gRPC invocations to include `wait_for_ready`
       flag if not present.

     * Catches gRPC exceptions and re-raises them as `DetailedError()` instances
       to decode custom error details (see `Event` in `event_types.proto`)
    '''

    class FutureResponse (grpc.Future):
        def __init__ (self, response):
            self._response = response

        def cancel(self):
            return self._response.cancel()

        def cancelled(self):
            return self._response.cancelled()

        def running(self):
            return self._response.running()

        def done(self):
            return self._response.done()

        def result(self, timeout=None):
            try:
                return self._response.result()
            except grpc.RpcError as e:
                e_type, e_name, e_tb = sys.exc_info()
                raise DetailedError(e).with_traceback(e_tb) from None

        def exception(self, timeout=None):
            return self._exception

        def traceback(self, timeout=None):
            return self._response.traceback()

        def add_done_callback(self, fn):
            fn(self)


    def _call_details(self, details):
        if details.wait_for_ready is None:
            newdetails = grpc.ClientCallDetails()

            newdetails.method = details.method
            newdetails.timeout = details.timeout
            newdetails.metadata = details.metadata
            newdetails.credentials = details.credentials
            newdetails.wait_for_ready = self.wait_for_ready

            return newdetails
        else:
            return details

    def _intercept_response(self,
                            continuation,
                            client_call_details,
                            request_or_iterator):
        call_details = self._call_details(client_call_details)
        response = continuation(call_details, request_or_iterator)
        return self.FutureResponse(response)

    def _intercept_stream(self,
                          continuation,
                          client_call_details,
                          request_or_iterator):
        call_details = self._call_details(client_call_details)
        try:
            for response in continuation(call_details, request_or_iterator):
                yield response
        except grpc.RpcError as e:
            if self.intercept_errors:
                e_type, e_name, e_tb = sys.exc_info()
                raise DetailedError(e).with_traceback(e_tb) from None
            else:
                raise


class AsyncClientInterceptor(ClientInterceptorBase,
                             grpc.aio.UnaryUnaryClientInterceptor,
                             grpc.aio.UnaryStreamClientInterceptor,
                             grpc.aio.StreamUnaryClientInterceptor,
                             grpc.aio.StreamStreamClientInterceptor):
    '''
    gRPC client interceptor using Python AsyncIO semantics.  Performs these tasks:

     * Updates `call_details` object of gRPC invocations to include `wait_for_ready`
       flag if not present.

     * Catches gRPC exceptions and re-raises them as `DetailedError()` instances
       to decode custom error details (see `protobuf.status.Event` in `event_types.proto`)

    '''

    def _call_details(self, details):
        if details.wait_for_ready is None:
            return grpc.aio.ClientCallDetails(details.method,
                                              details.timeout,
                                              details.metadata,
                                              details.credentials,
                                              self.wait_for_ready)
        else:
            return details


    async def _intercept_response(self,
                                  continuation,
                                  client_call_details,
                                  request_or_iterator):
        call_details = self._call_details(client_call_details)
        try:
            return await continuation(call_details, request_or_iterator)
        except (grpc.RpcError, grpc.aio.AioRpcError) as e:
            e_type, e_name, e_tb = sys.exc_info()
            raise DetailedError(e).with_traceback(e_tb) from None

    async def _intercept_stream(self,
                                continuation,
                                client_call_details,
                                request_or_iterator):

        call_details = self._call_details(client_call_details)
        try:
            async for response in continuation(call_details, request_or_iterator):
                yield response
        except (grpc.RpcError, grpc.aio.AioRpcError) as e:
            if self.intercept_errors:
                e_type, e_name, e_tb = sys.exc_info()
                raise DetailedError(e).with_traceback(e_tb) from None
            else:
                raise

