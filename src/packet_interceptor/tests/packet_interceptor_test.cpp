#include "../packet_interceptor.h"

#include <cassert>
#include <iostream>
#include <string>

std::string hex_to_bytes(const std::string& hex)
{
    std::string bytes;

    for (std::size_t i = 0; i < hex.length(); i += 2)
    {
        unsigned int value = std::stoul(
            hex.substr(i, 2),
            nullptr,
            16
        );

        bytes.push_back(static_cast<char>(value));
    }

    return bytes;
}

void test_valid_modbus_packet()
{
    PacketInterceptor interceptor;
    ModbusPacket packet;

    // Modbus/TCP Write Single Register
    // Transaction ID: 1
    // Protocol ID: 0
    // Length: 6
    // Unit ID: 1
    // Function: 0x06
    // Register: 1
    // Value: 1000 RPM
    const std::string raw_packet =
        hex_to_bytes("0001000000060106000103E8");

    bool result = interceptor.parse_packet(raw_packet, packet);

    assert(result);
    assert(packet.transaction_id == 1);
    assert(packet.protocol_id == 0);
    assert(packet.length == 6);
    assert(packet.unit_id == 1);
    assert(packet.function_code == 0x06);

    std::cout << "[PASS] Valid Modbus/TCP packet" << std::endl;
}

void test_short_packet()
{
    PacketInterceptor interceptor;
    ModbusPacket packet;

    const std::string raw_packet =
        hex_to_bytes("00010000");

    bool result = interceptor.parse_packet(raw_packet, packet);

    assert(!result);

    std::cout << "[PASS] Short packet rejected" << std::endl;
}

void test_invalid_protocol_id()
{
    PacketInterceptor interceptor;
    ModbusPacket packet;

    // Protocol ID = 1 instead of required 0
    const std::string raw_packet =
        hex_to_bytes("0001000100060106000103E8");

    bool result = interceptor.parse_packet(raw_packet, packet);

    assert(!result);

    std::cout << "[PASS] Invalid protocol ID rejected" << std::endl;
}

void test_dangerous_pump_command()
{
    PacketInterceptor interceptor;
    ModbusPacket packet;

    // Register 1 = 4000 RPM
    const std::string raw_packet =
        hex_to_bytes("000100000006010600010FA0");

    bool result = interceptor.parse_packet(raw_packet, packet);

    assert(result);
    assert(packet.function_code == 0x06);

    std::cout << "[PASS] Dangerous pump command parsed" << std::endl;
}

int main()
{
    std::cout << "======================================" << std::endl;
    std::cout << "   VOLTGUARD PACKET INTERCEPTOR TEST" << std::endl;
    std::cout << "======================================" << std::endl;

    test_valid_modbus_packet();
    test_short_packet();
    test_invalid_protocol_id();
    test_dangerous_pump_command();

    std::cout << "--------------------------------------" << std::endl;
    std::cout << "All C++ parser tests passed." << std::endl;
    std::cout << "======================================" << std::endl;

    return 0;
}