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

#include "server.h"
/////////////////////////////////////////////////////////////////////////////
//
// Common stuff
//
/////////////////////////////////////////////////////////////////////////////

bool g_bServerQuit = false;

SteamNetworkingMicroseconds g_ServerLogTimeZero;

// We do this because I won't want to figure out how to cleanly shut
// down the thread that is reading from stdin.
static void NukeProcess(int rc) {
#ifdef WIN32
    ExitProcess(rc);
#else
    (void)rc; // Unused formal parameter
        kill( getpid(), SIGKILL );
#endif
}

static void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char *pszMsg) {
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - g_ServerLogTimeZero;
    printf("%10.6f %s\n", time * 1e-6, pszMsg);
    fflush(stdout);
    if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug) {
        fflush(stdout);
        fflush(stderr);
        NukeProcess(1);
    }
}

static void FatalError(const char *fmt, ...) {
    char text[2048];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);
    char *nl = strchr(text, '\0') - 1;
    if (nl >= text && *nl == '\n')
        *nl = '\0';
    DebugOutput(k_ESteamNetworkingSocketsDebugOutputType_Bug, text);
}

static void Printf(const char *fmt, ...) {
    char text[2048];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);
    char *nl = strchr(text, '\0') - 1;
    if (nl >= text && *nl == '\n')
        *nl = '\0';
    DebugOutput(k_ESteamNetworkingSocketsDebugOutputType_Msg, text);
}

static void InitSteamDatagramConnectionSockets() {
#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg))
        FatalError("GameNetworkingSockets_Init failed.  %s", errMsg);
#else
    SteamDatagramClient_SetAppID( 570 ); // Just set something, doesn't matter what
        //SteamDatagramClient_SetUniverse( k_EUniverseDev );

        SteamDatagramErrMsg errMsg;
        if ( !SteamDatagramClient_Init( true, errMsg ) )
            FatalError( "SteamDatagramClient_Init failed.  %s", errMsg );

        // Disable authentication when running with Steam, for this
        // example, since we're not a real app.
        //
        // Authentication is disabled automatically in the open-source
        // version since we don't have a trusted third party to issue
        // certs.
        SteamNetworkingUtils()->SetGlobalConfigValueInt32( k_ESteamNetworkingConfig_IP_AllowWithoutAuth, 1 );
#endif

    g_ServerLogTimeZero = SteamNetworkingUtils()->GetLocalTimestamp();

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, DebugOutput);
}

static void ShutdownSteamDatagramConnectionSockets() {
    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
    GameNetworkingSockets_Kill();
#else
    SteamDatagramClient_Kill();
#endif
}

/////////////////////////////////////////////////////////////////////////////
//
// Non-blocking console user input.  Sort of.
// Why is this so hard?
//
/////////////////////////////////////////////////////////////////////////////

std::mutex ServerMutexUserInputQueue;
std::queue<std::string> serverQueueUserInput;

std::thread *s_pServerThreadUserInput = nullptr;

inline void LocalUserInput_Init() {
    s_pServerThreadUserInput = new std::thread([]() {
        while (!g_bServerQuit) {
            char szLine[4000];
            if (!fgets(szLine, sizeof(szLine), stdin)) {
                // Well, you would hope that you could close the handle
                // from the other thread to trigger this.  Nope.
                if (g_bServerQuit)
                    return;
                g_bServerQuit = true;
                Printf("Failed to read on stdin, quitting\n");
                break;
            }

            ServerMutexUserInputQueue.lock();
            serverQueueUserInput.push(std::string(szLine));
            ServerMutexUserInputQueue.unlock();
        }
    });
}

ChatServer *ChatServer::s_pServerCallbackInstance = nullptr;
//void LocalUserInput_Kill()
//{
// Does not work.  We won't clean up, we'll just nuke the process.
//	g_bServerQuit = true;
//	_close( fileno( stdin ) );
//
//	if ( s_pServerThreadUserInput )
//	{
//		s_pServerThreadUserInput->join();
//		delete s_pServerThreadUserInput;
//		s_pServerThreadUserInput = nullptr;
//	}
//}

// You really gotta wonder what kind of pedantic garbage was
// going through the minds of people who designed std::string
// that they decided not to include trim.
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


// Read the next line of input from stdin, if anything is available.
inline bool LocalUserInput_GetNext(std::string &result) {
    bool got_input = false;
    ServerMutexUserInputQueue.lock();
    while (!serverQueueUserInput.empty() && !got_input) {
        result = serverQueueUserInput.front();
        serverQueueUserInput.pop();
        ltrim(result);
        rtrim(result);
        got_input = !result.empty(); // ignore blank lines
    }
    ServerMutexUserInputQueue.unlock();
    return got_input;
}

/////////////////////////////////////////////////////////////////////////////
//
// ChatServer
//
/////////////////////////////////////////////////////////////////////////////


void ChatServer::Run(uint16 nPort) {
    // Select instance to use.  For now we'll always use the default.
    // But we could use SteamGameServerNetworkingSockets() on Steam.
    m_pInterface = SteamNetworkingSockets();

    // Start listening
    SteamNetworkingIPAddr serverLocalAddr;
    serverLocalAddr.Clear();
    serverLocalAddr.m_port = nPort;
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
               (void *) SteamNetConnectionStatusChangedCallback);
    m_hListenSock = m_pInterface->CreateListenSocketIP(serverLocalAddr, 1, &opt);
    if (m_hListenSock == k_HSteamListenSocket_Invalid)
        FatalError("Failed to listen on port %d", nPort);
    m_hPollGroup = m_pInterface->CreatePollGroup();
    if (m_hPollGroup == k_HSteamNetPollGroup_Invalid)
        FatalError("Failed to listen on port %d", nPort);
    Printf("Server listening on port %d\n", nPort);

    while (!g_bServerQuit) {
        PollIncomingMessages();
        PollConnectionStateChanges();
        PollLocalUserInput();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Close all the connections
    Printf("Closing connections...\n");
    for (auto it: m_mapClients) {
        // Send them one more goodbye message.  Note that we also have the
        // connection close reason as a place to send final data.  However,
        // that's usually best left for more diagnostic/debug text not actual
        // protocol strings.
        SendStringToClient(it.first, "Server is shutting down.  Goodbye.");

        // Close the connection.  We use "linger mode" to ask SteamNetworkingSockets
        // to flush this out and close gracefully.
        m_pInterface->CloseConnection(it.first, 0, "Server Shutdown", true);
    }
    m_mapClients.clear();

    m_pInterface->CloseListenSocket(m_hListenSock);
    m_hListenSock = k_HSteamListenSocket_Invalid;

    m_pInterface->DestroyPollGroup(m_hPollGroup);
    m_hPollGroup = k_HSteamNetPollGroup_Invalid;
}

void ChatServer::SendStringToClient(HSteamNetConnection conn, const char *str) {
    m_pInterface->SendMessageToConnection(conn, str, (uint32) strlen(str), k_nSteamNetworkingSend_Reliable, nullptr);
}

void ChatServer::SendStringToAllClients(const char *str, HSteamNetConnection except) {
    for (auto &c: m_mapClients) {
        if (c.first != except)
            SendStringToClient(c.first, str);
    }
}

void ChatServer::PollIncomingMessages() {
    char temp[1024];

    while (!g_bServerQuit) {
        ISteamNetworkingMessage *pIncomingMsg = nullptr;
        int numMsgs = m_pInterface->ReceiveMessagesOnPollGroup(m_hPollGroup, &pIncomingMsg, 1);
        if (numMsgs == 0)
            break;
        if (numMsgs < 0)
            FatalError("Error checking for messages");
        assert(numMsgs == 1 && pIncomingMsg);
        auto itClient = m_mapClients.find(pIncomingMsg->m_conn);
        assert(itClient != m_mapClients.end());

        // '\0'-terminate it to make it easier to parse
        std::string sCmd;
        sCmd.assign((const char *) pIncomingMsg->m_pData, pIncomingMsg->m_cbSize);
        const char *cmd = sCmd.c_str();

        // We don't need this anymore.
        pIncomingMsg->Release();

        // Check for known commands.  None of this example code is secure or robust.
        // Don't write a real server like this, please.

        if (strncmp(cmd, "/nick", 5) == 0) {
            const char *nick = cmd + 5;
            while (isspace(*nick))
                ++nick;

            // Let everybody else know they changed their name
            sprintf(temp, "%s shall henceforth be known as %s", itClient->second.m_sNick.c_str(), nick);
            SendStringToAllClients(temp, itClient->first);

            // Respond to client
            sprintf(temp, "Ye shall henceforth be known as %s", nick);
            SendStringToClient(itClient->first, temp);

            // Actually change their name
            SetClientNick(itClient->first, nick);
            continue;
        }

        // Assume it's just a ordinary chat message, dispatch to everybody else
        sprintf(temp, "%s: %s", itClient->second.m_sNick.c_str(), cmd);
        std::cout << itClient->second.m_sNick.c_str() << cmd << std::endl;
        SendStringToAllClients(temp, itClient->first);
    }
}

void ChatServer::PollLocalUserInput() {
    std::string cmd;
    while (!g_bServerQuit && LocalUserInput_GetNext(cmd)) {
        if (strcmp(cmd.c_str(), "/quit") == 0) {
            g_bServerQuit = true;
            Printf("Shutting down server");
            break;
        }

        // That's the only command we support
        Printf("The server only knows one command: '/quit'");
    }
}

void ChatServer::SetClientNick(HSteamNetConnection hConn, const char *nick) {

    // Remember their nick
    m_mapClients[hConn].m_sNick = nick;

    // Set the connection name, too, which is useful for debugging
    m_pInterface->SetConnectionName(hConn, nick);
}

void ChatServer::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) {
    char temp[1024];

    // What's the state of the connection?
    switch (pInfo->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_None:
            // NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
            // Ignore if they were not previously connected.  (If they disconnected
            // before we accepted the connection.)
            if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {

                // Locate the client.  Note that it should have been found, because this
                // is the only codepath where we remove clients (except on shutdown),
                // and connection change callbacks are dispatched in queue order.
                auto itClient = m_mapClients.find(pInfo->m_hConn);
                assert(itClient != m_mapClients.end());

                // Select appropriate log messages
                const char *pszDebugLogAction;
                if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
                    pszDebugLogAction = "problem detected locally";
                    sprintf(temp, "Alas, %s hath fallen into shadow.  (%s)", itClient->second.m_sNick.c_str(),
                            pInfo->m_info.m_szEndDebug);
                } else {
                    // Note that here we could check the reason code to see if
                    // it was a "usual" connection or an "unusual" one.
                    pszDebugLogAction = "closed by peer";
                    sprintf(temp, "%s hath departed", itClient->second.m_sNick.c_str());
                }

                // Spew something to our own log.  Note that because we put their nick
                // as the connection description, it will show up, along with their
                // transport-specific data (e.g. their IP address)
                Printf("Connection %s %s, reason %d: %s\n",
                       pInfo->m_info.m_szConnectionDescription,
                       pszDebugLogAction,
                       pInfo->m_info.m_eEndReason,
                       pInfo->m_info.m_szEndDebug
                );

                m_mapClients.erase(itClient);

                // Send a message so everybody else knows what happened
                SendStringToAllClients(temp);
            } else {
                assert(pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting);
            }

            // Clean up the connection.  This is important!
            // The connection is "closed" in the network sense, but
            // it has not been destroyed.  We must close it on our end, too
            // to finish up.  The reason information do not matter in this case,
            // and we cannot linger because it's already closed on the other end,
            // so we just pass 0's.
            m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            break;
        }

        case k_ESteamNetworkingConnectionState_Connecting: {
            // This must be a new connection
            assert(m_mapClients.find(pInfo->m_hConn) == m_mapClients.end());

            Printf("Connection request from %s", pInfo->m_info.m_szConnectionDescription);

            // A client is attempting to connect
            // Try to accept the connection.
            if (m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK) {
                // This could fail.  If the remote host tried to connect, but then
                // disconnected, the connection may already be half closed.  Just
                // destroy whatever we have on our side.
                m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                Printf("Can't accept connection.  (It was already closed?)");
                break;
            }

            // Assign the poll group
            if (!m_pInterface->SetConnectionPollGroup(pInfo->m_hConn, m_hPollGroup)) {
                m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                Printf("Failed to set poll group?");
                break;
            }

            // Generate a random nick.  A random temporary nick
            // is really dumb and not how you would write a real chat server.
            // You would want them to have some sort of signon message,
            // and you would keep their client in a state of limbo (connected,
            // but not logged on) until them.  I'm trying to keep this example
            // code really simple.
            char nick[64];
            sprintf(nick, "BraveWarrior%d", 10000 + (rand() % 100000));

            // Send them a welcome message
            sprintf(temp,
                    "Welcome, stranger.  Thou art known to us for now as '%s'; upon thine command '/nick' we shall know thee otherwise.",
                    nick);
            SendStringToClient(pInfo->m_hConn, temp);

            // Also send them a list of everybody who is already connected
            if (m_mapClients.empty()) {
                SendStringToClient(pInfo->m_hConn, "Thou art utterly alone.");
            } else {
                sprintf(temp, "%d companions greet you:", (int) m_mapClients.size());
                for (auto &c: m_mapClients)
                    SendStringToClient(pInfo->m_hConn, c.second.m_sNick.c_str());
            }

            // Let everybody else know who they are for now
            sprintf(temp, "Hark!  A stranger hath joined this merry host.  For now we shall call them '%s'", nick);
            SendStringToAllClients(temp, pInfo->m_hConn);

            // Add them to the client list, using std::map wacky syntax
            m_mapClients[pInfo->m_hConn];
            SetClientNick(pInfo->m_hConn, nick);
            break;
        }

        case k_ESteamNetworkingConnectionState_Connected:
            // We will get a callback immediately after accepting the connection.
            // Since we are the server, we can ignore this, it's not news to us.
            break;

        default:
            // Silences -Wswitch
            break;
    }
}

ChatServer *s_pServerCallbackInstance;

void ChatServer::SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t *pInfo) {
    s_pServerCallbackInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

void ChatServer::PollConnectionStateChanges() {
    s_pServerCallbackInstance = this;
    m_pInterface->RunCallbacks();
}


const uint16 DEFAULT_SERVER_PORT = 27020;

inline void PrintUsageAndExit(int rc = 1) {
    fflush(stderr);
    printf(
            R"usage(Usage:
    example_chat client SERVER_ADDR
    example_chat server [--port PORT]
)usage"
    );
    fflush(stdout);
    exit(rc);
}

int server() {
    const char *argv[] = {"server.exe", "server", "--port", "6789"};
    int argc = 4;
    bool bServer = false;
    bool bClient = false;
    int nPort = DEFAULT_SERVER_PORT;
    SteamNetworkingIPAddr addrServer;
    addrServer.Clear();

    for (int i = 1; i < argc; ++i) {
        if (!bClient && !bServer) {
            if (!strcmp(argv[i], "client")) {
                bClient = true;
                continue;
            }
            if (!strcmp(argv[i], "server")) {
                bServer = true;
                continue;
            }
        }
        if (!strcmp(argv[i], "--port")) {
            ++i;
            if (i >= argc)
                PrintUsageAndExit();
            nPort = atoi(argv[i]);
            if (nPort <= 0 || nPort > 65535)
                FatalError("Invalid port %d", nPort);
            continue;
        }

        // Anything else, must be server address to connect to
        if (bClient && addrServer.IsIPv6AllZeros()) {
            if (!addrServer.ParseString(argv[i]))
                FatalError("Invalid server address '%s'", argv[i]);
            if (addrServer.m_port == 0)
                addrServer.m_port = DEFAULT_SERVER_PORT;
            continue;
        }

        PrintUsageAndExit();
    }

    if (bClient == bServer || (bClient && addrServer.IsIPv6AllZeros()))
        PrintUsageAndExit();

    // Create client and server sockets
    InitSteamDatagramConnectionSockets();
    LocalUserInput_Init();

    {
        ChatServer server;
        server.Run((uint16) nPort);
    }

    ShutdownSteamDatagramConnectionSockets();

    // Ug, why is there no simple solution for portable, non-blocking console user input?
    // Just nuke the process
    //LocalUserInput_Kill();
    NukeProcess(0);
    return 0;
}
