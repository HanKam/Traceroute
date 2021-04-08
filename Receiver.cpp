#include "Receiver.hpp"

// Częściowo skopiowane ze strony wykładu.
// Funkcja odpowiedzialna za odbieranie pojedynczych pakietów. 
Packet Receiver::receive()
{
    sockaddr_in sender;	
	socklen_t sender_len = sizeof(sender);
	u_int8_t buffer[IP_MAXPACKET];

    // Odbieranie pakietu z gniazda.
	ssize_t packet_len = recvfrom(socket, buffer, IP_MAXPACKET, MSG_DONTWAIT, (sockaddr*)&sender, &sender_len);
	if (packet_len < 0) 
    {
		throw strerror(errno);
	}
    
    // Konwersja pakietu.
    char sender_ip_str[20]; 
	if(inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str)) == nullptr)
    {
        throw strerror(errno);
    }

    // Przygotowanie obiektu odebranego pakietu. 
    iphdr* ip_header = (iphdr*) buffer;
	icmphdr* icmp_header = (icmphdr*)(buffer + 4 * ip_header->ihl);

    if(icmp_header->type == ICMP_TIME_EXCEEDED)
    {
        uint8_t* icmp_packet = (uint8_t*)buffer + 4 * ip_header->ihl;
        icmp_packet += 8;
		icmp_packet += 4 * ((struct ip*) icmp_packet)->ip_hl;
		icmp_header = (struct icmphdr*) icmp_packet;
    }
    else if(icmp_header->type != ICMP_ECHOREPLY)
    {
        // Jeżeli pakiet nie należał do szukanych przez nas typów, zwracamy pusty pakiet.
        return Packet();
    }
    return Packet(std::string(sender_ip_str), icmp_header);
}

int Receiver::get_socket() const
{
    return socket;
}