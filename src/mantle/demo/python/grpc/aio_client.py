#!/usr/bin/python3
#===============================================================================
## @file aio_client.py
## @brief Invoke a server method using Python AsyncIO semantics
## @author Tor Slettnes
#===============================================================================

### Standard Python modules
import asyncio
import argparse
import os.path
import sys
import logging

### Modules relative to install folder
from .client import Client as DemoClient
from cc.messaging.grpc.client import AsyncMixIn

### Add a few arguments to the base argparser
class ArgParser (argparse.ArgumentParser):
    def __init__ (self, prog=os.path.basename(sys.argv[0]), identity="DemoShell", *args, **kwargs):
        super().__init__(prog=prog, *args, **kwargs);

        self.add_argument('--identity',
                          type=str,
                          default=identity,
                          help="Identity of this client")

        self.add_argument('--host',
                          type=str,
                          default="localhost",
                          help="Host for remote services")

        self.add_argument('--debug',
                          default=False,
                          action='store_const',
                          const=True,
                          help='Print debug messages')

        self.add_argument('--use-signal-queue',
                          default=False,
                          action='store_const',
                          const=True,
                          help='If monitoring (i.e., no command given), use a signal queue'
                          'rather than individual stream.')

        self.add_argument('command',
                          type=str,
                          nargs='?',
                          default=None,
                          help="Invoke a function with Python AsyncIO semantics")


class AsyncDemoClient(AsyncMixIn, DemoClient):
    pass

async def main(args):
    demo = AsyncDemoClient(args.host,
                           identity = args.identity)
    demo.initialize()
    # result = await demo.get_current_time()

    if args.command:
        result = await eval(args.command)

    elif args.use_signal_queue:
        signalqueue = asyncio.Queue()
        demo.start_notify_time(signalqueue.put)
        demo.start_notify_greetings(signalqueue.put)
        try:
            while True:
                msg = await signalqueue.get()
                print(msg)
        except KeyboardInterrupt as e:
            demo.stop_notify_greetings()
            demo.stop_notify_time()

    else:
        try:
            async for msg in demo.watch():
                print(msg)
        except KeyboardInterrupt:
            pass


    demo.deinitialize()
    print(result)


if __name__ == "__main__":
    args   = ArgParser().parse_args()

    pylogger = logging.getLogger()
    pylogger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

    asyncio.run(main(args))
