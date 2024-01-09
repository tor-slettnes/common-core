#!/usr/bin/python3 -i
#===============================================================================
## @file dds_listener.py
## @brief RTI DDS Listener class
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from typing    import Optional, Union, Callable, NewType, IO
from dds_base  import dds, Base, Path
from process   import safeInvoke

import threading
import logging


class Listener (Base):
    '''
    DDS listener
    '''

    Callback = Callable[[dds.DynamicData], None]

    def __init__(self,
                 provider : Union[dds.QosProvider, Path, IO],
                 domain_id : Optional[int] = None):

        Base.__init__(self, provider, domain_id)
        self.subscriber = dds.Subscriber(self.domain_participant)
        self.waitset = dds.WaitSet()
        self.listener_thread = None
        self.keepalive = False
        self.last_sample = None


    def add_reader(self,
                   idl_type: str,
                   topic_name: Optional[str],
                   callback: Callback):

        topic  =  self.get_topic(topic_name, idl_type)
        reader = dds.DynamicData.DataReader(self.subscriber, topic)
        status_condition = dds.StatusCondition(reader)
        status_condition.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
        status_condition.set_handler(lambda condition_argument:
                                     self._process_data(reader, callback))

        self.waitset += status_condition

    def start_listening(self):
        self.keepalive = True
        t = self.listener_thread
        if not t:
            t = self.listener_thread = threading.Thread(
                None,
                self._listener,
                "Listener thread",
                (),
                {})

            t.setDaemon(True)
            logging.info("Starting %s", t.name)
            t.start()

    def stop_listening(self, wait: bool = False):
        t = self.listener_thread
        if t:
            logging.debug("Ending listener thread")
            self.keepalive = False

            if wait and t.is_alive():
                logging.debug("Waiting for listener thread to exit")
                t.join()

    def _listener(self):
        while self.keepalive:
            self.waitset.dispatch(dds.Duration(1))

    def _process_data(self,
                      reader: dds.DynamicData.DataReader,
                      callback: Callback):

        with reader.take() as samples:
            for sample in samples:
                if sample.info.valid:
                    self.last_sample = sample
                    safeInvoke(callback, (sample,), {})
