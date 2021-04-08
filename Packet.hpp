#ifndef Packet_hpp
#define Packet_hpp

#include "Libraries.hpp"

class Packet
{
public:
    Packet()
    : id(0),
      sequence(0),
      ip_addr(""),
      time(std::chrono::high_resolution_clock::now())
    {}
    Packet(int seq, const std::string& ip_addr)
    : id(getpid() % UINT16_MAX),
      sequence(seq),
      ip_addr(ip_addr),
      time(std::chrono::high_resolution_clock::now())
    {}
    Packet(const std::string& ip_addr, struct icmphdr* icmp_header)
    : id(icmp_header->un.echo.id),
      sequence(icmp_header->un.echo.sequence),
      ip_addr(ip_addr),
      time(std::chrono::high_resolution_clock::now())
    {}

    // Funkcje zwracające zmienne prywatne klasy.
    int get_id() const;
    int get_sequence() const;
    const std::string& get_ip_addr() const;
    std::chrono::high_resolution_clock::time_point get_time() const;

private:
    int id;
    int sequence;
    std::string ip_addr;
    std::chrono::high_resolution_clock::time_point time; // Czas utworzenia pakietu.
};

#endif