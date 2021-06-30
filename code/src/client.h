//
// Created by caspase on 7/1/2021.
//

#ifndef FS_CLIENT_H
#define FS_CLIENT_H
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



/////////////////////////////////////////////////////////////////////////////
//
// ChatClient
//
/////////////////////////////////////////////////////////////////////////////

class ChatClient
{
public:
    void Run( const SteamNetworkingIPAddr &serverAddr );
private:

    HSteamNetConnection m_hConnection;
    ISteamNetworkingSockets *m_pInterface;

    void PollIncomingMessages();

    void PollLocalUserInput();

    void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

    static void SteamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo );
    void PollConnectionStateChanges();

};

void PrintUsageAndExit( int rc = 1 );

int client();

#endif //FS_CLIENT_H
