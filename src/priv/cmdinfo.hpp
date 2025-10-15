#pragma once

#include <filesystem>
#include <LiteGL/logger.hpp>

namespace fs = std::filesystem;

namespace OUTINFO{
    void output(){
        std::filesystem::path absolute = fs::absolute(".");
        LiteAPI::Logger::info("Running from: "+absolute.string());
    }
}