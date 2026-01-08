## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_pubsub_python")

cc_add_python("${TARGET}"
  DIRECTORIES .
  NAMESPACE_COMPONENT "platform.pubsub"
  PYTHON_DEPS "cc_core_python"
  PROTO_DEPS "cc_pubsub_proto"
  INSTALL_CONDITION INSTALL_PYTHON_MODULES
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_PUBSUB)
  cc_add_python_wheel("pubsub_client"
    PYTHON_DEPS "${TARGET}"
    WHEEL_DEPS "core"
    DESCRIPTION "Python clients for the Pub/Sub Relay service"
    INSTALL_CONDITION INSTALL_PYTHON_WHEELS
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
