#include "network.h"

void Network::Initialize()
{
	if (enet_initialize() != 0) {
		std::cout << "Enet Initialize failed." << "\n";
	}
	atexit(enet_deinitialize);
}

ENetHost* Network::Client_Create(int channels)
{
	ENetHost* client = enet_host_create(NULL, 1, channels, 0, 0); // ?, CONNECTIONS, CHANNELS, INCOMING BANDWIDTH, OUTCOMING BANDWIDTH
	if (client == NULL) {
		std::cout << "Couldn't create ENet client\n";
	}
	return client;
}

ENetPeer* Network::Client_ConnectToServer(ENetHost* Client,std::string hostname, int port,int channels)
{
	ENetAddress address;
	ENetEvent event;
	ENetPeer* connection;

	enet_address_set_host(&address, hostname.c_str());
	address.port = port;
	std::cout << "Connecting to " << hostname.c_str() << "\n";

	connection = enet_host_connect(Client, &address, channels, 0);
	if (connection != NULL) {
		if (enet_host_service(Client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
			std::cout << "Connected!\n";
			return connection;
		}
		else {
			std::cout << "NOT Connected!\n";
			enet_peer_reset(connection);
			return NULL;
		}
	}
	return NULL;
}

void Network::Client_DisconnectFromServer(ENetHost* client,ENetPeer* connection)
{
	ENetEvent event;
	enet_peer_disconnect(connection, 0);
	while (enet_host_service(client, &event, 3000) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			break;
		}
	}
}

ENetEvent Network::Client_ReceiveData(ENetHost* client)
{
	ENetEvent event;
	while (enet_host_service(client, &event, 0) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			std::cout << "Packet length " << event.packet->dataLength << "\n";
			std::cout << "Packet data" << event.packet->data << "\n";
			std::cout << "Peer adress " << event.peer->address.host << "\n";
			std::cout << "Peer port " << event.peer->address.port << "\n";
			std::cout << "Channel " << event.channelID << "\n";
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "Server Closed\n";
			break;
		}
	}
	return event;
}



ENetHost* Network::Server_Create(int port, int max_players, int channels)
{
	ENetAddress address;
	ENetEvent event;

	address.host = ENET_HOST_ANY;
	address.port = port;

	ENetHost* server = enet_host_create(&address, max_players, channels, 0, 0); // ?, CONNECTIONS, CHANNELS, INCOMING BANDWIDTH, OUTCOMING BANDWIDTH
	if (server == NULL) {
		std::cout << "Couldn't create ENet Server\n";
	}
	return server;
}

ENetEvent Network::Server_ReceiveData(ENetHost* server)
{
	ENetEvent event;
	while (enet_host_service(server, &event, 0) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_CONNECT:
			std::cout << "NEW CONNECTION MADE > " << event.peer->address.host << ":" << event.peer->address.port << "\n";
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			std::cout << "Packet length " << event.packet->dataLength << "\n";
			std::cout << "Packet data" << event.packet->data << "\n";
			std::cout << "Peer adress " << event.peer->address.host << "\n";
			std::cout << "Peer port " << event.peer->address.port << "\n";
			std::cout << "Channel " << event.channelID << "\n";
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "DISCONNECT > " << event.peer->address.host << ":" << event.peer->address.port << "\n";
			break;
		}
	}

	return event;
}

