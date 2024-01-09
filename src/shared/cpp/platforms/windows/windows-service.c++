// -*- c++ -*-
//==============================================================================
/// @file windows-service.c++
/// @brief Windows service hooks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-service.h++"
#include "platform/symbols.h++"
#include "status/exceptions.h++"
#include "../application/init.h++"

#include <csignal>

#pragma comment(lib, "advapi32.lib")

namespace cc::platform
{
    WindowsServiceProvider::WindowsServiceProvider()
        : Super(TYPE_NAME_FULL(This), path->exec_name())
    {
    }

    void WindowsServiceProvider::initialize()
    {
        Super::initialize();
    }

    void WindowsServiceProvider::deinitialize()
    {
        Super::deinitialize();
    }

    void WindowsServiceProvider::install()
    {
        fs::path exec_path(path->exec_path());

        /// Get a handle to the SCM database
        SC_HANDLE sc_manager_handle = OpenSCManager(
            NULL,                    // local computer
            NULL,                    // ServicesActive database
            SC_MANAGER_ALL_ACCESS);  // full service rights

        if (!sc_manager_handle)
        {
            throw exception::SystemError(
                "OpenSCManager",
                GetLastError());
        }

        SC_HANDLE sc_service_handle = CreateService(
            sc_manager_handle,                  // SCM database
            this->service_name().data(),        // name of service
            this->service_name().data(),        // service name to display
            SERVICE_ALL_ACCESS,                 // desired access
            SERVICE_WIN32_OWN_PROCESS,          // service type
            SERVICE_DEMAND_START,               // start type
            SERVICE_ERROR_NORMAL,               // error control type
            path->exec_path().string().data(),  // path to binary
            NULL,                               // no load ordering group
            NULL,                               // no tag identifier
            NULL,                               // no dependencies
            NULL,                               // LocalStyem account
            NULL);                              // no password

        if (!sc_service_handle)
        {
            CloseServiceHandle(sc_manager_handle);
            throw exception::SystemError(
                "CreateService",
                GetLastError());
        }

        CloseServiceHandle(sc_service_handle);
        CloseServiceHandle(sc_manager_handle);
    }

    void WindowsServiceProvider::uninstall()
    {
    }

    void WindowsServiceProvider::start()
    {
        SERVICE_TABLE_ENTRY dispatch_table[] = {
            {const_cast<LPSTR>(path->exec_name().data()), (LPSERVICE_MAIN_FUNCTION)This::ServiceStart},
            {NULL, NULL},
        };

        if (!StartServiceCtrlDispatcher(dispatch_table))
        {
            throw exception::SystemError(
                "StartServiceCtrlDispatcher",
                GetLastError());
        }
    }

    void WindowsServiceProvider::stop()
    {
        This::ServiceStop();
    }

    VOID WINAPI WindowsServiceProvider::ServiceStart(DWORD argc, LPTSTR *argv)
    {
        // Register our service control handler with the SCM
        This::service_status_handle = RegisterServiceCtrlHandler(
            argc ? argv[0] : "",
            This::ServiceCtrlHandler);

        if (!service_status_handle)
        {
            throw exception::SystemError("RegisterServiceCtrlHandler",
                                         GetLastError());
        }

        // Tell the service controller we are starting
        ZeroMemory(&service_status, sizeof(service_status));
        This::service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        This::service_status.dwControlsAccepted = 0;

        // Report initial status to the SCM
        This::ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000, 0, 1);

        // Report running status when initialization is complete.
        This::ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0, SERVICE_STOP, 2);
    }

    VOID WINAPI WindowsServiceProvider::ServiceStop()
    {
        This::ReportServiceStatus(SERVICE_STOPPED, 0, 0, 0, 3);
    }

    VOID WINAPI WindowsServiceProvider::ServiceCtrlHandler(DWORD ctrl_code)
    {
        switch (ctrl_code)
        {
        case SERVICE_CONTROL_STOP:
            This::ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 0, 4);
            cc::application::signal_shutdown.emit(SIGTERM);
            This::ReportServiceStatus(
                This::service_status.dwCurrentState, NO_ERROR, 0, 0, 5);
            break;

        default:
            break;
        }
    }

    VOID WINAPI WindowsServiceProvider::ReportServiceStatus(DWORD current_state,
                                                            DWORD win32_exit_code,
                                                            DWORD wait_hint,
                                                            DWORD controls_accepted,
                                                            DWORD checkpoint)
    {
        // Fill in the SERVICE_STATUS structure.
        This::service_status.dwCurrentState = current_state;
        This::service_status.dwWin32ExitCode = win32_exit_code;
        This::service_status.dwWaitHint = wait_hint;
        This::service_status.dwControlsAccepted = controls_accepted;
        This::service_status.dwCheckPoint = checkpoint;

        // Report the status of the service to the SCM.
        if (!SetServiceStatus(This::service_status_handle, &This::service_status))
        {
            OutputDebugString(
                TEXT("SetServiceStatus returned error"));
        }
    }

    SERVICE_STATUS WindowsServiceProvider::service_status = {SERVICE_WIN32_OWN_PROCESS};
    SERVICE_STATUS_HANDLE WindowsServiceProvider::service_status_handle = nullptr;

}  // namespace cc::platform
