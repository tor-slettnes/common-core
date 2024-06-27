Common Core Apps Example - Command-Line Utility
===============================================

The `demo-dds-tool` executable provides end-user access to our demo API. It incorporates both of the [API implementations](../../impl/README.md) from our demo by linking in the following libraries:

* [cc_demo_native](../../impl/native/README.md), the core library
* [cc_demo_dds_subscriber](../../impl/dds/dds-subscriber), containing the DDS subscriber
* [cc_demo_dds_client](../../impl/dds/rpc-client), containing the RPC client


How to use
----------

### Synopsis:

The basic syntax for invoking the tool is

```shell
$ demo-dds-tool COMMAND [ARGS ..]
```

where: `COMMAND` is one of the following:

   - `say_hello TEXT` sends sends a greeting to the server and/or our peers. `TEXT` is a required argument.

   - `get_current_time` returns current time data (epoch, local time, utc time)

   - `monitor` keeps the utility open to monitor events from the server and print them back to the stdout as they arrive. By default both event types are included, which will print at least one event per second; append either `time` or `greetings` to monitor just one or the other.

But before we can launch the tool, we need to launch the appropriate service daemons so that we have someone to talk to[^1]. Please refer to the [daemon/README.md](../../daemon/README.md) file for further information on this.

[^1]: The first of these implementations is of course self-contained, so if you choose this one there is no service daemon involved. However, in this case you also won't be able to send//receive greetings from peers.


### Example:

It's time to try using tool now.

 * In a dedicated terminal tab/window, monitor greetings from others via `DemoService` by invoking:

   ```shell
   $ demo-dds-tool monitor greetings

   ### Listening for updates. Press ENTER to end. ###

   ```

 * Open a new terminal window and send a greeting via the Pub/Sub backend:

   ```shell
   $ demo-dds-tool say_hello "I am here!"
   ```

   In the first terminal you should now see a message similar to this:

   ```shell
   Greeting(text="I am here!", identity="demo-dds-tool", implementation="dds", birth=birth=2023-05-09@00:41:41.029, uptime=0.000 sec)
   ```

### That's it!

