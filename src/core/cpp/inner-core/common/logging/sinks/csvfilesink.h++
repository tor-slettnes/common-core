/// -*- c++ -*-
//==============================================================================
/// @file csvfilesink.h++
/// @brief Log tabular data to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "tabulardatasink.h++"
#include "rotatingpath.h++"
#include "json/writer.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace core::logging
{
    class CSVFileSink : public TabularDataSink,
                        public RotatingPath,
                        public types::enable_create_shared<CSVFileSink>
    {
        using This = CSVFileSink;
        using Super = TabularDataSink;

    protected:
        CSVFileSink(const std::string contract_id,
                    const ColumnDefaults &column_template,
                    const std::string &path_template,
                    const dt::Duration &rotation_interval);

        void open(const dt::TimePoint &tp) override;
        void close() override;
        void capture_event(const status::Event::ptr &event) override;

        void write_header();

    private:
        std::shared_ptr<std::ofstream> stream_;
    };
}  // namespace core::logging
