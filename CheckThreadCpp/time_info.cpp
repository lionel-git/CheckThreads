#include "time_info.h"

time_info::time_info(FILETIME ft_kernel_time, FILETIME ft_user_time)
{
    kernel_time = get_time(ft_kernel_time);
    user_time = get_time(ft_user_time);
    FILETIME ft_time_stamp;
    GetSystemTimeAsFileTime(&ft_time_stamp);
    time_stamp = get_time(ft_time_stamp);

    delta_kernel_time = 0;
    delta_user_time = 0;
    delta_time_stamp = 0;
}

void time_info::update(uint64_t& delta, uint64_t& value, uint64_t new_value)
{
    delta = new_value - value;
    value = new_value;
}

void 
time_info::update(const time_info& new_time)
{
    time_info::update(delta_kernel_time, kernel_time, new_time.kernel_time);
    time_info::update(delta_user_time,   user_time,   new_time.user_time);
    time_info::update(delta_time_stamp,  time_stamp,  new_time.time_stamp);
}

double 
time_info::get_usage_percent() const
{
    return  100.0 * (double)(delta_kernel_time + delta_user_time) / (double)delta_time_stamp;
}

// All times are expressed using FILETIME data structures. 
// Such a structure contains two 32-bit values that combine to form a 64-bit count of 100-nanosecond time units.
uint64_t time_info::get_time(FILETIME ft)
{
    return (((uint64_t)ft.dwHighDateTime) << 32) | ((uint64_t)ft.dwLowDateTime);
}

// Return time as double in milli seconds
// time is in 1e-7 seconds unit
double
time_info::get_time_ms(uint64_t time)
{
    return 1.0e-4 * (double)time;
}

std::ostream& operator<<(std::ostream& os, const time_info& ti)
{
    os << "K:" << time_info::get_time_ms(ti.kernel_time)
        << " U:" << time_info::get_time_ms(ti.user_time)
        << " T: " << time_info::get_time_ms(ti.kernel_time + ti.user_time);
    if (ti.delta_time_stamp > 0)
    {
        os << " DK:" << time_info::get_time_ms(ti.delta_kernel_time)
            << " DU:" << time_info::get_time_ms(ti.delta_user_time)
            << " DT:" << time_info::get_time_ms(ti.delta_time_stamp)
            << " P:" << ti.get_usage_percent() << "%";
    }
    return os;
}