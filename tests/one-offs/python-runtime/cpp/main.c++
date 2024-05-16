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
    cc::python::SimpleObject real90 =
        cc::python::SimpleObject::pyobj_from_value(90.0);

    cc::python::ContainerObject sin90 =
        cc::python::runtime->call(
            "math",
            "sin",
            cc::python::SimpleObject::Vector({real90}));

    cc::types::ByteVector pickle = cc::python::pickle(sin90);

    logf_notice("Python value=%r, pickle=%r", sin90, pickle);

    cc::python::ContainerObject unpickle = cc::python::unpickle(pickle);
    logf_notice("Python pickle=%r, unpickled=%r", pickle, unpickle);
}

int main(int argc, char** argv)
{
    // Initialize paths, load settings, set up shutdown signal handlers
    cc::application::initialize(argc, argv);

    auto options = std::make_unique<cc::argparse::CommonOptions>(false);
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
    cc::application::deinitialize();
    return 0;
}
