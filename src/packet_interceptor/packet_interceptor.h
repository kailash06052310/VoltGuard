#ifndef PACKET_INTERCEPTOR_H
#define PACKET_INTERCEPTOR_H

#include <cstdint>
#include <string>

struct ModbusPacket {
    uint16_t transaction_id;
    uint16_t protocol_id;
    uint16_t length;
    uint8_t unit_id;
    uint8_t function_code;
};

class PacketInterceptor {
public:
    PacketInterceptor();

    bool start_capture();
    void stop_capture();

    bool parse_packet(const std::string& packet, ModbusPacket& result);

private:
    bool capturing;
};

#endif