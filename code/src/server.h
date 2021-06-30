//
// Created by caspase on 6/28/2021.
//

//====== Copyright Valve Corporation, All rights reserved. ====================
//
// Example client/server chat application using SteamNetworkingSockets

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <map>
#include <cctype>

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <steam/steam_api.h>
#endif

#ifdef WIN32
#include <windows.h> // Ug, for NukeProcess -- see below
#include <iostream>

#else
#include <unistd.h>
	#include <signal.h>
#endif


class ChatServer
{
public:
    void Run( uint16 nPort );
private:

    HSteamListenSocket m_hListenSock;
    HSteamNetPollGroup m_hPollGroup;
    ISteamNetworkingSockets *m_pInterface;

    struct Client_t
    {
        std::string m_sNick;
    };

    std::map< HSteamNetConnection, Client_t > m_mapClients;

    void SendStringToClient( HSteamNetConnection conn, const char *str );

    void SendStringToAllClients( const char *str, HSteamNetConnection except = k_HSteamNetConnection_Invalid );

    void PollIncomingMessages();

    void PollLocalUserInput();

    void SetClientNick( HSteamNetConnection hConn, const char *nick );

    void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

    static ChatServer *s_pServerCallbackInstance;
    static void SteamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo );

    void PollConnectionStateChanges();
};

int server();