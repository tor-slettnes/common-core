Common Core Demo App - C++ code
===============================

Our Demo applications's C++ code is organized into these distinct compoonents:

* [An abstract Application Programming Interface](api/) (API), serving as the contract between each provider library and the downstream executable(s).

* [Implementations](impl/) of this API provided as linkable (standalone/client/server) libraries.

* [demoserver](daemon/), a server executable (daemon) that serves requests from/notifications to cilents (such as `demo-tool`)

* [tools](tools/), command-line tools that incorporates each of the above implementation libraries to provide access to the above API.
