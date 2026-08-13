/// -*- c++ -*-
//==============================================================================
/// @file domain.c++
/// @brief Event domain
/// @author Tor Slettnes
//==============================================================================

#include "domain.h++"

namespace cc::core::status
{
    /// Names for printing.
    const types::SymbolMap<Domain> domain_names = {
        {Domain::NONE, "NONE"},
        {Domain::APPLICATION, "APPLICATION"},
        {Domain::SYSTEM, "SYSTEM"},
        {Domain::PROCESS, "PROCESS"},
        {Domain::DEVICE, "DEVICE"},
        {Domain::SERVICE, "SERVICE"},
    };

    std::ostream &operator<<(std::ostream &stream, Domain domain)
    {
        try
        {
            stream << domain_names.at(domain);
        }
        catch (const std::out_of_range &)
        {
            stream << "UNKNOWN_" << static_cast<uint>(domain);
        }
        return stream;
    }

    std::istream &operator>>(std::istream &stream, Domain &domain)
    {
        return domain_names.from_stream(stream, &domain, Domain::NONE, false);
    }

}  // namespace cc::core::status
