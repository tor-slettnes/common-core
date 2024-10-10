#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file __init__.py
## @brief Initialization for generated gRPC code
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### The generated ProtoBuf/gRPC bindings are not aware of our Python namespace,
### so let's add this folder to the module search path.

import sys, os.path

this_dir = os.path.dirname(__file__)
sys.path.insert(0, this_dir)
