#include "Sender.hpp"

// Częściowo skopiowane ze strony wykładu. 
// Funkcja wysyłająca pakiet. 
void Sender::send(const Packet& packet)
{
    icmphdr header = create_icmphdr(packet.get_id(), packet.get_sequence());
    int ttl = (packet.get_sequence() - 1) / 3 + 1;
    setsockopt(socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

    sockaddr_in recipient = create_sockaddr_in(packet.get_ip_addr());

    ssize_t bytes_sent = sendto(socket, &header, sizeof(header), 0, (struct sockaddr*)&recipient, sizeof(recipient));
    if(bytes_sent <= 0)
    {
        throw "Packet can't be sent.";
    }
}

// Częściowo skopiowane ze strony wykładu.
// Funkcja tworząca zmienną typu sockaddr_in na podstawie podanego adresu IP.
sockaddr_in Sender::create_sockaddr_in(const std::string& ip_addr)
{
    sockaddr_in recipient = {};
    recipient.sin_family = AF_INET;
    if(inet_pton(AF_INET, ip_addr.c_str(), &recipient.sin_addr) != 1)
    {
        throw "IP addres does not contain a character string representing a valid network address in the specified address family.";
    }

    return recipient;
}

// Skopiowane ze strony wykładu. 
// Funkcja obliczająca sumę kontrolną dla nagłówka. 
uint16_t Sender::compute_icmp_checksum (void const *buff, int length)
{
	uint32_t sum;
	const uint16_t* ptr = (const uint16_t*) buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (uint16_t)(~(sum + (sum >> 16)));
}

// Skopiowane ze strony wykładu. 
// Funkcja tworząca nagłówek ICMP. 
icmphdr Sender::create_icmphdr (int echo_id, int echo_seq)
{
	icmphdr header;
	header.type = ICMP_ECHO;
	header.code = 0;
	header.un.echo.id = echo_id;
	header.un.echo.sequence = echo_seq;
	header.checksum = 0;
	header.checksum = compute_icmp_checksum((uint16_t*)&header, sizeof(header));
	return header;
}