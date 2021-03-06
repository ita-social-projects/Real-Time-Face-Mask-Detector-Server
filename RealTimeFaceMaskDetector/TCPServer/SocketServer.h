#pragma once
#include <ws2tcpip.h>
#include "Logger.h"
#include "SQLServer.h"
#include <mutex>
#include <thread>
#include <future>
#include <filesystem>
#pragma comment (lib, "Ws2_32.lib")
#include <iostream>
const int  DEFAULT_BUFLEN = 512;

enum class ServerStatus { SocketError = -2, NoConnection = -1, Error = 0, Listening, Messaging, ConnectionClosed, ServerIsDown, };

class SocketServer
{
public:
	bool InitSocketServer();
	bool CreateListeningSocket();
	bool StartListening(bool& ret_value);
	bool ShutdownServer();
	int GetMessageLength();

	SocketServer() = default;
	~SocketServer();
private:

	void ConnectToSQL();

	bool BindListeningSocket();
	bool AcceptConnection();
	void TryAcceptAndStartMessaging(ServerStatus& ser_status);
	void SaveAndSendData();
	bool UpdateDataBase();
	void CreateTableIfNeeded(std::unique_ptr<SQLConnection>& sql_server);

	void ReceiveMessage(ServerStatus& ser_status);
	void StartMessagingWintClient(ServerStatus& ser_status);
	bool ReceiveFullMessage(ServerStatus& ser_status);
	void TryReceiveAndSendMessage(ServerStatus& ser_status);

	bool ServerStatusCheck(ServerStatus& ser_status);

	/*return path to TCPServer.exe file*/
	std::filesystem::path GetCurrentPath();

	/*Creates directory for received images*/
	bool SpecifyPathForPhotos();

	/*Creates and opens .png file with specific filename*/
	bool OpenParticularFile(std::ofstream& stream);

	/*This function will take current date & time and
	initialise <file_specificator> with converted date*/
	void CreateFileNameSpecificator(std::string& file_specificator);
	void ReplaceForbiddenSymbol(char& symbol);
	/*Implement mock accept*/
	void MakeAccept();

	WSADATA wsaData;
	int m_func_result;
	SOCKET m_listen_socket = INVALID_SOCKET;
	SOCKET m_client_socket = INVALID_SOCKET;
	std::vector<char> m_buffer;
	addrinfo* m_host_info = nullptr;
	addrinfo hints;
	std::mutex recv_mutex;
	std::mutex sql_mutex;

	std::unique_ptr<SQLConnection>sql_server;

	/*Photo that will be sent to database.
	It's fields will be rewritten with each
	received photo from client*/
	Photo m_photo_to_send;

	/*When listening socket is created, this variable will be set to <true>.
	When Server will shut down, varible will be set to <false> */
	bool server_is_up;
	/*ip and port from config.ini*/
	std::string m_ip;
	std::string m_port;
};