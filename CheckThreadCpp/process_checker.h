#pragma once

#include <string>
#include <map>
#include <windows.h>
#include "module_info.h"

#if INTPTR_MAX != INT64_MAX
#error Only designed for x64 build
#endif
#include "time_info.h"

class process_checker
{
	typedef std::map<ULONG64, module_info> map_module_type;
	typedef std::map<int, time_info> map_thread_time_type;

public:
	process_checker();
	~process_checker();

	void show_processes(const std::string& filter);
	void check_thread(int thread_id, HANDLE handle_process);
	void show_modules(int process_id);

private:
	void list_threads(int process_pid);
	const module_info* get_module(unsigned long long address);

private:
	HANDLE _handle_process_snap;
	HANDLE _handle_thread_snap;
	map_module_type _modules;
	map_thread_time_type _thread_times;
};