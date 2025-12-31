// -*- c++ -*-
//==============================================================================
/// @file windows-service.h++
/// @brief Windows service hooks
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/systemservice.h++"
#include "string/misc.h++"

#include <windows.h>

#include <string>

namespace core::platform
{
    class WindowsServiceProvider : public SystemServiceProvider
    {
        using This = WindowsServiceProvider;
        using Super = SystemServiceProvider;

    public:
        WindowsServiceProvider();

        void initialize() override;
        void deinitialize() override;

        void install() override;
        void uninstall() override;

        void start() override;
        void stop() override;

    private:
        static VOID WINAPI ServiceStart(DWORD argc, LPTSTR *argv);
        static VOID WINAPI ServiceStop();
        static VOID WINAPI ServiceCtrlHandler(DWORD);
        static VOID WINAPI ReportServiceStatus(DWORD current_state,
                                               DWORD win32_exit_code,
                                               DWORD wait_hint,
                                               DWORD controls_accepted,
                                               DWORD checkpoint);

    private:
        static SERVICE_STATUS service_status;
        static SERVICE_STATUS_HANDLE service_status_handle;
    };
}  // namespace core::platform
