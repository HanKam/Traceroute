#include "Packet.hpp"

int Packet::get_id() const
{
    return id;
}
int Packet::get_sequence() const
{
    return sequence;
}
const std::string& Packet::get_ip_addr() const
{
    return ip_addr;
}
std::chrono::high_resolution_clock::time_point Packet::get_time() const
{
    return time;
}