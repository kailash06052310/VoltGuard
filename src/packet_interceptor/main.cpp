#include "packet_interceptor.h"
#include <iostream>

int main()
{
    std::cout << "=== VoltGuard Packet Interceptor ===" << std::endl;

    PacketInterceptor interceptor;

    if (!interceptor.start_capture()) {
        std::cerr << "[ERROR] Failed to start packet capture."
                  << std::endl;
        return 1;
    }

    // Mock packet for initial testing.
    interceptor.parse_packet(
        "01 03 00 00 00 02 C4 0B"
    );

    interceptor.stop_capture();

    std::cout << "[INFO] Packet Interceptor test completed."
              << std::endl;

    return 0;
}