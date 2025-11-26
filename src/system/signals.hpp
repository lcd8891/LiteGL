#pragma once
#include <csignal>
#include <functional>
#include "../window/exception.hpp"

namespace{std::function<void()> func = nullptr;}

#ifdef _WIN32
#include <windows.h>
    LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS* exception_info){
        if (!exception_info) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        DWORD exception_code = exception_info->ExceptionRecord->ExceptionCode;
        PVOID exception_address = exception_info->ExceptionRecord->ExceptionAddress;

        std::string error_msg = "ENGINE EXECUTION TERMINATED!\n->Windows Exception\n";

        switch (exception_code) {
            case EXCEPTION_ACCESS_VIOLATION:
                error_msg += "EXCEPTION_ACCESS_VIOLATION (Access Violation) - memory access error\n";
                error_msg += "Address: " + std::to_string(reinterpret_cast<uintptr_t>(exception_address)) + "\n";
                
                if (exception_info->ExceptionRecord->NumberParameters >= 2) {
                    ULONG_PTR violation_type = exception_info->ExceptionRecord->ExceptionInformation[0];
                    ULONG_PTR violation_address = exception_info->ExceptionRecord->ExceptionInformation[1];
                    
                    switch (violation_type) {
                        case 0:
                            error_msg += "Reason: attempt to read on address " + 
                                    std::to_string(violation_address);
                            break;
                        case 1:
                            error_msg += "Reason: attempt to write on address " + 
                                    std::to_string(violation_address);
                            break;
                        case 8:
                            error_msg += "Reason: DEP violation on address" + 
                                    std::to_string(violation_address);
                            break;
                        default:
                            error_msg += "Reason: unknown access violation";
                    }
                }
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                error_msg += "EXCEPTION_INT_DIVIDE_BY_ZERO - integer division by zero";
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                error_msg += "EXCEPTION_FLT_DIVIDE_BY_ZERO - division of a floating point by zero";
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                error_msg += "EXCEPTION_ILLEGAL_INSTRUCTION - illegal instruction";
                break;
            case EXCEPTION_STACK_OVERFLOW:
                error_msg += "EXCEPTION_STACK_OVERFLOW - stack overflow";
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                error_msg += "EXCEPTION_ARRAY_BOUNDS_EXCEEDED - array out of bounds";
                break;
            case EXCEPTION_IN_PAGE_ERROR:
                error_msg += "EXCEPTION_IN_PAGE_ERROR - page fault";
                break;
            case EXCEPTION_BREAKPOINT:
                error_msg += "EXCEPTION_BREAKPOINT - breakpoint";
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                error_msg += "EXCEPTION_DATATYPE_MISALIGNMENT - Unaligned data access";
                break;
            case EXCEPTION_FLT_OVERFLOW:
                error_msg += "EXCEPTION_FLT_OVERFLOW - floating point overflow";
                break;
            case EXCEPTION_FLT_UNDERFLOW:
                error_msg += "EXCEPTION_FLT_UNDERFLOW - floating point loss";
                break;
            case EXCEPTION_INT_OVERFLOW:
                error_msg += "EXCEPTION_INT_OVERFLOW - integer overflow";
                break;
            default:
                error_msg += "Unknown exception Windows 0x" + 
                            std::to_string(exception_code);
        }
        show_error(error_msg);
        func();
        TerminateProcess(GetCurrentProcess(), exception_code);
        return EXCEPTION_EXECUTE_HANDLER;
    }
    void init_signal_handler(std::function<void()> _func){
    func=_func;
        SetUnhandledExceptionFilter(windows_exception_handler);
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    }
#else
#include <cstdlib>
void signal_action_handler(int sig, siginfo_t* info, void* context){
    std::string error_msg = "ENGINE EXECUTION TERMINATED!\n->Recieved signal\n";
    switch(sig) {
        case SIGSEGV:
            error_msg += "SIGSEGV (Segmentation Fault)\n";
            error_msg += "Address: " + std::to_string(reinterpret_cast<uintptr_t>(info->si_addr)) + "\n";
            switch(info->si_code) {
                case SEGV_MAPERR:
                    error_msg += "Reason: access to null";
                    break;
                case SEGV_ACCERR:
                    error_msg += "Reason: violation memory access";
                    break;
                default:
                    error_msg += "Reason: unknown";
            }
            break;
            
        case SIGFPE:
            error_msg += "SIGFPE (Floating Point Exception)\n";
            switch(info->si_code) {
                case FPE_INTDIV:
                    error_msg += "Reason: integer division by zero";
                    break;
                case FPE_INTOVF:
                    error_msg += "Reason: integer overflow";
                    break;
                case FPE_FLTDIV:
                    error_msg += "Reason: division of a floating point by zero";
                    break;
                case FPE_FLTOVF:
                    error_msg += "Reason: floating point overflow";
                    break;
                case FPE_FLTUND:
                    error_msg += "Reason: floating point loss";
                    break;
                default:
                    error_msg += "Reason: unknown arithmetic error";
            }
            break;
            
        case SIGILL:
            error_msg += "SIGILL (Illegal Instruction)";
            break;
            
        case SIGABRT:
            error_msg += "SIGABRT (Abort) - Aborted";
            break;
            
        case SIGTERM:
            error_msg += "SIGTERM (Terminate) - Request for terminate";
            break;
            
        case SIGINT:
            error_msg += "SIGINT (Interrupt) - program interrupted";
            break;
            
        default:
            error_msg += "Неизвестный сигнал (" + std::to_string(sig) + ")";
    }
    show_error(error_msg);
    func();
    signal(sig, SIG_DFL);
    raise(sig);
}

void init_signal_handler(std::function<void()> _func){
    func=_func;
    struct sigaction sa;
    sa.sa_sigaction = signal_action_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_RESETHAND;
    sigemptyset(&sa.sa_mask);
    
    sigaddset(&sa.sa_mask, SIGSEGV);
    sigaddset(&sa.sa_mask, SIGFPE);
    sigaddset(&sa.sa_mask, SIGILL);
    sigaddset(&sa.sa_mask, SIGABRT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGINT);

    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGFPE, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);

    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
    
    signal(SIGPIPE, SIG_IGN);
}

#endif