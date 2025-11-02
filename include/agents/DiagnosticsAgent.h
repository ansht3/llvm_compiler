#pragma once

#include <llvm/IR/Module.h>
#include <string>
#include <vector>

struct Diagnostic {
    enum Level {
        Info,
        Warning,
        Error
    };
    
    Level level;
    std::string message;
    int line;
    int column;
    
    Diagnostic(Level l, const std::string& msg, int ln = 0, int col = 0)
        : level(l), message(msg), line(ln), column(col) {}
};

class DiagnosticsAgent {
private:
    std::vector<Diagnostic> diagnostics;
    
public:
    void addDiagnostic(Diagnostic::Level level, const std::string& message, int line = 0, int column = 0);
    void dumpIR(llvm::Module* module, bool toStdout = true, const std::string& filename = "");
    void printDiagnostics();
    void clearDiagnostics();
    
    bool hasErrors() const;
    size_t errorCount() const;
};

