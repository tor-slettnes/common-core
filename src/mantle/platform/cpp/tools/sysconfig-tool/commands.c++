// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief SysConfig service control tool - command implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "sysconfig.h++"
#include "protobuf-message.h++"
#include "string/format.h++"

void Options::add_commands()
{
    this->add_command(
        "get_product_info",
        {},
        "Get general information about the installed product",
        std::bind(&Options::get_product_info, this));

    this->add_command(
        "set_serial_number",
        {},
        "Set the unique serial number for this system. Intended for manufacturing use.",
        std::bind(&Options::set_serial_number, this));

    this->add_command(
        "set_model_name",
        {},
        "Set the model name for this system. Intended for manufacturing use.",
        std::bind(&Options::set_model_name, this));

    this->add_command(
        "get_time",
        {"[local|utc|epoch]"},
        "Return the current time. The options `local`, `utc`, and `epoch` "
        "determine the output format  as, respectively:yyyy-mm-ddTHH:MM:SS, "
        "yyyy-mm-ddTHH:MM:SSZ, or an integer represening seconds since UNIX epoch.",
        std::bind(&Options::get_time, this));

    this->add_command(
        "set_time",
        {"TIMESTAMP"},
        "Set the system time to TIMESTAMP, interpreted as either seconds since "
        "UNIX epoch or a JavaScript compatible string: yyyy-mm-ddTHH:MM:SS for "
        "local time, or yyyy-mm-ddTHH:MM:SSZ for UTC",
        std::bind(&Options::set_time, this));

    this->add_command(
        "get_ntp",
        {},
        "Indicate whether automatic time upates via NTP are currently enabled.",
        std::bind(&Options::get_ntp, this));

    this->add_command(
        "set_ntp",
        {"{off|on}", "[SERVER]", "..."},
        "Turn automatic time updates via NTP off or on.",
        std::bind(&Options::set_ntp, this));

    this->add_command(
        "list_timezone_areas",
        {},
        "List top-level time zone areas",
        std::bind(&Options::list_timezone_areas, this));

    this->add_command(
        "list_timezone_countries",
        {"[AREA]"},
        "List countries, optionally within a specific timezone area.",
        std::bind(&Options::list_timezone_countries, this));

    this->add_command(
        "list_timezone_regions",
        {"COUNTRY", "[AREA]"},
        "List zones within a country, optionally also restricted to a specific area.",
        std::bind(&Options::list_timezone_regions, this));

    this->add_command(
        "list_timezone_specs",
        {"[AREA]", "[COUNTRY]"},
        "List canoncial zone specifications, "
        "optionally within a specific timezone area and/or country.",
        std::bind(&Options::list_timezone_specs, this));

    this->add_command(
        "get_timezone_spec",
        {"[ZONE]"},
        "Get specifications for the specified canonical ZONE. "
        "If no zone is provided, get specifications for the currently configured zone.",
        std::bind(&Options::get_timezone_spec, this));

    this->add_command(
        "set_timezone_by_name",
        {"ZONE"},
        "Set the system timezone to the the specified ZONE.",
        std::bind(&Options::set_timezone_by_name, this));

    this->add_command(
        "set_timezone_by_location",
        {"COUNTRY", "[REGION]"},
        "Set the system timezone according to the specified country, "
        "and if applicable, region.  COUNTRY should be specified either "
        "by its ISO 3166 code (e.g. \"US\") or its name in English "
        "(e.g., \"United States\").  REGION should be included if and "
        "only if the specified country has more than one time zone.",
        std::bind(&Options::set_timezone_by_location, this));

    this->add_command(
        "get_timezone_info",
        {"[ZONE]", "[TIMESTAMP]"},
        "Get offset information for the specified canonical zone. "
        "If no canonical zone name is provided, get information "
        "for the current effective zone.",
        std::bind(&Options::get_timezone_info, this));

    this->add_command(
        "get_host_info",
        {},
        "Get general information about the host system",
        std::bind(&Options::get_host_info, this));

    this->add_command(
        "set_host_name",
        {"NAME"},
        "Configure the hostname.",
        std::bind(&Options::set_host_name, this));

    this->add_command(
        "invoke",
        {"COMMAND", "..."},
        "Invoke a command and wait for its completion. "
        "Its exit status is passed on to the caller (see the '--status' option).",
        std::bind(&Options::invoke_sync, this));

    this->add_command(
        "invoke_async",
        {"COMMAND", "..."},
        "Invoke a command asynchronously. and print out its process ID (PID)."
        "This PID can subsequently be used to wait for its completion with 'invoke_finish'.",
        std::bind(&Options::invoke_async, this));

    this->add_command(
        "invoke_finish",
        {"PID"},
        "Wait for a previously invoked asynchronous command to finish."
        "Its exit status is passed on to the caller (see the '--status' option).",
        std::bind(&Options::invoke_finish, this));

    this->add_command(
        "reboot",
        {},
        "Reboot the system",
        std::bind(&Options::reboot, this));

    this->add_command(
        "monitor",
        {"[except]", "[time|time_config|tz_info|tz_config|host_info]", "..."},
        "Stay alive and monitor events. Unless specified, all events are printed.",
        std::bind(&Options::monitor, this));
}

void Options::get_time()
{
    FlagMap flags;
    bool &local = flags["local"];
    bool &utc = flags["utc"];
    bool &epoch = flags["epoch"];
    this->get_flags(&flags);

    std::string format =
        epoch ? "%.3f"
        : utc ? "%.0Z"
              : "%.0T";

    core::str::format(
        std::cout,
        format + "\n",
        sysconfig::time->get_current_time());
}

void Options::set_time()
{
    auto value = core::types::Value::from_literal(this->get_arg("timestamp"));
    core::dt::TimePoint tp = value.as_timepoint();
    core::str::format(std::cout, "Setting timepoint: %.3Z\n", tp);
    sysconfig::time->set_current_time(tp);
}

void Options::set_ntp()
{
    bool enable = core::str::convert_to<bool>(this->get_arg("ON or OFF"));
    sysconfig::TimeConfig config = {
        .synchronization = (enable ? sysconfig::TSYNC_NTP
                                   : sysconfig::TSYNC_NONE),
    };

    sysconfig::time->set_time_config(config);
}

void Options::get_ntp()
{
    this->report_status_and_exit(
        sysconfig::time->get_time_config().synchronization == sysconfig::TSYNC_NTP);

    // this->report_status_and_exit(sysconfig::time->get_ntp());
}

void Options::list_timezone_areas()
{
    for (const sysconfig::TimeZoneArea &area :
         sysconfig::timezone->list_timezone_areas())
    {
        std::cout << area << std::endl;
    }
}

void Options::list_timezone_countries()
{
    std::string area = this->next_arg().value_or("");
    for (const sysconfig::TimeZoneCountry &country :
         sysconfig::timezone->list_timezone_countries(area))
    {
        std::cout << country << std::endl;
    }
}

void Options::list_timezone_regions()
{
    std::string country = this->get_arg("country");
    std::string area = this->next_arg().value_or("");

    sysconfig::TimeZoneLocationFilter filter = {
        .area = area,
        .country = {
            .code = (country.size() == 2) ? country : "",
            .name = (country.size() != 2) ? country : "",
        },
    };

    for (const sysconfig::TimeZoneRegion &region :
         sysconfig::timezone->list_timezone_regions(filter))
    {
        std::cout << region << std::endl;
    }
}

void Options::list_timezone_specs()
{
    std::string area = this->next_arg().value_or("");
    std::string country = this->next_arg().value_or("");

    sysconfig::TimeZoneLocationFilter filter = {
        .area = area,
        .country = {
            .code = (country.size() == 2) ? country : "",
            .name = (country.size() != 2) ? country : "",
        },
    };

    for (const sysconfig::TimeZoneCanonicalSpec &spec :
         sysconfig::timezone->list_timezone_specs(filter))
    {
        std::cout << spec << std::endl;
    }
}

void Options::get_timezone_spec()
{
    std::string zonename = this->next_arg().value_or("");
    std::cout << sysconfig::timezone->get_timezone_spec(zonename)
              << std::endl;
}

void Options::set_timezone_by_name()
{
    std::string zonename = this->get_arg("time zone");

    sysconfig::TimeZoneInfo result =
        sysconfig::timezone->set_timezone(zonename);

    std::cout << result << std::endl;
}

void Options::set_timezone_by_location()
{
    std::string country = this->get_arg("country");
    std::string region = this->next_arg().value_or("");

    sysconfig::TimeZoneLocation location = {
        .country = {
            .code = country.size() == 2 ? country : "",
            .name = country.size() != 2 ? country : "",
        },
        .region = region,
    };

    sysconfig::TimeZoneInfo result =
        sysconfig::timezone->set_timezone(location);

    std::cout << result << std::endl;
}

void Options::get_timezone_info()
{
    std::string zonename = this->next_arg().value_or("");
    core::dt::TimePoint tp;

    if (auto timestamp = this->next_arg())
    {
        auto value = core::types::Value::from_literal(timestamp.value());
        tp = value.as_timepoint();
    }

    std::cout << sysconfig::timezone->get_timezone_info(zonename, tp)
              << std::endl;
}

void Options::get_host_info()
{
    std::cout << sysconfig::host->get_host_info()
              << std::endl;
}

void Options::set_host_name()
{
    std::string hostname = this->get_arg("NAME");
    sysconfig::host->set_host_name(hostname);
}

void Options::get_product_info()
{
    std::cout << sysconfig::product->get_product_info()
              << std::endl;
}
void Options::set_serial_number()
{
    std::string serial = this->get_arg("SERIAL_NUMBER");
    sysconfig::product->set_serial_number(serial);
}

void Options::set_model_name()
{
    std::string model = this->get_arg("MODEL_NAME");
    sysconfig::product->set_model_name(model);
}

void Options::invoke_sync()
{
    std::string command = this->get_arg("COMMAND");

    core::platform::InvocationResult result = sysconfig::process->invoke_sync(
        {.argv = this->args},  // invocation
        {});                   // input

    if (result.stdout_size())
    {
        std::cout << result.stdout->rdbuf();
    }

    if (result.stderr_size())
    {
        std::cerr << result.stderr->rdbuf();
    }

    this->report_status_and_exit(result.error_code());
}

void Options::invoke_async()
{
    std::string command = this->get_arg("COMMAND");
    core::platform::PID pid = sysconfig::process->invoke_async(
        {.argv = this->args},  // invocation
        {});                   // input

    std::cout << pid << std::endl;
}

void Options::invoke_finish()
{
    auto pid = core::str::convert_to<core::platform::PID>(this->get_arg("PID"));

    core::platform::InvocationResult result = sysconfig::process->invoke_finish(
        pid,
        {});

    if (result.stdout_size())
    {
        std::cout << result.stdout->rdbuf();
    }

    if (result.stderr_size())
    {
        std::cerr << result.stderr->rdbuf();
    }

    this->report_status_and_exit(result.error_code());
}

void Options::reboot()
{
    sysconfig::host->reboot();
}
