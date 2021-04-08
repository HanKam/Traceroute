#include "Libraries.hpp"
#include "Packet.hpp"
#include "Sender.hpp"
#include "Receiver.hpp"

// Sprawdzanie argumentów wejściowych. 
void valid_arguments(int argc, char* argv[])
{
    if(argc > 2)
    {
        throw "Arguments amount is invalid.";
    }

    std::regex rgx("^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
    if(!regex_match(argv[1], rgx))
    {
        throw "Invalid IP format.";
    }
}

// Zapisywanie 3 pakietów.
std::vector<Packet> recive_packets(Receiver& receiver, int ttl)
{
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (receiver.get_socket(), &descriptors);
    struct timeval tv; 
    tv.tv_sec = 1; 
    tv.tv_usec = 0;

    int received_packets_amount = 0;
    std::vector<Packet> packets;

    // Dopóki liczba odebranych pakietów jest mniejsza niż 3, rób...
    while(received_packets_amount < 3)
    {
        int ready = select(receiver.get_socket() + 1, &descriptors, NULL, NULL, &tv);
        if(ready < 0)
        {
            throw "Error while reading a socket.";
        }
        if(ready == 0)
        {
            // Skończył się czas przeznaczony na odebranie pakietów.
            break;
        }

        // Odbieramy pakiety będące w gnieździe.
        Packet packet = receiver.receive();

        // Sprawdzamy, czy odebrany pakiet, to ten, który odpowiada nam.
        if(packet.get_id() == getpid() % UINT16_MAX && ((packet.get_sequence() - 1) / 3 + 1) == ttl)
        {
            packets.push_back(packet);
            received_packets_amount++;
        }
    }

    return packets;
}

// Wypisywanie drogi.
bool print(const std::vector<Packet>& packets, std::string ip_addr, std::chrono::high_resolution_clock::time_point time)
{
    // Zmienna odpowiedzialna za informację, czy dotarliśmy już do adresu docelowego.
    bool destination = false;

    // Sprawdzanie, czy odebraliśmy jakiś pakiet.
    if(packets.empty())
    {
        std::cout << "*" << std::endl;
        return destination;
    }

    // Wypisywanie, gdy zostaną odebrane 3 pakiety.
    if(packets.size() == 3)
    {
        double avg_time;
        
        std::set<std::string> ip_addreses;
        for(const auto& p : packets)
        {
            ip_addreses.insert(p.get_ip_addr());
            avg_time += (p.get_time() - time).count(); 
        }
        for(const auto& ip : ip_addreses)
        {
            if(ip == ip_addr)
            {
                destination = true;
            }
            std::cout << ip << " ";
        }
        std::cout << avg_time / 1000000 / 3 << " ms" << std::endl;
    }

    // Wypisywanie, gdy nie zostaną dostarczone 3 pakiety.
    else
    {
        std::set<std::string> ip_addreses;
        for(const auto& p : packets)
        {
            ip_addreses.insert(p.get_ip_addr());
        }
        for(const auto& ip : ip_addreses)
        {
            if(ip == ip_addr)
            {
                destination = true;
            }
            std::cout << ip << " ";
        }
        std::cout << "???" << std::endl;
    }
    return destination;
}


int main(int argc, char* argv[])
{
    try
    {
        valid_arguments(argc, argv);

        // Tworzenie gniazda surowego.
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if(sockfd == -1)
        {
            throw strerror(errno);
        }

        // Tworzenie obiektów odpowiedzialnych za wysyłanie i odbieranie pakietów.
        Sender sender(sockfd);
        Receiver receiver(sockfd);
        std::string ip_addr = argv[1];

        // Zmienna odpowiedzialna za numer sekwencjny.
        int sequence = 1;

        // Iteracja przez wszystkie ttl z przedziału [1,30].
        for(int ttl = 1; ttl < 31; ttl++)
        {
            auto time = std::chrono::high_resolution_clock::now();
            
            // Wysyłanie 3 pakietów.
            for(int i = 0; i < 3; i++)
            {
                sender.send(Packet(sequence, ip_addr));
                sequence++;
            }

            // Odbieranie pakietów.
            std::vector<Packet> packets = recive_packets(receiver, ttl);

            // Wypisywanie pakietów.
            std::cout << ttl << ".   "; 
            if(print(packets, ip_addr, time))
            {
                break;
            }
        }
    }
    // Łapanie rzuconych wyjątków - wypisywanie wiadomości i zamykanie programu.
    catch(const char* msg)
    {
        std::cerr << msg << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}