#pragma once

#include <string>
#include <windows.h>

class process_checker
{
public:
	process_checker();
	~process_checker();

	void show_processes(const std::string& filter);

private:
	void check_error(const std::string& msg);
	void list_threads(int process_pid);

private:
	HANDLE _handle_process_snap;
	HANDLE _handle_thread_snap;

};