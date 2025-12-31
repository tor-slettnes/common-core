#!/usr/bin/python3
#===============================================================================
## @file aio_server.py
## @brief Python flavor of 'Demo' gRPC server
## @author Tor Slettnes
#===============================================================================

### Modules relative to install dir
from cc.demo.grpc.service import DemoService
from cc.demo.native import NativeDemo
from cc.messaging.grpc.server import AsyncServerWrapper

### Standard Python modules
import logging
import asyncio

async def main():
    pylogger = logging.getLogger()
    pylogger.setLevel(logging.INFO)

    service = DemoService(NativeDemo())
    server = AsyncServerWrapper(service)

    try:
        logging.info("Starting Python gRPC Demo Server")
        await server.start()
        await server.wait_for_termination()
    except (KeyboardInterrupt, asyncio.exceptions.CancelledError):
        pass
    finally:
        logging.info("Shutting down Python gRPC Demo Server")
        await server.stop(5)

if __name__ == '__main__':
    asyncio.run(main())
