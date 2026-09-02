#include "packet_interceptor.h"
#include <iostream>
#include <string>

int main()
{
    PacketInterceptor interceptor;

    // Start packet capture
    if (!interceptor.start_capture()) {
        std::cerr << "[ERROR] Failed to start packet capture."
                  << std::endl;
        return 1;
    }

    // Sample Modbus/TCP Write Single Register packet
    // Transaction ID: 1
    // Protocol ID:    0
    // Length:         6
    // Unit ID:        1
    // Function:       0x06
    // Register:       1
    // Value:          1000 (0x03E8)
    std::string packet = {
        '\x00', '\x01',
        '\x00', '\x00',
        '\x00', '\x06',
        '\x01',
        '\x06',
        '\x00', '\x01',
        '\x03', '\xE8'
    };

    ModbusPacket parsed{};

    if (interceptor.parse_packet(packet, parsed)) {
        std::cout << "\n[RESULT] Packet parsed successfully.\n";
        std::cout << "Function Code: "
                  << static_cast<int>(parsed.function_code)
                  << std::endl;
    } else {
        std::cerr << "[ERROR] Packet parsing failed."
                  << std::endl;
    }

    interceptor.stop_capture();

    return 0;
}