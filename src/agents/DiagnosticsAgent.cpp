#include "agents/DiagnosticsAgent.h"
#include "utils/Logger.h"
#include <llvm/Support/raw_ostream.h>
#include <fstream>
#include <iostream>

void DiagnosticsAgent::addDiagnostic(Diagnostic::Level level, const std::string& message, int line, int column) {
    diagnostics.push_back(Diagnostic(level, message, line, column));
    
    switch (level) {
        case Diagnostic::Info:
            LOG_INFO("Diagnostic: " + message);
            break;
        case Diagnostic::Warning:
            LOG_WARNING("Diagnostic: " + message);
            break;
        case Diagnostic::Error:
            LOG_ERROR("Diagnostic: " + message);
            break;
    }
}

void DiagnosticsAgent::dumpIR(llvm::Module* module, bool toStdout, const std::string& filename) {
    if (!module) {
        LOG_ERROR("DiagnosticsAgent: Cannot dump IR: module is null");
        return;
    }
    
    if (toStdout) {
        LOG_INFO("DiagnosticsAgent: Dumping IR to stdout");
        module->print(llvm::outs(), nullptr);
    } else if (!filename.empty()) {
        LOG_INFO("DiagnosticsAgent: Dumping IR to file: " + filename);
        std::error_code EC;
        llvm::raw_fd_ostream file(filename, EC);
        if (EC) {
            LOG_ERROR("DiagnosticsAgent: Cannot open file: " + EC.message());
            return;
        }
        module->print(file, nullptr);
    }
}

void DiagnosticsAgent::printDiagnostics() {
    std::cout << "\n=== Diagnostics ===" << std::endl;
    for (const auto& diag : diagnostics) {
        std::string prefix;
        switch (diag.level) {
            case Diagnostic::Info:
                prefix = "[INFO]";
                break;
            case Diagnostic::Warning:
                prefix = "[WARN]";
                break;
            case Diagnostic::Error:
                prefix = "[ERROR]";
                break;
        }
        
        std::cout << prefix << " ";
        if (diag.line > 0) {
            std::cout << "line " << diag.line;
            if (diag.column > 0) {
                std::cout << ":" << diag.column;
            }
            std::cout << ": ";
        }
        std::cout << diag.message << std::endl;
    }
    std::cout << "==================\n" << std::endl;
}

void DiagnosticsAgent::clearDiagnostics() {
    diagnostics.clear();
}

bool DiagnosticsAgent::hasErrors() const {
    for (const auto& diag : diagnostics) {
        if (diag.level == Diagnostic::Error) {
            return true;
        }
    }
    return false;
}

size_t DiagnosticsAgent::errorCount() const {
    size_t count = 0;
    for (const auto& diag : diagnostics) {
        if (diag.level == Diagnostic::Error) {
            count++;
        }
    }
    return count;
}

