#include "packet_interceptor.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string hex_to_bytes(const std::string& hex)
{
    std::string bytes;

    if (hex.length() % 2 != 0) {
        return "";
    }

    for (size_t i = 0; i < hex.length(); i += 2) {
        try {
            unsigned int value =
                std::stoul(hex.substr(i, 2), nullptr, 16);

            bytes.push_back(static_cast<char>(value));
        }
        catch (...) {
            return "";
        }
    }

    return bytes;
}

void print_usage()
{
    std::cout << "VoltGuard Packet Interceptor\n\n";

    std::cout << "Usage:\n";
    std::cout << "  VoltGuard.exe <hex_modbus_packet>\n\n";

    std::cout << "Example:\n";
    std::cout << "  VoltGuard.exe 0001000000060106000103E8\n\n";
}

int main(int argc, char* argv[])
{
    PacketInterceptor interceptor;

    if (!interceptor.start_capture()) {
        std::cerr << "[ERROR] Failed to start packet capture."
                  << std::endl;

        return 1;
    }

    std::string hex_packet;

    // --------------------------------------------------
    // Command-line packet mode
    // --------------------------------------------------

    if (argc >= 2) {
        hex_packet = argv[1];
    }
    else {
        // Default demo packet:
        // Transaction ID = 1
        // Protocol ID   = 0
        // Length        = 6
        // Unit ID       = 1
        // Function      = 6
        // Register      = 1
        // Value         = 1000 RPM = 0x03E8

        hex_packet = "0001000000060106000103E8";

        std::cout << "[INFO] No packet supplied."
                  << std::endl;

        std::cout << "[INFO] Using default demo Modbus/TCP packet."
                  << std::endl;
    }

    std::string packet = hex_to_bytes(hex_packet);

    if (packet.empty()) {
        std::cerr << "[ERROR] Invalid hexadecimal packet."
                  << std::endl;

        interceptor.stop_capture();
        return 1;
    }

    ModbusPacket result{};

    if (!interceptor.parse_packet(packet, result)) {
        std::cerr << "[ERROR] Packet parsing failed."
                  << std::endl;

        interceptor.stop_capture();
        return 1;
    }

    std::cout << std::endl;
    std::cout << "======================================"
              << std::endl;
    std::cout << "      VOLTGUARD PACKET RESULT"
              << std::endl;
    std::cout << "======================================"
              << std::endl;

    std::cout << "Transaction ID : "
              << result.transaction_id << std::endl;

    std::cout << "Protocol ID    : "
              << result.protocol_id << std::endl;

    std::cout << "Length         : "
              << result.length << std::endl;

    std::cout << "Unit ID        : "
              << static_cast<int>(result.unit_id)
              << std::endl;

    std::cout << "Function Code  : "
              << static_cast<int>(result.function_code)
              << std::endl;

    // --------------------------------------------------
    // For Modbus function 0x06:
    //
    // Byte 8-9  = Register Address
    // Byte 10-11 = Register Value
    //
    // Our simulated Register 1 represents Pump RPM.
    // --------------------------------------------------

    if (result.function_code == 0x06 &&
        packet.size() >= 12) {

        uint16_t register_address =
            (static_cast<uint8_t>(packet[8]) << 8) |
             static_cast<uint8_t>(packet[9]);

        uint16_t register_value =
            (static_cast<uint8_t>(packet[10]) << 8) |
             static_cast<uint8_t>(packet[11]);

        std::cout << "Register Addr  : "
                  << register_address << std::endl;

        std::cout << "Register Value : "
                  << register_value << std::endl;

        if (register_address == 1) {
            std::cout << "Pump RPM       : "
                      << register_value << std::endl;
        }
    }

    std::cout << "======================================"
              << std::endl;

    interceptor.stop_capture();

    return 0;
}