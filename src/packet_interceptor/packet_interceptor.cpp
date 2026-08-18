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

bool PacketInterceptor::parse_packet(const std::string& packet)
{
    if (packet.empty()) {
        std::cerr << "[WARNING] Empty packet received." << std::endl;
        return false;
    }

    std::cout << "[INFO] Packet received: " << packet << std::endl;

    // Actual Modbus/TCP parsing will be implemented later.
    return true;
}