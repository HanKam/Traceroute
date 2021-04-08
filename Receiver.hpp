#ifndef Receiver_hpp
#define Receiver_hpp

#include "Libraries.hpp"
#include "Packet.hpp"

class Receiver
{
public:
    Receiver(int socket)
    : socket(socket)
    {}

    Packet receive();

    // Funkcja zwracająca gniazdo surowe przypisane do obiektu.
    int get_socket() const;

private:
    int socket;
};

#endif