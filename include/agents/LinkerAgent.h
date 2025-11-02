#pragma once

#include <string>
#include <vector>

class LinkerAgent {
public:
    static bool linkWithLLD(const std::vector<std::string>& objectFiles, 
                           const std::string& outputFile,
                           const std::vector<std::string>& libraries = {});
    static bool linkWithSystemLinker(const std::vector<std::string>& objectFiles,
                                    const std::string& outputFile,
                                    const std::vector<std::string>& libraries = {});
};

