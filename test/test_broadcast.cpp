//
//  broadcast_test.cpp
//  orwell
//
//  Created by Massimo Gengarelli on 15/02/14.
//
//

#include "orwell/com/RawMessage.hpp"

#include "controller.pb.h"
#include "server-game.pb.h"

#include "orwell/support/GlobalLogger.hpp"
#include "orwell/game/Game.hpp"
#include "orwell/com/Sender.hpp"
#include "orwell/com/Receiver.hpp"
#include "orwell/BroadcastServer.hpp"
#include "orwell/support/GlobalLogger.hpp"

#include "Common.hpp"

#include <log4cxx/ndc.h>

#include <string>
#include <cstring>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <signal.h>

#define MULTICAST_GROUP "225.0.0.42"

using namespace log4cxx;

using namespace orwell::com;
using namespace orwell::messages;

static orwell::BroadcastServer * ServerPtr;

static void signal_handler(int signum)
{
	ORWELL_LOG_INFO("signal_handler received " << signum);
	ServerPtr->stop();
}

struct IP4
{
	uint8_t b1, b2, b3, b4 = 0;

	operator std::string() const
	{
		char buffer[32];
		sprintf(&buffer[0], "%u.%u.%u.%u", b1, b2, b3, b4);

		return std::string(buffer);
	};

	operator bool() const
	{
		return (not
				(b1 == 0 and b2 == 0 and b3 == 0 and b4 == 0));
	};

	friend std::ostream & operator<<(std::ostream & _stream, IP4 const & ip4)
	{
		_stream << (std::string) ip4;
		return _stream;
	};
};

bool get_ip4(IP4 & oIp4)
{
	char szBuffer[1024];

	if(gethostname(szBuffer, sizeof(szBuffer)) == -1)
	{
		return false;
	}

	struct hostent *host = gethostbyname(szBuffer);
	if(host == NULL)
	{
		return false;
	}

	//Obtain the computer's IP
	oIp4.b1 = (uint8_t) host->h_addr_list[0][0];
	oIp4.b2 = (uint8_t) host->h_addr_list[0][1];
	oIp4.b3 = (uint8_t) host->h_addr_list[0][2];
	oIp4.b4 = (uint8_t) host->h_addr_list[0][3];

	return true;
}

uint32_t simulateClient(char const * iMessage)
{
	log4cxx::NDC ndc("client");

	ssize_t const aMessageLength = strlen(iMessage);

	// Build the socket
	int aSocket;
	if ((aSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("socket()");
		return 255;
	}

	IP4 address;
	if (not get_ip4(address))
	{
		ORWELL_LOG_ERROR("Couldn't retrieve local address");
		return 255;
	}
	address.b4 = 255;

	if (address)
	{
		ORWELL_LOG_INFO("IP: " << address);
	}

	struct sockaddr_in aDestination;
	memset(&aDestination, 0, sizeof(aDestination));
	aDestination.sin_family = AF_INET;
	aDestination.sin_addr.s_addr = inet_addr(
			((std::string) address).c_str());
	aDestination.sin_port = htons(9080);

	// Set the destination to the socket
	setsockopt(aSocket, IPPROTO_IP, IP_MULTICAST_IF, &aDestination, sizeof(aDestination));

	// Allow the socket to send broadcast messages
	int const broadcast = 1;
	if ((setsockopt(aSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int))) == -1)
	{
		ORWELL_LOG_ERROR("Not allowed to send broadcast");
		return 255;
	}

	struct ip_mreq aGroup;
	bzero(&aGroup, sizeof(aGroup));
	aGroup.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
	aGroup.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(aSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &aGroup, sizeof(aGroup)) < 0)
	{
		ORWELL_LOG_ERROR("Failed to join multicast group (" << MULTICAST_GROUP << ")");
		return 255;
	}

	if (aMessageLength != sendto(
				aSocket,
				iMessage,
				aMessageLength,
				0,
				(struct sockaddr *) &aDestination,
				sizeof(aDestination)))
	{
		ORWELL_LOG_ERROR("Did not send the right number of bytes..");
		return 255;
	}

	unsigned int aDestinationLength = sizeof(aDestination);
	char aReply[256];
	if (-1 == recvfrom(
				aSocket,
				aReply,
				sizeof(aReply),
				0,
				(struct sockaddr *) &aDestination,
				&aDestinationLength))
	{
		ORWELL_LOG_ERROR("Did not receive a message...");
		return 255;
	}

	uint8_t const aFirstSeparator = (uint8_t) aReply[0];
	uint8_t const aFirstSize = (uint8_t) aReply[1];
	std::string const aFirstUrl = std::string(&aReply[2], aFirstSize);
	uint8_t const aSecondSeparator = (uint8_t) aReply[2 + aFirstSize];
	uint8_t const aSecondSize = (uint8_t) aReply[2 + aFirstSize + 1];
	std::string const aSecondUrl = std::string(&aReply[2 + aFirstSize + 2], aSecondSize);
	char aBufferForLogger[128];
	int aReturn = 0;
	int const aVersion = atoi(iMessage);
	if (0 == aVersion)
	{
		sprintf(aBufferForLogger, "0x%X %d (%s) 0x%X %d (%s)\n",
				aFirstSeparator, aFirstSize, aFirstUrl.c_str(),
				aSecondSeparator, aSecondSize, aSecondUrl.c_str());
		if ((0xA0 != aFirstSeparator) or (0xA1 != aSecondSeparator))
		{
			aReturn += 1;
		}
	}
	else
	{
		uint8_t const aThirdSeparator = (uint8_t) aReply[2 + aSecondSize];
		uint8_t const aThirdSize = (uint8_t) aReply[2 + aSecondSize + 1];
		std::string const aThirdUrl = std::string(&aReply[2 + aSecondSize + 2], aThirdSize);

		sprintf(aBufferForLogger, "0x%X %d (%s) 0x%X %d (%s) 0x%X %d (%s)\n",
				aFirstSeparator, aFirstSize, aFirstUrl.c_str(),
				aSecondSeparator, aSecondSize, aSecondUrl.c_str(),
				aThirdSeparator, aThirdSize, aThirdUrl.c_str());
		if ((0xA2 != aThirdSeparator))
		{
			aReturn += 1;
		}
	}
	ORWELL_LOG_INFO(aBufferForLogger);

	close(aSocket);

	return aReturn;
}

void simulateServer()
{
	log4cxx::NDC ndc("server");
	ORWELL_LOG_INFO("Running broadcast receiver on server");
	ServerPtr->runBroadcastReceiver();
	ORWELL_LOG_INFO("Server stopped correctly");
}

int main(int argc, const char * argv [])
{
	orwell::support::GlobalLogger::Create("test_broadcast", "test_broadcast.log");
	ORWELL_LOG_INFO("\nmain");
	log4cxx::NDC ndc("test_broadcast");
	int aRc(0);

	std::string const aPullerUrl("tcp://*:9800");
	std::string const aPublisherUrl("tcp://*:9991");
	std::string const aRequestUrl("tcp://*:9992");
	std::string const aAgentUrl("tcp://*:9993");
	
	// we want to stop the server when we receive SIGCHLD
	// this will be received when the child exits
	signal(SIGCHLD, signal_handler);

	pid_t aPid = fork();
	switch (aPid)
	{
		case 0:
			// child
			usleep(2543);
			// version 0
			aRc = simulateClient("") + simulateClient("0");
			// version 1
			aRc += simulateClient("1");
			ORWELL_LOG_INFO("Return from child");
			break;
		default:
			// parent
			ServerPtr = new orwell::BroadcastServer(
					9080, aPullerUrl, aPublisherUrl, aRequestUrl, aAgentUrl);
			simulateServer();
			ORWELL_LOG_INFO("Delete server in parent");
			delete ServerPtr;

			ORWELL_LOG_INFO("Return from parent");
			orwell::support::GlobalLogger::Clear();
			break;
	}
	
	return aRc;
}

