#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	WSADATA wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult = NULL;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;

	const char* sendBuffer = "Hello from Server!";
	char recvBuffer[512];

	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		cout << "WSAStartup failed, result = " << result << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, "6666", &hints, &addrResult);
	if (result != 0) {
		cout << "getaddrinfo failed, result = " << result << endl;
		WSACleanup();
		return 1;
	}
	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // делаем сокет
	if (ListenSocket == INVALID_SOCKET) 
	{
		cout << "Socket creation failed" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // подключаемся
	if (result == SOCKET_ERROR) {
		cout << "Binding socket failed!" << endl;
		closesocket(ListenSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		cout << "listening socket failed!" << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << "accepting socket failed!" << endl;
		closesocket(ListenSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	closesocket(ListenSocket);

	
	do {
		ZeroMemory(recvBuffer, sizeof(recvBuffer));
		result = recv(ClientSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Recieved: " << result << " bytes" << endl;
			cout << "Recieved data: " << recvBuffer << endl;

			result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
			if (result == SOCKET_ERROR) {
				cout << "failed to send data to client" << endl;
				closesocket(ClientSocket);
				freeaddrinfo(addrResult);
				WSACleanup();
				return 1;
			}
		 }
		else if (result == 0) {
			cout << "Connection closed!" << endl;
		}
		else {
			cout << "recv failed with error " << result << endl;
			closesocket(ClientSocket);
			freeaddrinfo(addrResult);
			WSACleanup();
			return 1;
		}
	} while (result > 0);

	result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		cout << "shutdown client socket failed" << endl;
	}
	closesocket(ClientSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 0;

}
