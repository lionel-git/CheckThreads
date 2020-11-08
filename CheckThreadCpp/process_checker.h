#pragma once

#include <string>
#include <map>
#include <windows.h>
#include "module_info.h"

class process_checker
{
	typedef std::map<ULONG64, module_info> map_module_type;
	typedef std::map<ULONG64, module_info>::iterator map_module_iterator;

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
};