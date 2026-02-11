//
// Created by itari on 2026-02-10.
//

#ifndef HYDROSIM_QUERY_SYSTEM_H
#define HYDROSIM_QUERY_SYSTEM_H

#include <iostream>
#include <windows.h>
namespace ioUtil {
    inline int print_system_type() {

#if defined(_WIN32)
        std::cout << "This system is Windows." << std::endl;
        // Windows-specific code can go here
#if defined(_WIN64)
        std::cout << "It is a 64-bit Windows system." << std::endl;
#else
        std::cout << "It is a 32-bit Windows system." << std::endl;
#endif
#elif defined(__linux__)
        std::cout << "This system is Linux." << std::endl;
#elif defined(__APPLE__) && defined(__MACH__)
        std::cout << "This system is macOS/iOS." << std::endl;
#else
        std::cout << "This system is unknown or unsupported." << std::endl;
#endif

        return 0;
    }
}


#endif //HYDROSIM_QUERY_SYSTEM_H
