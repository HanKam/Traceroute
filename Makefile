CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

traceroute: Traceroute.o Receiver.o Sender.o Packet.o
	$(CXX) $(CXXFLAGS) -o traceroute Traceroute.o Receiver.o Sender.o Packet.o

Traceroute.o: Traceroute.cpp

Receiver.o: Receiver.cpp Receiver.hpp

Sender.o: Sender.cpp Sender.hpp

Packet.o: Packet.cpp Packet.hpp

clean:
	$(RM) *.o

distclean:
	$(RM) traceroute *.o

.PHONY: clean