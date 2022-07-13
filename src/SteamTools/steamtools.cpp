#include "steamtools.h"

SteamTools::SteamManager::SteamManager() {}

bool SteamTools::SteamManager::Init(int appid)
{
	if (SteamAPI_RestartAppIfNecessary(appid))
		return false;
	SteamAPI_Init();
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
	m_LobbyMatchList_t.Set(SteamMatchmaking()->RequestLobbyList(), this, &SteamManager::OnLobbyListFound);
	this->found_lobbies = -1;
}

void SteamTools::SteamManager::OnLobbyListFound(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure) {
	this->found_lobbies = pLobbyMatchList->m_nLobbiesMatching;
}

void SteamTools::SteamManager::OnLobbyEnter(LobbyEnter_t* pCallback)
{
	this->lobby_connection_success = pCallback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess;
}

void SteamTools::SteamManager::OnLobbyCreated(LobbyCreated_t* pCallback)
{
	this->created_lobby_id = pCallback->m_ulSteamIDLobby;
}