'''
Python client for Pub/Sub Relay gRPC service.

Example usage:

* Create a Pub/Sub AsyncClient instance:

  ```
  python3
  >>> from cc.platform.pubsub.grpc.client import Client, MessageTuple
  >>> pubsub = Client("localhost")
  ```

* Listen for specific topics in the foreground:

  ```
  >>> for msg in pubsub.listen(topics=("my topic",)):
  ...     print(f"{msg=}\r")
  ```

  (This blocks until cancelled, e.g. via **[Ctrl]** + **[C]**)

* Publish a message from a new terminal, e.g. using `cc-pubsub-tool`

  ```
  cc-pubsub-tool publish 'my topic' '{"one": true, "two": 2, "three": 3.141592653589793238, "four":"IV"}' --json
  ```

* Observe that the message appears in the first terminal

  ```
  msg=MessageTuple(topic='my topic', value={'four': 'IV', 'two': 2, 'three': 3.141592653589793, 'one': True})
  ```

* Add a subscription:

  ```
  >>> def my_subscriber(msg: MessageTuple):
  ...     """callback handler, invoked asynchronously on matching message publications"""
  ...     print(f"{msg=}\r")
  ...
  >>> handle = pubsub.subscribe(topics=["my topic"], callback=my_subscriber)
  ```

* Publish a message via the Relay.  The subscriber above will be invoked from an asynchronous worker thread:

  ```
  >>> pubsub.publish("my topic", {"one":True, "two":2, "three":3.141592653589793238, "four":"IV"})

  msg=MessageTuple(topic='my topic', value={'three': 3.141592653589793, 'two': 2, 'one': True, 'four': 'IV'})
  >>>
  ```

* Enqueue a message for asynchronous publication.

  ```
  >>> pubsub.enqueue("my topic", {"five":False, "six":[1,2,3]})
  >>> msg=MessageTuple(topic='my topic', value={'six': [1, 2, 3], 'five': False})
  ```

  **Note:** Upon first invocation, `enqueue()` creates a publication queue, as well as a worker thread to read from this queue and publish the messages in the background.  The maximum queue size can be provided in the `Client()` constructor (see `BaseClient.__init__()`).

* Cancel the subscription

  ```
  >>> pubsub.unsubscribe(handle)
  ```

'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from typing import Callable, Iterator, Sequence, Mapping
from threading import Thread

### Common Core modules
from cc.core.decorators import override
from cc.core.invocation import safe_invoke
from cc.core.roundrobin import Queue
from cc.protobuf.dissecter import decode_message

### Pub/Sub module
from ..protobuf import (
    MessageTuple,
    TopicName,
    TopicsInput,
    ReplayPolicy,
    ReplayPolicyMapInput,
    decodePublication,
)
from .base_client import BaseClient, Subscription

#-------------------------------------------------------------------------------
# Relay Client

class Client (BaseClient):
    __doc__ = BaseClient.__doc__

    writer_thread = None

    @override
    def assign_replay_policies(self, policy_map: ReplayPolicyMapInput):
        self.call_assign_replay_policies(policy_map)

    @override
    def assign_replay_policy(self,
                             topic: TopicName,
                             key_paths: Sequence[str]|None = None):
        self.call_assign_replay_policies({topic: key_paths})

    @override
    def unassign_replay_policies(self, topics: TopicsInput):
        self.call_unassign_replay_policies(topics)

    @override
    def unassign_replay_policy(self, topic: TopicName):
        self.call_unassign_replay_policies(topic)

    @override
    def clear_replay_policies(self):
        self.call_unassign_replay_policies(())

    @override
    def get_replay_policies(self) -> Mapping[TopicName, ReplayPolicy]:
        policy_map = self.call_get_replay_policies(())
        return decode_message(policy_map.map)

    @override
    def get_replay_policy(self, topic: TopicName) -> ReplayPolicy|None:
        policy_map = self.call_get_replay_policies(topic)
        return decode_message(policy_map.map)

    @override
    def writer_active(self) -> bool:
        if t := self.writer_thread:
            return t.is_alive()
        else:
            return False

    @override
    def start_writer(self):
        if not self.writer_active():
            t = Thread(target = self._write_worker,
                       daemon = True)
            self.write_queue = Queue(self.queue_size)
            self.writer_thread = t
            t.start()

    @override
    def stop_writer(self):
        if t := self.writer_thread:
            self.writer_thread = None
            self.write_queue.put(None)
            t.join()

    def _write_worker(self):
        while self.writer_thread:
            while publication := self.write_queue.get():
                self.stub.Publish(publication, wait_for_ready=True)

    @override
    def listen(self,
               topics: str|Sequence[str]|None = None,
               wait_for_ready = True,
               ) -> Iterator[MessageTuple]:

        try:
            for msg in BaseClient.listen(**locals()):
                yield decodePublication(msg)
        except KeyboardInterrupt:
            print("Cancelling subscription")


    @override
    def start_reader(self, subscription: Subscription):
        thread = Thread(
            target = self._read_worker,
            args=(subscription,),
            daemon = True)
        thread.start()

    @override
    def stop_reader(self, subscription: Subscription):
        try:
            subscription.stream.cancel()
        except AttributeError:
            ## Readers from older `grpcio` verisons do not have a `cancel()` method
            subscription.stream.throw(StopIteration)

        ### Older versions of the standard Python gRPC client stream do not have
        ### a `cancel()` method.  There is a `stop()`, but attempt to invoke
        ### this on an active stream raises a ValueError.  So in lieu of
        ### cancelling the stream, we simply set the `callback` field to
        ### None. The worker thread will then exit after reading the next
        ### publication from the relay.
        subscription.callback = None


    def _read_worker(self, subscription: Subscription):
        for msg in subscription.stream:
            if callback := subscription.callback:
                ### We are still subscribed.
                safe_invoke(
                    function = callback,
                    args = (decodePublication(msg),),
                )
            else:
                ### We the client unubscribed while we were waiting for the next message.
                ### Exit the thread here.
                break


if __name__ == "__main__":
    client = Client()
    handle = client.subscribe(callback = lambda publication: print(f"{publication=}\r"))
