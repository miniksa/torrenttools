//
// Created by fbdtemme on 2/26/20.
//
#include <filesystem>
#include <iostream>
#include <fstream>


#include <catch2/catch.hpp>

#include <dottorrent/metafile.hpp>
#include "tree_view.hpp"


namespace fs = std::filesystem;
namespace dt = dottorrent;

static auto fedora_torrent = fs::path(TEST_DIR) / "resources"
                             / "Fedora-Workstation-Live-x86_64-30.torrent";
static auto tree_index_test = fs::path(TEST_DIR) / "resources"
                             / "tree_index_test.torrent";

TEST_CASE("test tree_index") {
    SECTION("only root dir") {
        std::ifstream ifs(fedora_torrent, std::ios::binary);
        auto m = dt::read_metafile(ifs);
        auto index = filetree_index(m.storage());
        auto root_size = index.get_directory_size("");

        SECTION("directory size of root dir") {
            CHECK(root_size == m.total_file_size());
        }
        SECTION("directory list for root dir") {
            auto c = index.list_directory_content("");
            CHECK(c[0].first == "Fedora-Workstation-30-1.2-x86_64-CHECKSUM");
            CHECK(c[1].first == "Fedora-Workstation-Live-x86_64-30-1.2.iso");
        }
    }

    SECTION ("nested file structure") {
        std::ifstream ifs(tree_index_test, std::ios::binary);
        auto m = dt::read_metafile(ifs);
        auto index = filetree_index(m.storage());
        auto root_size = index.get_directory_size("");

        SECTION("directory list for root dir") {
            auto c = index.list_directory_content("");
            SUCCEED();
        }

        SECTION("directory size of root dir") {
            CHECK(root_size == m.total_file_size());
        }

        SECTION("file list") {
            auto l = index.list_directory_content("dir1");
            CHECK(l.size() == 3);
            CHECK(l[0].first == "dir3");
            CHECK(l[1].first == "f1");
            CHECK(l[2].first == "f2");

            l = index.list_directory_content("dir2");
            CHECK(l.size() == 4);
            CHECK(l[0].first == "f1");
            CHECK(l[1].first == "f2");
            CHECK(l[2].first == "f3");
            CHECK(l[3].first == "f4");
        }
    }
}
//
//TEST_CASE("test tree pretty printer") {
//    std::ifstream ifs(tree_index, std::ios::binary);
//    auto m = dt::read_metafile(ifs);
//
//    auto oi = std::ostreambuf_iterator(std::cout);
//    format_file_tree(oi, m);
//}