/// -*- c++ -*-
//==============================================================================
/// @file domain.c++
/// @brief Event domain
/// @author Tor Slettnes
//==============================================================================

#include "domain.h++"

namespace core::status
{
    /// Names for printing.
    const types::SymbolMap<Domain> DomainNames = {
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
            stream << DomainNames.at(domain);
        }
        catch (const std::out_of_range &)
        {
            stream << "UNKNOWN_" << static_cast<uint>(domain);
        }
        return stream;
    }

    std::istream &operator>>(std::istream &stream, Domain &domain)
    {
        return DomainNames.from_stream(stream, &domain, Domain::NONE, false);
    }

}  // namespace core::status
