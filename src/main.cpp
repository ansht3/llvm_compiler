#include "agents/ParserAgent.h"
#include "agents/ASTAgent.h"
#include "agents/IRGenerationAgent.h"
#include "agents/ModuleSetupAgent.h"
#include "agents/OptimizationAgent.h"
#include "agents/VerificationAgent.h"
#include "agents/JITAgent.h"
#include "agents/CodegenAgent.h"
#include "agents/LinkerAgent.h"
#include "agents/DiagnosticsAgent.h"
#include "agents/SanitizerAgent.h"
#include "utils/Logger.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/IR/Module.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace llvm::cl;

static opt<std::string> InputFilename(Positional, desc("<input DSL file>"), Required);
static opt<std::string> OutputFilename("o", desc("Output filename"), value_desc("filename"));
static opt<bool> EmitIR("emit-ir", desc("Emit LLVM IR"));
static opt<bool> EmitObject("emit-obj", desc("Emit object file"));
static opt<bool> EmitBitcode("emit-bc", desc("Emit bitcode"));
static opt<bool> EmitAssembly("emit-asm", desc("Emit assembly"));
static opt<bool> RunJIT("jit", desc("Run using JIT"));
static opt<bool> DumpIR("dump-ir", desc("Dump IR to stdout"));
static opt<bool> NoOptimize("O0", desc("Disable optimizations"));
static opt<int> OptLevel("O", desc("Optimization level (0-3)"), value_desc("level"), init(2));
static opt<bool> EnableASan("asan", desc("Enable AddressSanitizer"));
static opt<bool> EnableUBSan("ubsan", desc("Enable UndefinedBehaviorSanitizer"));
static opt<bool> Verbose("v", desc("Verbose output"));
static opt<bool> Link("link", desc("Link object file to executable"));

int main(int argc, char** argv) {
    llvm::cl::ParseCommandLineOptions(argc, argv, "LLVM DSL Compiler\n");
    
    LOG_INFO("=== LLVM DSL Compiler ===");
    
    // Parse source code
    std::string source;
    try {
        source = ParserAgent::readFile(InputFilename);
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to read input file: " + std::string(e.what()));
        return 1;
    }
    
    DiagnosticsAgent diagnostics;
    
    // Agent 1: Parser Agent
    LOG_INFO("\n[Agent 1] Parser Agent");
    ParserAgent parserAgent(source);
    std::unique_ptr<ast::Program> program;
    try {
        program = parserAgent.parse();
    } catch (const std::exception& e) {
        diagnostics.addDiagnostic(Diagnostic::Error, "Parse error: " + std::string(e.what()));
        diagnostics.printDiagnostics();
        return 1;
    }
    
    // Agent 2: AST Agent
    LOG_INFO("\n[Agent 2] AST Agent");
    ASTAgent::validateAST(program.get());
    if (Verbose) {
        ASTAgent::dumpAST(program.get());
    }
    
    // Agent 3: IR Generation Agent
    LOG_INFO("\n[Agent 3] IR Generation Agent");
    llvm::LLVMContext context;
    IRGenerationAgent irAgent(context);
    irAgent.generate(program.get());
    llvm::Module* module = irAgent.getModule();
    
    // Agent 4: Module Setup Agent
    LOG_INFO("\n[Agent 4] Module Setup Agent");
    ModuleSetupAgent moduleAgent;
    moduleAgent.setupModule(module);
    
    // Agent 5: Optimization Agent
    LOG_INFO("\n[Agent 5] Optimization Agent");
    if (!NoOptimize) {
        OptimizationAgent optAgent;
        optAgent.configureOptimizationLevel(OptLevel);
        optAgent.optimize(module);
    }
    
    // Agent 6: Verification Agent
    LOG_INFO("\n[Agent 6] Verification Agent");
    if (!VerificationAgent::verify(module, true)) {
        diagnostics.addDiagnostic(Diagnostic::Error, "IR verification failed");
        diagnostics.printDiagnostics();
        return 1;
    }
    
    // Agent 7: Diagnostics Agent
    LOG_INFO("\n[Agent 7] Diagnostics Agent");
    if (DumpIR || Verbose) {
        diagnostics.dumpIR(module, true);
    }
    
    // Agent 8: Sanitizer Agent
    LOG_INFO("\n[Agent 8] Sanitizer Agent");
    if (EnableASan || EnableUBSan) {
        SanitizerAgent::addSanitizers(module, EnableASan, EnableUBSan);
    }
    
    bool hasOutput = false;
    
    // Agent 9: Codegen Agent
    if (EmitIR || EmitObject || EmitBitcode || EmitAssembly || OutputFilename != "") {
        LOG_INFO("\n[Agent 9] Codegen Agent");
        CodegenAgent codegenAgent;
        
        std::string outputFile = OutputFilename;
        if (outputFile.empty()) {
            outputFile = InputFilename + ".o";
        }
        
        if (EmitIR || outputFile.find(".ll") != std::string::npos) {
            codegenAgent.emitIRFile(module, outputFile);
            hasOutput = true;
        } else if (EmitAssembly || outputFile.find(".s") != std::string::npos) {
            codegenAgent.emitAssemblyFile(module, outputFile);
            hasOutput = true;
        } else if (EmitBitcode || outputFile.find(".bc") != std::string::npos) {
            codegenAgent.emitBitcodeFile(module, outputFile);
            hasOutput = true;
        } else if (EmitObject || outputFile.find(".o") != std::string::npos) {
            codegenAgent.emitObjectFile(module, outputFile);
            hasOutput = true;
            
            // Agent 10: Linker Agent
            if (Link) {
                LOG_INFO("\n[Agent 10] Linker Agent");
                std::vector<std::string> objects = {outputFile};
                std::string exeFile = outputFile;
                if (exeFile.find(".o") != std::string::npos) {
                    exeFile.replace(exeFile.find(".o"), 2, "");
                }
                exeFile += ".out";
                
                // Try lld first, fall back to system linker
                if (!LinkerAgent::linkWithLLD(objects, exeFile)) {
                    LOG_WARNING("lld not available, trying system linker");
                    LinkerAgent::linkWithSystemLinker(objects, exeFile);
                }
            }
        }
    }
    
    // Agent 11: JIT Agent
    if (RunJIT) {
        LOG_INFO("\n[Agent 11] JIT Agent");
        JITAgent jitAgent;
        if (jitAgent.initialize()) {
            // Create a copy of the module for JIT
            auto moduleCopy = llvm::CloneModule(*module);
            if (jitAgent.addModule(std::move(moduleCopy))) {
                LOG_INFO("JIT Agent: Module loaded successfully");
                
                // Try to find and execute main function
                void* mainFunc = jitAgent.getFunctionAddress("main");
                if (mainFunc) {
                    typedef int (*MainFunc)();
                    MainFunc main = reinterpret_cast<MainFunc>(mainFunc);
                    LOG_INFO("Executing main() via JIT...");
                    int result = main();
                    LOG_INFO("Program returned: " + std::to_string(result));
                } else {
                    LOG_WARNING("No main() function found for JIT execution");
                }
            }
        }
    }
    
    diagnostics.printDiagnostics();
    
    if (diagnostics.hasErrors()) {
        LOG_ERROR("Compilation failed with " + std::to_string(diagnostics.errorCount()) + " error(s)");
        return 1;
    }
    
    LOG_INFO("\n=== Compilation successful ===");
    return 0;
}

