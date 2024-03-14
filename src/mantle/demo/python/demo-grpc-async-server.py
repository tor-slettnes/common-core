#!/usr/bin/python3 -i
#===============================================================================
## @file demo-grpc-server.py
## @brief Python flavor of 'Demo' gRPC server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from demo.grpc.service import DemoService
from demo.native import NativeDemo
from cc.messaging.grpc.server import create_async_server

### Standard Python modules
import logging
import asyncio

async def serve(server):
    logging.info("Starting Python gRPC Demo Server")
    await server.start()
    await server.wait_for_termination()


async def server_graceful_shutdown(server):
    logging.info("Shutting down Python gRPC Demo Server")
    await server.stop(5)


if __name__ == '__main__':
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)

    demoservice = DemoService(NativeDemo())
    demoserver = create_async_server(demoservice)

    loop = asyncio.get_event_loop();
    try:
        loop.run_until_complete(serve(demoserver))
    finally:
        loop.run_until_complete(server_graceful_shutdown(demoserver))
        loop.close()
