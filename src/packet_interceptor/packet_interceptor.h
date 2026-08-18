#ifndef PACKET_INTERCEPTOR_H
#define PACKET_INTERCEPTOR_H

#include <string>

class PacketInterceptor {
public:
    PacketInterceptor();

    bool start_capture();
    void stop_capture();

    bool parse_packet(const std::string& packet);

private:
    bool capturing;
};

#endif