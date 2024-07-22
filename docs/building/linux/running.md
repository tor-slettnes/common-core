
Installing and Running
======================

Here we cover how to install and run Debian package(s) as well as the Python wheel(s) we created in the previous section: [Building](building.md).

We will primarily cover the included [DEMO](../../../src/mantle/demo/) reference application and its associated `Demo` service. However, these steps will be similar for other microservice applications, both within this repository (for instance, [Platform Server](../../../src/mantle/platform) and for other applications that are built on this framework (e.g. using [common-core](../../..) as a submodule within their own repositories).


## Installing the Debian package(s)

### Within your host environment

The generated packages go into the folder `out/packages` (this folder is created on demand). To install them onto your own system, invoke:

  ```shell
  sudo dpkg --install out/packages/common-core-*.deb
  ```

### Onto another Debian system

To install these packages onto another Debian system, some runtime libraries may be needed. The simplest way to ensure those requirements are met is to use the `apt` tool rather than `dpkg` directly.

After copying the `.deb` file(s) from `out/packages` onto the desired target system, invoke the following command on that system (assuming that the packages were copied to `$HOME/Downloads`):

  ```shell
  sudo apt -y install $HOME/Downloads/common-core-*.deb
  ```

(Note that the leading `./` is required for the `apt` tool to recognize this as a local filename, rather than a package name to be retrieved from your Debian/Ubuntu repository).


## Launching service daemons

By default you do not need to do anything, as the installed service daemons are launched automatically as system services.  For instance, `demoserver` should now be listening on TCP port `3366` (`D-E-M-O` on a telephone keypad).

If you prefer to run it interactively, for instance to see log messages printed to your terminal, run the following:

  ```shell
  sudo systemctl stop demoserver      # Stop the service
  sudo systemctl disable demoserver   # Keep it from automatically launching at boot
  /usr/sbin/demoserver                # Launch Edge in the foreground
  ```

**Tip:** Use the `--help` option to view available command-line options:

  ```shell
  /usr/sbin/demoserver --help
  ```

If you later wish to re-enable the system service, use the following:

  ```shell
  sudo systemctl enable demoserver
  sudo systemctl start demoserver
  ```

## Interacting with services

### Default Service Ports

gRPC and ZMQ services each have an associated default service port, defined in corresponding service  blocks within `/usr/share/cc/settings/grpc-endpoints-*.json` and `/usr/share/cc/settings/zmq-endpoints-*.json`.  For instance:

* The **Demo** gRPC service is available on TCP port **3366** (**D-E-M-O**) by default
* The **Demo** ZMQ responder is listening on TCP port **3362** (**D-E-M**-O **C**ommand)
* The **Demo** ZMQ publisher is listening on TCP port **3367** (**D-E-M**-O **P**ublisher)
* The **System**, **Network**, **Upgrade**, and **File** gRPC services are all available on TCP port **7528** (**P-L-A-T**), as they are all hosted within the `platformserver` daemon.

[DDS](https://www.omg.org/omg-dds-portal/) uses multicast networking and does not rely on specific host names or port numbers.  Instead, communication is organized in DDS *domains*, and traffic within each domain is organized by Pub/Sub topic


### Command Line Tools

Microservice applications in this repository all come with associated command line clients written in C++. These are specific to each *service* rather than the server daemon in which they happen to be hosted; for instance, [system-tool](src/mantle/platform/cpp/apps/system-tool/), [network-tool](src/mantle/platform/cpp/apps/network-tool/) and [upgrade-tool](src/mantle/platform/cpp/apps/upgrade-tool/) all communicate with the corresponding gRPC services, which happen to be hosted within `platformserver`.

As for the DEMO example, there is only a `Demo` service; however there are three associated command-line tools, each corresponding to a different messaging interface:
* [demo-grpc-tool](../../../src/mantle/demo/cpp/utils/grpc-tool/)
* [demo-dds-tool](../../../src/mantle/demo/cpp/utils/dds-tool/)
* [demo-zmq-tool](../../../src/mantle/demo/cpp/utils/zmq-tool/)

#### Communicating with remote services

By default each these tools communicate with their corresponding service daemon on `localhost`.  If you wish to connect to a remote server instance, use the `--host` option:

  ```shell
  demo-grpc-tool --host=ADDRESS COMMAND ...
  ```
Replace `ADDRESS` with one of the following:

 * A resolvable name, e.g. `servername.local`
 * An IPv4 ("IP") address in "dotted quad" format, e.g. `192.168.0.1`
 * An IPv6 address enclosed in square brackets, e.g. `[fe80::dead:beef:bad:f00d]`

Use the `--help` option with any of these commands for detailed usage.

#### Invoking synchronous RPC methods

Each client tool accepts a number of service-specific commands, corresponding roughly to the available service methods. Use `--help commands` to get a brief description of available commands and arguments.  For instance, each of the `demo-*-tool` commands accepts a `say_hello` command:

  ```bash
  demo-grpc-tool say_hello "I am here"
  ```

#### Monitoring events

Each tool has a `monitor` command to passively listen for asynchronous events ("signals") from the corresponding service.  For instance, to listen for events streamed back from the `Demo` gRPC service, use

  ```bash
  demo-grpc-tool monitor
  ```

By default, *all* events emitted by the service are printed on standard output.  To filter for specific event names, enumerate those after the `monitor` command, e.g.:

  ```bash
  demo-grpc-monitor greetings
  ```

Again, use `--help commands` to see a list of possible subcommands.


### Python client modules

#### Client module

Python gRPC client modules are installed under the standard Python 3.x module folder, `/usr/lib/python3/dist-packages`. These contain wrapper methods for the above service functions. Specifically for the `Demo` service:

* `cc/demo/grpc/client.py`: client module for the `Demo` gRPC service
* `cc/demo/grpc/service.py`: service (request handler) module for the `Demo` gRPC service

* `cc/demo/zmq/client.py`: client module for the `Demo` ZMQ interface
* `cc/demo/zmq/requesthandler.py`: request handler module for the `Demo` ZMQ interface
* `cc/demo/zmq/server.py`: ZMQ server module


These modules require Python gRPC and/or ZMQ as well as Protocol Buffers bindings to be available on your system.  To install these, run the following:

  ```bash
  sudo apt install python3-grpcio python3-zmq python3-protobuf
  ```

#### DEMO Shell

An interative Python shell with preloaded gRPC modules can be launched using the `demoshell` command:

  ```shell
  demoshell
  ```

You should see a brief legend with an interative Python prompt:

  ```
  Interactive Service Control.  Subsystems loaded:

          demo_grpc - Simple gRPC communications example
          demo_zmq - Simple 0MQ communications example

      ProtoBuf types are generally loaded into namespaces matching the package
      names from their respective `.proto` files:

          google.protobuf - Well-known types from Google
          cc.* - Various custom types
          cc.protobuf.* - General utilities and wrapper modules

      Use 'help(subsystem)' to list available subcomponents or methods

  >>>
  ```

As you might surmise, the `demo_grpc` and `demo_zmq` objects are instances of `cc.demo.grpc.DemoClient()` and `cc.demo.zmq.DemoClient()`, respectively.  Within each you will find corresponding service methods.

Additionally:

* Generated ProtoBuf data types are generally available within namespaces corresponding to the package names from their respetive `.proto` files: `google.protobuf.*` and `cc.*`.

* Various ProtoBuf utility wrappers are available within the namespace `cc.protobuf`. For example,
  - `cc.protobuf.variant.decodeValue()` and `cc.protobuf.variant.encodeValue()` translate between a `cc.variant.Value()` instance and a native Python value, whereas
  - `cc.protobuf.wellknown.decodeTimestamp()` and `cc.protobuf.wellknown.encodeTimestamp()` do the same for a `google.protobuf.Timestamp()` value.
  - `cc.protobuf.utils.print_message()` prints a formatted version of a ProtoBuf message on standard output.


> **Tip**: Use Python's `help()` function for documentation and calling syntax.  For instance,
>  - use `help(demo_grpc)` to get a list of available methods in the `DemoClient()` instance, and
>  - `help(cc.demo.Greeting)` to get describe the generated `Greeting()` message from `demo.proto`, including its field names.


Some examples:

* The method `demo_grpc.hello()` is a Python wrapper method that in turn calls the `say_hello()` gRPC method. To invoke it, use:

  ```python
  demo_grpc.hello("Hi there")
  '''

* To monitor events, use `demo_grpc.start_notify_signals(CALLBACK)`, where `CALLBACK` is a callback function which will receive `cc.demo.Signal()` ProtoBuf messages as its single argument.  For instance, to format and print these ProtoBuf messages in your terminal, use:

  ```python
  demo_grpc.start_notify_signals(cc.protobuf.utils.print_messsage)
  ```

> **Tip**: Use Pythons's interactive command line completion to list available completions at various points.  For instance, once you have typed `demo_grpc.start_notify_`, press **[TAB]** twice to show a list of possible method completions starting with `start_notify_`.


### <a name="wheel">Python package ("Wheel")</a>

The Python client modules are also available in an installable [Python wheel](https://pythonwheels.com/), which allows them to be used in a Python 3.9 or higher environment on any platform.

You have two options for invoking modules within this wheel.

#### Install the wheel with PIP

You can use the [Package Installer for Python](https://pypi.org/project/pip/) (PIP) to install this wheel, preferably into a [Python Virtual Environment](https://virtualenv.pypa.io/en/latest/user_guide.html).  The steps vary slightly based on your host OS; on a Debian-based system, follow these steps:

1. Install required Python modules:

   ```bash
   sudo apt install python3-virtualenv python3-pip
   ```

2. Create a virtual environment.  For this example we'll do so in `$HOME/cc`:

   ```bash
   virtualenv $HOME/cc
   ```

3. "Activate" this environment. This effectively adjusts your `$PATH` environment variable so that commands like `python` and `pip` are launched from within this environment instead of your system folders:

   ```bash
   $HOME/cc/bin/activate
   ```

   You will need to repeat this command whenever you want to use this environment from a new shell (i.e. Terminal window).

4. Install the wheel within this newly activated environment (replacing its path as appropriate):

   ```bash
   pip install $HOME/Downloads/common_core-VERSION-py3-none-any.whl
   ```

5. Launch the `demoshell` interactive prompt from within this environment:

   ```bash
   python -i -m cc.demo.shell
   ```

   If you wish to connect to a DEMO server running on another host, use:

   ```bash
   python -i -m cc.demo.shell --host=ADDRESS
   ```

   (Replace `ADDRESS` with the resolvable name or IP address of the remote host).


#### Launch modules directly from wheel

Alternatively, you can load modules from the wheel without installing it.  To do so, launch your OS native Python interpreter but with the environment variable `PYTHONPATH` pointing to the `.whl` file (again, modifying the path and the optional `--host` argument as appropriate):

   ```bash
   PYTHONPATH=$HOME/Downloads/common_core-VERSION-py3-none-any.whl \
      python3 -i -m cc.demo.shell --host=ADDRESS
   ```

or

   ```bash
   export PYTHONPATH=$HOME/Downloads/common_core-VERSION-py3-none-any.whl

   python3 -i -m cc.demo.shell --host=ADDRESS
   ```


### Web UI

The gRPC server applications in this repository are built with support for [gRPC reflection](https://grpc.io/docs/guides/reflection/), and is therefore accessible with tools such as [gRPC-UI](https://www.fullstory.com/blog/grpcui-dont-grpc-without-it/).

Follow the instructions the [gRPC-UI GitHub page](https://github.com/fullstorydev/grpcui) to install this tool, then point it to the DEMO sever it as follows:

  ```bash
  $HOME/go/bin/grpcui -plaintext localhost:3366
  ```

This will bring an interactive gRPC request builder in your default web browser.

**Caveat:** gRPC-UI does not seem to handle continuous server streams, so is not a suitable tool for the `watch()` method.


#### Example: Send a greeting.

To greet your server:

* From the drop-down list near the top, select `cc.demo.Demo`
* Among the available methods just under this service name, choose `say_hello`
* Check `text [ ]` and type in a text message
* Check `identity [ ]` and type in a identity for this client instance, e.g. "`interactive`"
* Check `implementation [ ]` and type in a client implementation name, e.g. "`gRPC UI`"
* Check `birth [ ]` and then choose date/time representing the birth of this client
* Check `data [ ]` and build some random key/value pairs you wish to include.


## Other services

As mentioned in the beginning the text in this step dealt primarily with the `DEMO` application example.  However most of it is also applicable to other services, including those offered by the `platformserver` within this repository.  For example, there's also a `platformshell` launcher that brings up an interactive Python prompt to interact with those services. 


## Next steps

You have now completed this Common Core *Getting Started* guide.  At this point you may want to learn a bit more about the code itself: design principles, theory of operation, and so on. A good starting point for this is the [DEMO](../../../src/mantle/demo/) application and its associated [documentation](../../../src/mantle/demo/README.md).

