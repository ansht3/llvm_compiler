#include "agents/LinkerAgent.h"
#include "utils/Logger.h"
#include <cstdlib>
#include <sstream>

bool LinkerAgent::linkWithLLD(const std::vector<std::string>& objectFiles,
                              const std::string& outputFile,
                              const std::vector<std::string>& libraries) {
    LOG_INFO("LinkerAgent: Linking with lld");
    
    std::stringstream cmd;
    cmd << "lld ";
    
    for (const auto& obj : objectFiles) {
        cmd << obj << " ";
    }
    
    for (const auto& lib : libraries) {
        cmd << "-l" << lib << " ";
    }
    
    cmd << "-o " << outputFile;
    
    LOG_INFO("LinkerAgent: Running: " + cmd.str());
    int result = std::system(cmd.str().c_str());
    
    if (result != 0) {
        LOG_ERROR("LinkerAgent: Linking failed");
        return false;
    }
    
    LOG_INFO("LinkerAgent: Linking completed successfully");
    return true;
}

bool LinkerAgent::linkWithSystemLinker(const std::vector<std::string>& objectFiles,
                                       const std::string& outputFile,
                                       const std::vector<std::string>& libraries) {
    LOG_INFO("LinkerAgent: Linking with system linker");
    
    std::stringstream cmd;
    #ifdef __APPLE__
        cmd << "clang ";
    #elif __linux__
        cmd << "gcc ";
    #else
        cmd << "gcc ";
    #endif
    
    for (const auto& obj : objectFiles) {
        cmd << obj << " ";
    }
    
    for (const auto& lib : libraries) {
        cmd << "-l" << lib << " ";
    }
    
    cmd << "-o " << outputFile;
    
    LOG_INFO("LinkerAgent: Running: " + cmd.str());
    int result = std::system(cmd.str().c_str());
    
    if (result != 0) {
        LOG_ERROR("LinkerAgent: Linking failed");
        return false;
    }
    
    LOG_INFO("LinkerAgent: Linking completed successfully");
    return true;
}

