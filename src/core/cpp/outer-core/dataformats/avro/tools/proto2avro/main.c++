/// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Proto2Avro main
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-protobufschema.h++"
#include "status.pb.h"
#include "application/init.h++"  // Common init routines
#include "string/format.h++"
#include "argparse/common.h++"

int main (int argc, char **argv)
{
    // Initialize paths, load settings, set up shutdown signal handlers
    core::application::initialize(argc, argv);

    auto options = std::make_unique<core::argparse::CommonOptions>();
    options->apply(argc, argv);

    const google::protobuf::Descriptor *desc = cc::status::Error::GetDescriptor();
    std::cout << avro::schema_from_proto(desc)
              << std::endl;
    return 0;
}
