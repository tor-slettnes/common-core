## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Demo application interface - ZeroMQ implementation
## @author Tor Slettnes
#===============================================================================

### Common declarations
add_subdirectory(common)

### Pub/Sub interface
add_subdirectory(zmq-publisher)
add_subdirectory(zmq-subscriber)

### Client/Server interface
add_subdirectory(zmq-client)
add_subdirectory(zmq-server)

