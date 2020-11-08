#include <windows.h>
#include <iostream>

#include "utils.h"

void
utils::check_error(const std::string& msg)
{
    DWORD eNum;
    TCHAR sysMsg[256];

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // Default language
        sysMsg, 256, NULL);

    std::string sys_msg(sysMsg);
    if (sys_msg.length() > 2)
        sys_msg = sys_msg.substr(0, sys_msg.length() - 2);

    // Display the message
    std::cout << "   WARNING: " << msg << " failed with error " << eNum << " (" << sys_msg << ")" << std::endl;
}

