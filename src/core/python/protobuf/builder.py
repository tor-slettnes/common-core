'''
Build ProtoBuf messages from native Python values.

By default, ProtoBuf message fields are populated recursively from provided
inputs, down to their primitive value types (bool, int, float, string, ...),
just as if they were passed in to the corresponding message constructors.

However, this behavior may be overriden for specific message types by
registering custom encoders for those types.  Rather than descending into
and encoding individual fields of such a registered message type, the input
value is passed onto the registered encoder for that type.

Encoder functions are pre-registered for:

- well-known message types (such as `google.protobuf.Timestamp`), and
- common message types defined in the `cc.protobuf` namespace (such as
  `cc.protobuf.variant.Value`).

In those cases the input is typically assumed to be common Python
representation of the corresponding data type.  For example:

 - A `Timestamp` may be provided in any format accepted by the function
   `cc.protobuf.wellknown.encodeTimestamp()`, including scalar values
   returned from `time.time()`, a `datetime` object, an ISO string, etc.

 - A `Variant` value would typically be a native object of any of the
   supported types, including `bool`, `int`, `real`, `string`, a supported
   Timestamp or Duration type, or a nested dictionary or list.

Additional encoders may be registered in subclasses, and/or in specific
builder instances.

Enumerated values may be provided as either
 - An integer, representing the enumerated value
 - A string, represening the enumerated symbol
 - A Python `Enum` instance with corresonding numeric values.


## Example usage:

Let's say you are implementing a Python client wrapper for the following gRPC
service method from the `Demo` service:

  ```protobuf

  service Demo {
    rpc say_hello (Greeting) returns (google.protobuf.Empty);
  }

  message Greeting
  {
    // The mesage presented by the greeter
    string text = 1;

    // The client's unique identity. We store the last greeting from each client.
    string identity = 2;

    // When was the greeter created?
    google.protobuf.Timestamp birth = 4;

    // Arbitrary key/value pairs associated with the greeting
    cc.variant.KeyValueMap data = 5;
  }
  ```

You'll probably start with scaffolding like the following:

  ```python

  import cc.protobuf.demo

  class DemoClient (cc.messaging.grpc.Client):

      from cc.generated.demo_pb2_grpc import DemoStub as Stub
      ...

      def __init__(self, identity: str|None = None):
          self.identity = identity or os.path.basename(sys.argv[0])
          self.birth    = time.time()

      def say_hello(self,
                    text: str,
                    data: dict = {}):

          request = cc.protobuf.demo.Greeting(...)
          self.stub.say_hello(request)
  ```

In order to construct the `request` message, you may have some custom code like this:

  ```python
          request = Greeting(
              text = text,
              identity = self.identity,
              birth = cc.protobuf.wellknown.encodeTimestamp(self.birth),
              data = cc.protobuf.variant.encodeKeyValueMap(data),
          )
   ```

Not too bad.  Now, what happens as the message types get more complex, with more
input arguments? Moreover, what happens if you need to do build that very same
ProtoBuf message from native Python inputs multiple places in your code?  (For
instance, let's say you're building another Python wrapper for the same service,
using Python AsyncIO semantics).

This can get tedious and brittle real fast.  That's where this module comes in.

First, by using `MessageBuilder` as an additional base for your `DemoClient`
implementation, you can pass in Python native arguments directly:

  ```python
          request = self.build_from_dict(
              Greeting,
              text = text,
              identity = self.identity,
              birth = self.birth,
              data = data,
          )
   ```

OK, great - but still a bit tedious to maintain as the argument list grows and
changes - especially if you're repeating this construction in multiple places.

Luckily, Python gives us a way to access all input arguments in a single
dictionary: `locals()`. (Well, technically it gives us a dictionary of local
variables, including `self`, but used carefully it will suffice for our
purpose).  Taking advantage of this, we can now use:

  ```python
          request = self.build_from_dict(
              Greeting,
              locals(),
              identity = self.identity,
              birth = self.birth,
          ))
  ```

Putting this all together, we get:

  ```python

  import cc.protobuf.demo
  import cc.protobuf.builder

  class DemoClient (cc.messaging.grpc.Client, cc.protobuf.builder.MessageBuilder):

      from cc.generated.demo_pb2_grpc import DemoStub as Stub
      ...

      def __init__(self, identity: str|None = None):
          self.identity = identity or os.path.basename(sys.argv[0])
          self.birth    = time.time()

      def say_hello(self,
                    text: str,
                    data: dict = {}):

          request = self.build_from_dict(
              Greeting,
              locals(),
              identity = self.identity,
              birth = self.birth,
          ))
          self.stub.say_hello(request)
  ```

'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'


from collections.abc import Callable, Sequence
import enum
import logging

from google.protobuf.descriptor import Descriptor, FieldDescriptor, EnumDescriptor

from cc.protobuf.wellknown import Message, MessageType
import cc.protobuf.wellknown as wellknown # Well-known types from Google
import cc.protobuf.variant   as variant   # Common Core variant type
import cc.protobuf.datetime  as datetime  # Calendar date/time types
import cc.protobuf.status    as status    # Status types

Encoder       = Callable[[object], Message]
TEXT_ENCODING = 'utf-8'


class MessageBuilder:
    '''
    Build ProtoBuf messages from native Python values.

    This class will normally be inherited and specialized for specific
    interfaces, e.g. gRPC service clients.

    See `help(cc.protobuf.builder)` for more detailed information, including
    usage examples.
    '''

    def __init__ (self):
        self.encoders = type(self).message_encoders.copy()


    def register_encoder(self,
                         message_type: MessageType,
                         encoder: Encoder,
                         ):
        '''
        Register a custom encoder for a ProtoBuf message type.

        @param message_type
            A ProtoBuf message type for which this encoder is registered.
            Future direct or indirect invocations of `build_from_value()` will
            use this encoder whenever it needs to create a message of this type.

        @param encoder
            Encoder function, which will receive input value (normally a native
            Python object) as its sole argument, and is expected to produce a
            new instance of `message_type`.
        '''

        self.encoders[type] = encoder


    def build_from_value(self,
                         message_type: MessageType,
                         value: object|None = None) -> Message:
        '''
        Create a new ProtoBuf message from the provided input value.

        @param message_type
            A ProtoBuf message type to create.

        @param value
            Input value. Depending on the registered encoder, this will normally
            be a corresponding native Python object.  If no encoder is
            registered, the input should be a dictionary with keys matching the
            field names of the desired ProtoBuf message.
        '''

        if not isinstance(message_type, MessageType):
            raise TypeError("'message_type' argument must be a valid ProtoBuf message type")

        try:
            encoder = self.encoders[message_type]
        except KeyError:
            if isinstance(value, dict):
                return self.build_from_dict(message_type, value)
            else:
                raise TypeError(f"{message_type.__name__} encoder input must be a dictionary")

        else:
            return encoder(value)


    def build_from_dict(self,
                        message: Message|MessageType,
                        inputs: dict|None = None,
                        ignore_inputs: Sequence[str] = ('self', 'cls'),
                        /,
                        **kwargs) -> Message:

        '''
        Build a new ProtoBuf message or populate an existing message with
        provided input values.

        @param message
            Either a messase type to create, or a message instance to populate.

        @param inputs
            A dictionary containing key/value pairs from which to populate each
            field of the message.

        @param kwargs
            Explicit field names and corresponding input values.

        Fields whose name is not found in the `inputs` dictionary nor provided
        as keyword arguments are left empty.  Conversely, keys and keyword
        arguments that are not mapped to any field names are silently ignored.
        (This allows `locals()` to be passed in as the `inputs` argument, even
        though it contains extra local variables from the call site such as
        `self`).
        '''

        if isinstance(message, type):
            message = message()

        if not isinstance(message, Message):
            raise TypeError("'message' argument must be a valid ProtoBuf message or message type")

        inputs = (inputs or {}) | kwargs

        for (field_name, field_descriptor) in message.DESCRIPTOR.fields_by_name.items():
            input_value = inputs.pop(field_name, None)
            if input_value is not None:
                setattr(message,
                        field_name,
                        self.encode_field(field_descriptor, input_value))

        if extra_inputs := set(inputs) - set(ignore_inputs):
            logging.warning(
                '%s.build_from_dict() received extra inputs not applicable for message type %s: %s'%
                (type(self).__name__, type(message).__name__, extra_inputs))

        return message


    def encode_field(self,
                     fd: FieldDescriptor,
                     value: object) -> object:

        if enum_type := fd.enum_type:
            encoder = lambda value: self.encode_enum(enum_type, value)

        elif message_type := fd.message_type:
            encoder = lambda value: self.build_from_value(message_type, value)

        else:
            encoder = lambda value: value

        if fd.label == fd.LABEL_REPEATED:
            if isinstance(value, (list, tuple)):
                return [ encoder(item) for item in value ]
            else:
                return [ encoder(value) ]

        else:
            return encoder(value)


    def encode_enum(self,
                    descriptor: EnumDescriptor,
                    value: object):

        if encoder := self.enum_encoders.get(descriptor):
            return encoder(value)
        elif isinstance(value, (int, enum.IntEnum)):
            return value
        elif isinstance(value, enum.Enum):
            return self.encode_enum_name(descriptor, value.name)
        elif isinstance(value, str):
            return self.encode_enum_name(descriptor, value)
        else:
            raise TypeError('Cannot convert %s to ProtoBuf enumeration: %s'%
                            (type(value).__name__, value))


    def encode_enum_name(self,
                         descriptor: EnumDescriptor,
                         symbol: str) -> int:
        '''
        Translate a symbolic name to an ProtoBuf enumerated value.

        If the symbol does not exist among the enumerated values, try again by
        prepending the largest possible string ending in `"_"` that is common to
        all symbols in the enumeration. For instance, given the ProotBuf
        declaration

        ```protobuf
        enum MyEnum
        {
            MY_ENUM_UNSET = 0;
            MY_ENUM_ONE   = 1;
            MY_ENUM_TWO   = 2;
        };
        ```

        passing in either `"MY_ENUM_ONE"` or `"ONE"` would return the value `1`.
        '''

        try:
            return descriptor.values_by_name[symbol].number
        except KeyError:
            common_base = self.largest_common_base(descriptor.values_by_name.keys())
            try:
                return descriptor.values_by_name[common_base + symbol].number
            except KeyError:
                raise ValueError('No such symbol in enumeration %s: %s'%
                                 (descriptor.name, symbol))

    @classmethod
    def largest_common_base(cls, names):
        divided_names = [name.split("_") for name in names]

        try:
            common_base = divided_names.pop(0)
        except IndexError:
            common_base = []


        cutoffs = [len(common_base)]
        for divided_name in divided_names:
            for index, element in enumerate(common_base):
                try:
                    candidate = divided_name[index]
                except IndexError:
                    break
                else:
                    if candidate != element:
                        cutoffs.append(index)

        return "".join([(word + "_") for word in common_base[:min(cutoffs)]])


    message_encoders = {
        wellknown.Empty: lambda v: wellknown.Empty(),
        wellknown.BoolValue: lambda v: wellknown.BoolValue(value=v),
        wellknown.StringValue: lambda v: wellknown.StringValue(value=v),
        wellknown.DoubleValue: lambda v: wellknown.DoubleValue(value=v),
        wellknown.FloatValue: lambda v: wellknown.FloatValue(value=v),
        wellknown.Int64Value: lambda v: wellknown.Int64Value(value=v),
        wellknown.Int32Value: lambda v: wellknown.Int32Value(value=v),
        wellknown.UInt64Value: lambda v: wellknown.UInt64Value(value=v),
        wellknown.UInt32Value: lambda v: wellknown.UInt32Value(value=v),
        wellknown.BytesValue: lambda v: wellknown.BytesValue(value=v),
        wellknown.Timestamp: wellknown.encodeTimestamp,
        wellknown.Duration: wellknown.encodeDuration,
        wellknown.Value: wellknown.encodeValue,
        wellknown.Struct: wellknown.encodeStruct,
        wellknown.ListValue: wellknown.encodeListValue,
        datetime.TimeStruct: datetime.encodeTimeStruct,
        datetime.Interval: datetime.encodeInterval,
        variant.Value: variant.encodeValue,
        variant.ValueList: variant.encodeValueList,
        variant.TaggedValue: variant.encodeTaggedValue,
        variant.TaggedValueList: variant.encodeTaggedValueList,
        variant.KeyValueMap: variant.encodeKeyValueMap,
    }

    enum_encoders = {
        status.Level: status.encodeLogLevel,
    }

