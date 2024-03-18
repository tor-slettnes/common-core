#!/usr/bin/python3 -i
#===============================================================================
## @file client_reader.py
## @brief Read streamed messages from server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================


import threading, asyncio

class ThreadReader (object):
    '''Stream reader implementation using threads'''

    thread = None

    def active (self):
        return (self.thread is not None)

    def start(self, stub_reader, callback):
        if not self.thread:
            self.thread = threading.Thread(target = self.watch,
                                           name = 'Watcher thread',
                                           args = (stub_reader, callback),
                                           daemon = True)
            self.thread.start()

    def stop(self, wait=False):
        if thread := self.thread:
            self.thread = None
            if wait:
                thread.join()

    def watch(self, stub_reader, callback):
        try:
            for msg in stub_reader:
                callback(msg)
                if not self.thread:
                    break
        finally:
            self.thread = None



class AsyncReader (object):
    '''Stream reader implementation using AsyncIO task.'''

    task = None

    def active (self):
        return self.task and not self.task.done()

    def start(self, stub_reader, callback):
        if not self.active():
            self.task = asyncio.create_task(
                self.watch(stub_reader, callback))

    def stop(self, wait = False):
        if task := self.task:
            task.cancel()
            self.task = None

    async def watch(self, stub_reader, callback):
        try:
            self.active = True
            async for msg in stub_reader:
                callback(msg)
        except asyncio.CancelledError:
            pass
        finally:
            self.active = False
