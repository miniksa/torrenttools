#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <memory>

#include <fmt/format.h>
#include <termcontrol/termcontrol.hpp>
#include <dottorrent/literals.hpp>

#include "progress.hpp"
#include "indicator.hpp"
#include "formatters.hpp"

namespace tc = termcontrol;
namespace tt = torrenttools;

void run_with_progress(std::ostream& os, dottorrent::storage_hasher& hasher, const dottorrent::metafile& m)
{
    using namespace std::chrono_literals;

    std::size_t current_file_index = 0;
    auto& storage = m.storage();

    os << "Hashing files..." << std::endl;
    cliprogress::application app(os);


    // v1 torrents count padding files as regular files in their progress counters
    // v2 and hybrid torrents do not take padding files into account in their progress counters.
    std::size_t total_file_size;
    std::unique_ptr<cliprogress::progress_indicator> indicator;

    if (hasher.protocol() == dt::protocol::v1) {
        total_file_size = storage.total_file_size();
        indicator = make_indicator(storage, storage.at(current_file_index));
    } else {
        total_file_size = storage.total_regular_file_size();
        indicator = make_indicator_v2(storage, storage.at(current_file_index));
    }

    indicator->start();
    app.start();

    auto start_time = std::chrono::system_clock::now();
    hasher.start();

    std::size_t index = 0;

    while (hasher.bytes_done() < total_file_size) {
        auto [index, file_bytes_hashed] = hasher.current_file_progress();

        // Current file has been completed, update last entry for the previous file(s) and move to next one
        if (index != current_file_index && index < storage.file_count()) {
            for ( ; current_file_index < index; ) {
                // set to 100%
                if (indicator) {
                    auto complete_size = storage.at(current_file_index).file_size();
                    indicator->set_value(complete_size);
                    on_indicator_completion(indicator);
                    indicator->stop();
                }

                ++current_file_index;

                if (hasher.protocol() == dt::protocol::v1) {
                    indicator = make_indicator(storage, storage.at(current_file_index));
                } else {
                    indicator = make_indicator_v2(storage, storage.at(current_file_index));
                }
                if (indicator) { indicator->start(); }
            }
        }
        if (indicator) { indicator->set_value(file_bytes_hashed); }
        std::this_thread::sleep_for(80ms);
    }

    if (indicator) {
        auto complete_progress = storage.at(current_file_index).file_size();
        indicator->set_value(complete_progress);
        on_indicator_completion(indicator);
        indicator->stop();
    }
    app.request_stop();
    app.wait();
    hasher.wait();

    tc::format_to(os, tc::ecma48::character_position_absolute);
    tc::format_to(os, tc::ecma48::erase_in_line);
    tc::format_to(os, tc::ecma48::cursor_up, 2);

    auto stop_time = std::chrono::system_clock::now();
    auto total_duration = stop_time - start_time;

    print_completion_statistics(os, m, total_duration);
}


void print_completion_statistics(std::ostream& os, const dottorrent::metafile& m, std::chrono::system_clock::duration duration)
{
    auto& storage = m.storage();

    std::string average_hash_rate_str {};
    using fsecs = std::chrono::duration<double>;
    auto seconds = std::chrono::duration_cast<fsecs>(duration).count();

    if (seconds != 0) {
        average_hash_rate_str = tt::format_hash_rate(storage.total_file_size() / seconds);
    } else {
        average_hash_rate_str = "∞ B/s";
    }

    fmt::print(os, "Hashing completed in: {}\n", tt::format_duration(duration));
    fmt::print(os, "Average hash rate:    {}\n",    average_hash_rate_str);

    // Torrent file is hashed so we can return to infohash
    std::string info_hash_string {};
    if (auto protocol = m.storage().protocol(); protocol != dt::protocol::none) {
        if ((protocol & dt::protocol::hybrid) == dt::protocol::hybrid ) {
            auto infohash_v1 = dt::info_hash_v1(m).hex_string();
            auto infohash_v2 = dt::info_hash_v2(m).hex_string();
            info_hash_string = fmt::format("Infohash:             v1: {}\n"
                                           "                      v2: {}\n", infohash_v1, infohash_v2);
        }
        // v2-only
        else if ((protocol & dt::protocol::v2) == dt::protocol::v2) {
            auto infohash_v2 = dt::info_hash_v2(m).hex_string();
            info_hash_string = fmt::format("Infohash:             {}\n", infohash_v2);
        }
            // v1-only
        else if ((protocol & dt::protocol::v1) == dt::protocol::v1) {
            auto infohash_v1 = dt::info_hash_v1(m).hex_string();
            info_hash_string = fmt::format("Infohash:             {}\n", infohash_v1);
        }
    }
    os << info_hash_string;
}




/// Progress using only carriage return and newline characters.
void run_with_simple_progress(std::ostream& os, dottorrent::storage_hasher& hasher, const dottorrent::metafile& m)
{
    using namespace std::chrono_literals;

    std::size_t current_file_index = 0;
    auto& storage = m.storage();

    os << "Hashing files..." << std::endl;


    // v1 torrents count padding files as regular files in their progress counters
    // v2 and hybrid torrents do not take padding files into account in their progress counters.
    std::size_t total_file_size;
    if (hasher.protocol() == dt::protocol::v1) {
        total_file_size = storage.total_file_size();
    } else {
        total_file_size = storage.total_regular_file_size();
    }

    auto start_time = std::chrono::system_clock::now();
    hasher.start();

    std::size_t index = 0;

    print_simple_indicator(os, storage, current_file_index, hasher.protocol());
    std::flush(os);

    while (hasher.bytes_done() < total_file_size) {
        auto [index, file_bytes_hashed] = hasher.current_file_progress();

        // Current file has been completed, update last entry for the previous file(s) and move to next one
        if (index != current_file_index && index < storage.file_count()) {
            for ( ; current_file_index < index; ) {
                // set to 100%
                ++current_file_index;
                print_simple_indicator(os, storage, current_file_index, hasher.protocol());
            }
            std::flush(os);
        }
        std::this_thread::sleep_for(1s);
    }

    os << std::endl;
    hasher.wait();

    auto stop_time = std::chrono::system_clock::now();
    auto total_duration = stop_time - start_time;
    print_completion_statistics(os, m, total_duration);
}