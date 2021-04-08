#ifndef Sender_hpp
#define Sender_hpp

#include "Libraries.hpp"
#include "Packet.hpp"

class Sender
{
public:
    Sender(int s)
    : socket(s)
    {}

    void send(const Packet& packet);

private:
    int socket;

    sockaddr_in create_sockaddr_in(const std::string& ip_addr);
    uint16_t compute_icmp_checksum (void const *buff, int length);
    icmphdr create_icmphdr (int echo_id, int echo_seq); 
};

#endif