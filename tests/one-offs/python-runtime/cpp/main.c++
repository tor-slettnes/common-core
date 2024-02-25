// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief C++ demo - main application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"  // Common init routines
#include "argparse/common.h++"   // Command-line options
#include "status/exceptions.h++" // Custom exception support
#include "python-runtime.h++"
#include "python-pickle.h++"

void run_tests()
{
    shared::python::SimpleObject real90 =
        shared::python::SimpleObject::pyobj_from_value(90.0);

    shared::python::ContainerObject sin90 =
        shared::python::runtime->call(
            "math",
            "sin",
            shared::python::SimpleObject::Vector({real90}));

    shared::types::ByteVector pickle = shared::python::pickle(sin90);

    logf_notice("Python value=%r, pickle=%r", sin90, pickle);

    shared::python::ContainerObject unpickle = shared::python::unpickle(pickle);
    logf_notice("Python pickle=%r, unpickled=%r", pickle, unpickle);
}

int main(int argc, char** argv)
{
    // Initialize paths, load settings, set up shutdown signal handlers
    shared::application::initialize(argc, argv);

    auto options = std::make_unique<shared::argparse::CommonOptions>(false);
    options->apply(argc, argv);

    try
    {
        run_tests();
    }
    catch (const std::exception& e)
    {
        log_error(e);
    }

    logf_debug("Deinitializing Example application");
    shared::application::deinitialize();
    return 0;
}
