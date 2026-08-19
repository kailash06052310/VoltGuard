#include "packet_interceptor.h"
#include <iostream>
#include <string>

int main()
{
    std::cout << "=== VoltGuard Packet Interceptor ===" << std::endl;

    PacketInterceptor interceptor;

    if (!interceptor.start_capture()) {
        std::cerr << "[ERROR] Failed to start packet capture."
                  << std::endl;

        return 1;
    }

    // Mock Modbus/TCP packet
    //
    // MBAP Header:
    // Transaction ID = 1
    // Protocol ID   = 0
    // Length        = 6
    // Unit ID       = 1
    //
    // PDU:
    // Function Code = 3
    // Start Address = 0
    // Quantity      = 2
    std::string mock_packet(
        "\x00\x01"
        "\x00\x00"
        "\x00\x06"
        "\x01"
        "\x03"
        "\x00\x00"
        "\x00\x02",
        12
    );

    ModbusPacket parsed_packet{};

    // Test invalid/incomplete packet
    std::string invalid_packet("\x00\x01\x00", 3);

    ModbusPacket invalid_result{};

    if (interceptor.parse_packet(invalid_packet, invalid_result)) {
        std::cerr << "[ERROR] Invalid packet was accepted."
                << std::endl;

        interceptor.stop_capture();
        return 1;
    }

std::cout << "[INFO] Invalid packet rejected successfully."
          << std::endl;

    if (!interceptor.parse_packet(mock_packet, parsed_packet)) {
        std::cerr << "[ERROR] Failed to parse Modbus/TCP packet."
                  << std::endl;

        interceptor.stop_capture();
        return 1;
    }

    interceptor.stop_capture();

    std::cout << "[INFO] Packet Interceptor test completed."
              << std::endl;

    return 0;
}