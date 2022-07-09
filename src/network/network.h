#ifndef NETWORK
#define NETWORK

#include <WinSock2.h>
#include <enet/enet.h>
#include <iostream>
#include <string>

namespace Network {
	void Initialize();

	ENetHost* Client_Create(int channels = 1);
	ENetPeer* Client_ConnectToServer(ENetHost* Client, std::string hostname, int port, int channels = 1);
	void Client_DisconnectFromServer(ENetHost* client, ENetPeer* connection);
	ENetEvent Client_ReceiveData(ENetHost* client);

	ENetHost* Server_Create(int port,int max_players, int channels = 1);
	ENetEvent Server_ReceiveData(ENetHost* server,bool debug = false);

	bool isDisconnectionEvent(ENetEvent event);
	bool isConnectionEvent(ENetEvent event);
	bool isReceiveEvent(ENetEvent event);
	bool isEvent(ENetEvent event);

	std::string GetEventData(ENetEvent event);

	void SendPacket(ENetPeer* destination, const char* data, int channel = 0);
}

#endif