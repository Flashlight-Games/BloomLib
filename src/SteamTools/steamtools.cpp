#include "steamtools.h"

SteamTools::SteamManager::SteamManager() {}

bool SteamTools::SteamManager::Init(int appid)
{
	if (SteamAPI_RestartAppIfNecessary(appid))
		return false;
	SteamAPI_Init();
    SteamNetworkingUtils()->InitRelayNetworkAccess();

	return true;
}

std::string SteamTools::SteamManager::GetMySteamName()
{
	return SteamFriends()->GetPersonaName();
}

void SteamTools::SteamManager::ProcessCallbacks()
{
	SteamAPI_RunCallbacks();
}

void SteamTools::SteamManager::OnGameOverlayActivated(GameOverlayActivated_t* pCallback)
{
	this->overlay_active = pCallback->m_bActive;
}

void SteamTools::SteamManager::FindLobbies()
{
    if (network_status != NetworkStatus::offline) {
        SteamMatchmaking()->LeaveLobby(current_lobby);
        network_status = NetworkStatus::offline;
    }
    network_status = NetworkStatus::searching;
    m_LobbyMatchList_t.Set(SteamMatchmaking()->RequestLobbyList(), this, &SteamManager::OnLobbyListFound);
    this->found_lobbies = -1;
}

void SteamTools::SteamManager::OnLobbyListFound(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure) {
	this->found_lobbies = pLobbyMatchList->m_nLobbiesMatching;
}

void SteamTools::SteamManager::OnLobbyEnter(LobbyEnter_t* pCallback)
{
	this->lobby_connection_success = pCallback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess;
    current_lobby = pCallback->m_ulSteamIDLobby;

    network_status = NetworkStatus::connected;
    SteamFriends()->SetRichPresence("connect", std::to_string(current_lobby.GetAccountID()).c_str());
}

void SteamTools::SteamManager::OnLobbyCreated(LobbyCreated_t* pCallback)
{
	this->created_lobby_id = pCallback->m_ulSteamIDLobby;
    current_lobby = created_lobby_id;

    SteamMatchmaking()->SetLobbyData(current_lobby, "BloomGameName", "Shooter");
    network_status = NetworkStatus::connected;
}

void SteamTools::SteamManager::OnGameLobbyJoinRequested_t(GameLobbyJoinRequested_t* pCallback)
{
    SteamMatchmaking()->JoinLobby(pCallback->m_steamIDLobby);
    network_status = NetworkStatus::connected;
}

std::vector<std::string> SteamTools::SteamManager::GetLobbyMemberNames()
{
    std::vector<std::string> names;
    for (int i = 0; i < SteamMatchmaking()->GetNumLobbyMembers(GetCurrentLobby()); i++)
    {
        names.push_back(SteamFriends()->GetFriendPersonaName(SteamMatchmaking()->GetLobbyMemberByIndex(GetCurrentLobby(), i)));
    }
    return names;
}

void SteamTools::SteamManager::MatchmakeLoop_Quick(int new_lobby_max_players) {
    switch (network_status) {
    case SteamTools::NetworkStatus::offline:
        network_position = NetworkPosition::local;
        break;
    case SteamTools::NetworkStatus::searching:
        if (GetLobbiesFound() != -1) {
            if (GetLobbiesFound() == 0) {
            } else {
                for (int i = 0; i < GetLobbiesFound() && network_status == SteamTools::NetworkStatus::searching; i++) {
                    std::string bloom_game_name = SteamMatchmaking()->GetLobbyData(SteamMatchmaking()->GetLobbyByIndex(i), "BloomGameName");
                    if (bloom_game_name == "Shooter") {
                        network_status = NetworkStatus::joining;
                        JoinLobbyByIdx(i);
                    }
                }
                if (network_status == SteamTools::NetworkStatus::searching) {
                    CreateLobby(new_lobby_max_players);
                    network_status = NetworkStatus::creating;
                }
            }
        }
        break;
    case SteamTools::NetworkStatus::joining:
        break;
    case SteamTools::NetworkStatus::creating:
        break;
    case SteamTools::NetworkStatus::connected:
        if(network_position == NetworkPosition::local)
            network_position = NetworkPosition::lobby;
        break;
    }
}

void SteamTools::SteamManager::HostP2P(int channel)
{
    SteamNetworkingSockets()->CreateListenSocketP2P(channel,0,nullptr);
    network_position = NetworkPosition::host;
    SteamMatchmaking()->SetLobbyJoinable(GetCurrentLobby(), false);
}

void SteamTools::SteamManager::ConnectP2P(CSteamID remoteID,int channel)
{
    auto target = IdentityFromSteamID(remoteID);
    SteamNetworkingSockets()->ConnectP2P(target, channel, 0, nullptr);
}

SteamNetworkingIdentity SteamTools::SteamManager::IdentityFromSteamID(CSteamID id)
{
    SteamNetworkingIdentity target;
    target.SetSteamID(id);
    return target;
}

void SteamTools::SteamManager::OnSteamNetConnectionStatusChangedCallback_t(SteamNetConnectionStatusChangedCallback_t* pCallback) {
    // CONNECTION REQUEST TO ME
    if (pCallback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
        SteamNetworkingSockets()->AcceptConnection(pCallback->m_hConn);
        remotes.push_back(pCallback->m_info.m_identityRemote);
        //std::cout << "Remotes " << remotes.size() << "\n";
        if (network_position == host) {
            if (SteamMatchmaking()->GetNumLobbyMembers(current_lobby) == 1) {
                SteamMatchmaking()->LeaveLobby(current_lobby);
            }
        }
        else {
            SteamMatchmaking()->LeaveLobby(current_lobby);
        }

    } // CONNECTION ACCEPTED BY PEER
    else if(pCallback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected) {
        if (network_position != host) {
            network_position = NetworkPosition::remote;
        }
    } // CONNECTION REJECTED BY PEER
    else if (pCallback->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer) {
        if (network_position != host) {
            SteamMatchmaking()->LeaveLobby(current_lobby);
            network_status = NetworkStatus::offline;
            network_position = NetworkPosition::local;
            remotes.clear();
        }
        else if (remotes.size() == 0) {
            SteamMatchmaking()->LeaveLobby(current_lobby);
            network_status = NetworkStatus::offline;
            network_position = NetworkPosition::local;
            remotes.clear();
        }
        else {
            for (int i = 0; i < remotes.size(); i++)
            {
                if (remotes.at(i).GetIPv4() == pCallback->m_info.m_identityRemote.GetIPv4()) {
                    remotes.erase(remotes.begin() + i);
                    break;
                }
            }

            if (remotes.size() == 0) {
                SteamMatchmaking()->LeaveLobby(current_lobby);
                network_status = NetworkStatus::offline;
                network_position = NetworkPosition::local;
                remotes.clear();
            }
        }
    } // PROBLEM DETECTED ?
    else if(pCallback->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
        SteamMatchmaking()->LeaveLobby(current_lobby);
        network_status = NetworkStatus::offline;
        network_position = NetworkPosition::local;
        remotes.clear();
    }
}

std::string SteamTools::SteamManager::PreparePayload(const char* data, DataType type)
{
    std::string paylod = (std::to_string(type) + PACKET_DELIMTER + std::to_string(strlen(data)).c_str() + PACKET_DELIMTER + DATA_EDGE + std::string(data) + DATA_EDGE + PACKET_DELIMTER + PACKET_END);
    return paylod;
}

SteamTools::Message SteamTools::SteamManager::ParsePayload(std::string data)
{
    std::cout << "PARSING {" << data << "}\n";
    size_t pos = 0;
    std::string token;
    Message output = Message();
    std::string msg_data;
    bool appending = false;
    while ((pos = data.find(PACKET_DELIMTER)) != std::string::npos) {
        token = data.substr(0, pos);

        if (output.type == DataType::UNINIT)
            output.type = DataType(std::stoi(token));
        else if (output.length == -1)
            output.length = std::stoi(token);
        else if (token[0] == '"') {
            appending = !appending;
            msg_data += token.substr(1, token.size() - 1) + PACKET_DELIMTER;
        }
        else if (token.back() == '"') {
            appending = !appending;
            msg_data += token.substr(0, token.size() - 1);
            output.data = msg_data;
        }
        else if(appending)
            msg_data += token + PACKET_DELIMTER;
        
        data.erase(0, pos + std::strlen(PACKET_DELIMTER));
    }
    if (data != "ENDDATA")
        std::cout << "PARSING ERROR!\n";
    return output;
}

bool SteamTools::SteamManager::SendMessageP2P_Reliable(int remote_idx, const char* data,int channel)
{
    std::string payload_string = PreparePayload(data);
    const void* payload = payload_string.c_str();
    auto result = SteamNetworkingMessages()->SendMessageToUser(remotes.at(remote_idx), payload, strlen(payload_string.c_str()), 8, channel);
    return result == EResult::k_EResultOK;
}

bool SteamTools::SteamManager::SendMessageP2P_Reliable(SteamNetworkingIdentity remote, const char* data, int channel)
{
    std::string payload_string = PreparePayload(data);
    const void* payload = payload_string.c_str();
    auto result = SteamNetworkingMessages()->SendMessageToUser(remote, payload, strlen(payload_string.c_str()), 8, channel);
    return result == EResult::k_EResultOK;
}

bool SteamTools::SteamManager::SendMessageP2P_Unreliable(int remote_idx, const char* data, int channel)
{
    std::string payload_string = PreparePayload(data);
    const void* payload = payload_string.c_str();
    auto result = SteamNetworkingMessages()->SendMessageToUser(remotes.at(remote_idx), payload, strlen(payload_string.c_str()), 0, channel);
    return result == EResult::k_EResultOK;
}

bool SteamTools::SteamManager::SendMessageP2P_Unreliable(SteamNetworkingIdentity remote, const char* data, int channel)
{
    std::string payload_string = PreparePayload(data);
    const void* payload = payload_string.c_str();
    auto result = SteamNetworkingMessages()->SendMessageToUser(remote, payload, strlen(payload_string.c_str()), 0, channel);
    return result == EResult::k_EResultOK;
}

void SteamTools::SteamManager::ReceiveMessageP2P(int channel)
{
    SteamNetworkingMessage_t** msgs = new SteamNetworkingMessage_t*[1];
    int received_amount = SteamNetworkingMessages()->ReceiveMessagesOnChannel(channel, msgs, 1);
    if (received_amount != 0) {
        for (int i = 0; i < (sizeof(msgs) / sizeof(msgs[0])); i++)
        {
            std::cout << "RECEIVE MSG : \"" << ParsePayload(std::string(static_cast<const char*>(msgs[i]->GetData()))).data << "\"\n";
            msgs[i]->Release();
        }
    }
}

void SteamTools::SteamManager::OnSteamNetworkingMessagesSessionRequest_t(SteamNetworkingMessagesSessionRequest_t* pCallback) {
    std::cout << "Accepted Session Request from " << SteamFriends()->GetFriendPersonaName(pCallback->m_identityRemote.GetSteamID()) << "!\n";
    SteamNetworkingMessages()->AcceptSessionWithUser(pCallback->m_identityRemote);
}