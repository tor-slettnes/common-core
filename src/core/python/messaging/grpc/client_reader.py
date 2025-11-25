#!/usr/bin/python3 -i
#===============================================================================
## @file client_reader.py
## @brief Read streamed messages from server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
import threading, asyncio

class ThreadReader:
    '''
    Stream reader implementation using threads
    '''

    thread = None

    def active (self):
        return (self.thread is not None)

    def start(self, stream, callback):
        if not self.thread:
            self.thread = threading.Thread(target = self.watch,
                                           name = 'Watcher thread',
                                           args = (stream, callback),
                                           daemon = True)
            self.thread.start()

    def stop(self, wait=False):
        if thread := self.thread:
            self.thread = None
            if wait:
                thread.join()

    def watch(self, stream, callback):
        try:
            for msg in stream:
                callback(msg)
                if not self.active():
                    break
        finally:
            self.thread = None
            self.cancel(stream)

    def cancel(self, stream):
        try:
            stream.cancel()
        except (TypeError, AttributeError):
            pass


class AsyncReader:
    '''
    Stream reader implementation using AsyncIO task.
    '''

    task = None

    def active (self):
        return self.task and not self.task.done()

    def start(self, stream, callback):
        if not self.active():
            self.task = asyncio.create_task(
                self.watch(stream, callback))

    def stop(self, wait = False):
        if task := self.task:
            task.cancel()
            self.task = None

    async def watch(self, stream, callback):
        try:
            async for msg in stream:
                callback(msg)
        except asyncio.CancelledError:
            pass
        finally:
            self.task = None
            await self.cancel(stream)

    async def cancel(self, stream):
        try:
            await stream.cancel()
        except (TypeError, AttributeError):
            pass
