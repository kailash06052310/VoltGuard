#include "packet_interceptor.h"
#include <iostream>

PacketInterceptor::PacketInterceptor()
    : capturing(false)
{
}

bool PacketInterceptor::start_capture()
{
    if (capturing) {
        return false;
    }

    capturing = true;
    std::cout << "[INFO] Packet capture started." << std::endl;

    return true;
}

void PacketInterceptor::stop_capture()
{
    if (!capturing) {
        return;
    }

    capturing = false;
    std::cout << "[INFO] Packet capture stopped." << std::endl;
}

bool PacketInterceptor::parse_packet(
    const std::string& packet,
    ModbusPacket& result
)
{
    // Modbus/TCP ADU must contain at least:
    // 7-byte MBAP header + 1-byte function code
    if (packet.size() < 8) {
        std::cerr << "[WARNING] Invalid Modbus/TCP packet: too short."
                  << std::endl;
        return false;
    }

    // Extract MBAP header fields (network byte order / big-endian)
    result.transaction_id =
        (static_cast<uint8_t>(packet[0]) << 8) |
         static_cast<uint8_t>(packet[1]);

    result.protocol_id =
        (static_cast<uint8_t>(packet[2]) << 8) |
         static_cast<uint8_t>(packet[3]);

    result.length =
        (static_cast<uint8_t>(packet[4]) << 8) |
         static_cast<uint8_t>(packet[5]);

    result.unit_id =
        static_cast<uint8_t>(packet[6]);

    result.function_code =
        static_cast<uint8_t>(packet[7]);

    // Modbus/TCP protocol ID must be 0.
    if (result.protocol_id != 0) {
        std::cerr << "[WARNING] Invalid Modbus/TCP protocol ID."
                  << std::endl;
        return false;
    }

    std::cout << "[INFO] Modbus/TCP packet parsed successfully."
              << std::endl;

    std::cout << "       Transaction ID: "
              << result.transaction_id << std::endl;

    std::cout << "       Protocol ID: "
              << result.protocol_id << std::endl;

    std::cout << "       Length: "
              << result.length << std::endl;

    std::cout << "       Unit ID: "
              << static_cast<int>(result.unit_id) << std::endl;

    std::cout << "       Function Code: "
              << static_cast<int>(result.function_code) << std::endl;

    return true;
}