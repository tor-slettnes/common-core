'''
thread_reader - Stream messages from server in a background thread
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from collections.abc import abstractmethod
import threading
import asyncio

### Common Core modules
from cc.core.decorators import override

#-------------------------------------------------------------------------------
# Reader

class AbstractReader:
    '''
    Stream reader - abstract base.
    '''

    @abstractmethod
    def active (self) -> bool:
        raise NotImplementedError

    @abstractmethod
    def start(self, stream, callback):
        raise NotImplementedError

    @abstractmethod
    def stop(self, wait=False):
        raise NotImplementedError

    @abstractmethod
    def cancel(self, stream):
        pass

#-------------------------------------------------------------------------------
# ThreadReader

class ThreadReader (AbstractReader):
    '''
    Stream reader implementation using threads
    '''

    thread = None

    @override
    def active (self) -> bool:
        return (self.thread is not None)

    @override
    def start(self, stream, callback):
        if not self.thread:
            self.thread = threading.Thread(target = self.worker,
                                           name = 'gRPC Reader Thread',
                                           args = (stream, callback),
                                           daemon = True)
            self.thread.start()

    @override
    def stop(self, wait=False):
        if thread := self.thread:
            self.thread = None
            if wait:
                thread.join()

    def worker(self, stream, callback):
        try:
            for msg in stream:
                callback(msg)
                if not self.active():
                    break
        finally:
            self.thread = None
            self.cancel(stream)

    @override
    def cancel(self, stream):
        try:
            stream.cancel()
        except (TypeError, AttributeError):
            pass

#-------------------------------------------------------------------------------
# AsyncReader

class AsyncReader (AbstractReader):
    '''
    Stream reader implementation using AsyncIO task.
    '''

    task = None

    @override
    def active (self) -> bool:
        return self.task and not self.task.done()

    @override
    def start(self, stream, callback):
        if not self.active():
            self.task = asyncio.create_task(
                self.worker(stream, callback))

    @override
    def stop(self, wait = False):
        if task := self.task:
            self.task = None
            return task.cancel()

    async def worker(self, stream, callback):
        try:
            async for msg in stream:
                callback(msg)
        except asyncio.CancelledError:
            pass
        finally:
            self.task = None
            await self.cancel(stream)

    @override
    async def cancel(self, stream):
        try:
            await stream.cancel()
        except (TypeError, AttributeError):
            pass
