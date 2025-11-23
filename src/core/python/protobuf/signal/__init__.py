'''
Package `cc.protobuf.signal` - data types used to implement the signal/slot
pattern using custom ProtoBuf messages.  These signals can then be propagated
across applications over transports such as gRPC, thereby emulating the pub/sub
pattern.

Our implementation makes use of a container message to multiplex between the
various signals your application provides, looking something like this:

  ```proto
  message MySignalMultiplexer
  {
      oneof signal
      {
          MyTypeA signal_a = 8;
          MyTypeB signal_b = 9;
          ...
      }
  }
  ```

We also support a specialization of the above, which we will refer to as
"mapping signal". Here, two extra fields are introduced:

  ```proto
  message MySignalMultiplexer
  {
      // Mapping action: one of ADDITION, UPDATE, REMOVAL
      cc.protobuf.signal.MappingAction mapping_action = 1;

      // Mapping key
      string mapping_key = 2;

      oneof signal
      {
          MyTypeA signal_a = 8;
          MyTypeB signal_b = 9;
          ...
      }
  }
  ```

Below you will find the `MappingAction` type used for this purpose.

Secondly, to propagate such signals over gRPC, your service needs a method to
stream signals, similar to the following:

  ```proto
  service MyService
  {
      ...
      rpc Watch (cc.protobuf.signal.Filter) returns (stream MySignalMultiplexer);
  }
  ```

The `Filter` type is defined and described below, and is used by the client to
select which of the available signals the client wants to receive (similar to a
_topic_ in a pub/sub scheme).  An empty `Filter` means listen for all signal
events.
'''

from .signalstore import MappingAction, Filter, SignalMessage, Slot, SignalStore
from .cachingsignalstore import CachingSignalStore
