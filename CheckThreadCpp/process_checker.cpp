#include "process_checker.h"
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>

#include "utils.h"

#include <imagehlp.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "dbghelp.lib")


process_checker::process_checker()
{
    _handle_process_snap = INVALID_HANDLE_VALUE;
    _handle_process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (_handle_process_snap == INVALID_HANDLE_VALUE)
    {
        utils::check_error("CreateToolhelp32Snapshot (of processes)");
    }

    _handle_thread_snap = INVALID_HANDLE_VALUE;
    _handle_thread_snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (_handle_thread_snap == INVALID_HANDLE_VALUE)
    {
        utils::check_error("CreateToolhelp32Snapshot (of threads)");
    }
}

process_checker::~process_checker()
{
    if (_handle_process_snap!= INVALID_HANDLE_VALUE)
        CloseHandle(_handle_process_snap);
    if (_handle_thread_snap != INVALID_HANDLE_VALUE)
        CloseHandle(_handle_thread_snap);
}

void
process_checker::show_processes(const std::string& filter)
{
    PROCESSENTRY32 pe32;
    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(_handle_process_snap, &pe32))
    {
        utils::check_error("Process32First"); // show cause of failure
        return;
    }

    do
    {
        std::string exeName(pe32.szExeFile);
        if (exeName.find(filter) != std::string::npos)
        {
            std::cout << "=============" << std::endl;
            std::cout << "File: " << pe32.szExeFile << std::endl;
            std::cout << "pid: " << pe32.th32ProcessID << std::endl;
            std::cout << "ParentPid: " << pe32.th32ParentProcessID << std::endl;
            std::cout << "Threads: " << pe32.cntThreads << std::endl;
            std::cout << "Usage: " << pe32.cntUsage << std::endl;
            std::cout << "==" << std::endl;
            show_modules(pe32.th32ProcessID);
            list_threads(pe32.th32ProcessID);
        }
    } 
    while (Process32Next(_handle_process_snap, &pe32));
}

void process_checker::list_threads(int process_pid)
{
    HANDLE handle_process = OpenProcess(PROCESS_ALL_ACCESS, false, process_pid);
    if (handle_process == 0)
    {
        utils::check_error("Open process");
        return;
    }

    if (!SymInitialize(handle_process, "C:\\Program Files(x86)\\Arena\\Engines\\Stockfish", true))
    {
        utils::check_error("SymInitialize");
    }

    // Fill in the size of the structure before using it. 
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if (!Thread32First(_handle_thread_snap, &te32))
    {
       utils::check_error("Thread32First"); // show cause of failure
       return;
    }

    do
    {
        if (te32.th32OwnerProcessID == process_pid)
        {
            std::cout << "=============================" << std::endl;
            std::cout << "ThreadId: " << te32.th32ThreadID << std::endl;
            check_thread(te32.th32ThreadID, handle_process);
        }
    } 
    while (Thread32Next(_handle_thread_snap, &te32));
}

void process_checker::check_thread(int thread_id, HANDLE handle_process)
{
    HANDLE handle_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, thread_id);
    if (handle_thread == 0)
    {
        utils::check_error("Error getting thread handle " + thread_id);
        return;
    }
    std::cout << "Thread open is ok: " << handle_thread << std::endl;
    
    CONTEXT c;
    memset(&c, 0, sizeof(c));
    c.ContextFlags = CONTEXT_ALL;

    if (SuspendThread(handle_thread) == -1)
    {
        utils::check_error("SuspendThread");
    }   
    if (GetThreadContext(handle_thread, &c) == 0)
    {
        utils::check_error("Get context");
    }
   
    std::cout << std::hex;
    std::cout << "c.ContextFlags = " << c.ContextFlags << std::endl;
    std::cout << "c.Rip = " << c.Rip << std::endl;
    std::cout << "c.Rsp = " << c.Rsp << std::endl;
    std::cout << "c.Rbp = " << c.Rbp << std::endl;

    STACKFRAME64 frame;
    memset(&frame, 0, sizeof(frame));
    frame.AddrPC.Offset = c.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Offset = c.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = c.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;

    do
    {

        // 32 bits: IMAGE_FILE_MACHINE_I386
        DWORD image_type = IMAGE_FILE_MACHINE_AMD64;
        if (StackWalk64(image_type, handle_process, handle_thread, &frame, &c, NULL,
            NULL /*SymFunctionTableAccess64*/,  NULL /*SymGetModuleBase64*/,  NULL) == false)
        {
            std::cout << "Error stackwalk" << std::endl;
        }

        //std::cout << std::hex;        
        //std::cout << "frame.AddrPC = " << /*frame.AddrPC.Mode << " " << frame.AddrPC.Segment << " " <<*/ frame.AddrPC.Offset 
        //    /*<< " V: " << frame.Virtual*/ ;
        //std::cout << std::dec;

        auto module_ptr = get_module(frame.AddrPC.Offset);
       /* if (module_ptr != nullptr)
        {
            std::cout << std::hex;
            std::cout << "Module:" << module_ptr->name << " " << module_ptr->base_address << std::endl;
            std::cout << std::dec;
        }*/



        int maxNameLength = 1024;
        std::unique_ptr<IMAGEHLP_SYMBOL64> p((IMAGEHLP_SYMBOL64*)new char[sizeof(IMAGEHLP_SYMBOL64) + maxNameLength]);
        memset(p.get(), 0, sizeof(IMAGEHLP_SYMBOL64) + maxNameLength);
        p->MaxNameLength = maxNameLength;
        p->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);        
        DWORD64 displacement = 0;

        std::cout << "\t";
        if (SymGetSymFromAddr64(handle_process, frame.AddrPC.Offset, &displacement, p.get()) == false)
        {
            auto last_error = GetLastError();
            if (last_error!=487)
                utils::check_error("SymGetSymFromAddr64");
            if (module_ptr != nullptr)
            {
                std::cout << std::hex;
                std::cout << module_ptr->name << "+0x" << frame.AddrPC.Offset - module_ptr->base_address << std::endl;
                std::cout << std::dec;
            }
        }
        else
        {
            if (module_ptr != nullptr)
                std::cout << module_ptr->name << "!";
            std::cout << p->Name;
            std::cout << std::hex;
            std::cout << "+0x" << frame.AddrPC.Offset - p->Address << std::endl;
            std::cout << std::dec;
        }
    }
    while (frame.AddrReturn.Offset != 0);

    if (ResumeThread(handle_thread) == -1)
    {
        utils::check_error("ResumeThread");
    }

    if (CloseHandle(handle_thread) == 0)
    {
        utils::check_error("Error closing handle");
    }
}

void process_checker::show_modules(int process_id)
{
    // Take a snapshot of all modules in the specified process.
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        utils::check_error("CreateToolhelp32Snapshot (of modules)");
        return;
    }

    // Set the size of the structure before using it.
    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful
    if (!Module32First(hModuleSnap, &me32))
    {
        utils::check_error("Module32First");  // show cause of failure
        CloseHandle(hModuleSnap);           // clean the snapshot object
        return;
    }

    // Now walk the module list of the process,
    // and display information about each module
    do
    {
        std::cout << "MODULE NAME:   " << me32.szModule << std::endl;
        std::cout << "     Executable     = " << me32.szExePath << std::endl;
        std::cout << "     Process ID     = " << me32.th32ProcessID << std::endl;
        std::cout << std::hex;
        std::cout << "     Ref count (g)  = " << me32.GlblcntUsage << std::endl;
        std::cout << "     Ref count (p)  = " << me32.ProccntUsage << std::endl;
        std::cout << "     Base address   = " << (ULONG64)me32.modBaseAddr << std::endl;
        std::cout << "     Base size      = " << me32.modBaseSize << std::endl;
        std::cout << std::dec;

        module_info module_info;
        module_info.name = me32.szModule;
        module_info.base_address = (ULONG64)me32.modBaseAddr;
        module_info.module_size = me32.modBaseSize;
        module_info.executable_path = me32.szExePath;
        module_info.process_id = me32.th32ProcessID;
        _modules[module_info.base_address]= module_info;
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}

const module_info*
process_checker::get_module(unsigned long long address)
{
	for (auto it = _modules.begin(); it != _modules.end(); ++it)
	{
		if (it->second.base_address <= address && address <= it->second.base_address + it->second.module_size)
			return &(it->second);
	}
	return nullptr;
}
