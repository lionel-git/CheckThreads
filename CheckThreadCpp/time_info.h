#pragma once

#include <cstdint>
#include <Windows.h>
#include <ostream>

class time_info
{
public:
	time_info(FILETIME ft_kernel_time, FILETIME ft_user_time);
	void update(const time_info& rhs);
	double get_usage_percent() const;

	static uint64_t get_time(FILETIME ft);
	static double get_time_ms(uint64_t time);
	
	friend std::ostream& operator<<(std::ostream& os, const time_info&);

private:
	static void update(uint64_t& value, uint64_t& delta, uint64_t new_value);

private:
	uint64_t kernel_time;
	uint64_t user_time;
	uint64_t time_stamp;

	// Diff with previous
	uint64_t delta_kernel_time;
	uint64_t delta_user_time;
	uint64_t delta_time_stamp;
};

