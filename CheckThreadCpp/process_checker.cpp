#include "process_checker.h"
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>

process_checker::process_checker()
{
    _handle_process_snap = INVALID_HANDLE_VALUE;
    _handle_process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (_handle_process_snap == INVALID_HANDLE_VALUE)
    {
        check_error("CreateToolhelp32Snapshot (of processes)");
    }

    _handle_thread_snap = INVALID_HANDLE_VALUE;
    _handle_thread_snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (_handle_thread_snap == INVALID_HANDLE_VALUE)
    {
        check_error("CreateToolhelp32Snapshot (of threads)");
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
        check_error("Process32First"); // show cause of failure
        return;
    }

    do
    {
        std::string exeName(pe32.szExeFile);
        if (exeName.find(filter) != std::string::npos)
        {
            std::wcout << "=============" << std::endl;
            std::wcout << "File: " << pe32.szExeFile << std::endl;
            std::wcout << "pid: " << pe32.th32ProcessID << std::endl;
            std::wcout << "ParentPid: " << pe32.th32ParentProcessID << std::endl;
            std::wcout << "Threads: " << pe32.cntThreads << std::endl;
            std::wcout << "Usage: " << pe32.cntUsage << std::endl;
            std::wcout << "==" << std::endl;
            list_threads(pe32.th32ProcessID);
        }
    } 
    while (Process32Next(_handle_process_snap, &pe32));
}

void process_checker::list_threads(int process_pid)
{
    // Fill in the size of the structure before using it. 
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if (!Thread32First(_handle_thread_snap, &te32))
    {
       check_error("Thread32First"); // show cause of failure
       return;
    }

    do
    {
        if (te32.th32OwnerProcessID == process_pid)
        {
            std::wcout << "ThreadId: " << te32.th32ThreadID << std::endl;
        }
    } 
    while (Thread32Next(_handle_thread_snap, &te32));
}

void
process_checker::check_error(const std::string& msg)
{
    DWORD eNum;
    TCHAR sysMsg[256];
    TCHAR* p;

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        sysMsg, 256, NULL);

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while ((*p > 31) || (*p == 9))
        ++p;
    do { *p-- = 0; } while ((p >= sysMsg) &&
        ((*p == '.') || (*p < 33)));

    // Display the message
    std::cout << "\n  WARNING: " << msg << " failed with error " << eNum << " (" << sysMsg << ")" << std::endl;
}