
#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

void printWsaData(const WSADATA &wsaData)
{
	std::cout << "wsaData\n";
	std::cout << "----------------------\n";
	std::cout << "wVersion: " << wsaData.wVersion << '\n';
	std::cout << "wHighVersion: " << wsaData.wHighVersion << '\n';
	std::cout << "iMaxSockets: " << wsaData.iMaxSockets << '\n';
	std::cout << "iMaxUdpDg: " << wsaData.iMaxUdpDg << '\n';
	//std::cout << "lpVendorInfo: " << (wsaData.lpVendorInfo ? wsaData.lpVendorInfo : "null")<< '\n';
	std::cout << "szDescription: " << wsaData.szDescription << '\n';
	std::cout << "szSystemStatus: " << wsaData.szSystemStatus << '\n';
	std::cout << '\n';
}

void printAddrInfo(const addrinfo ai)
{
	std::cout << "addrinfo\n";
	std::cout << "----------------------\n";
	std::cout << "ai_flags: " << ai.ai_flags << '\n';
	std::cout << "ai_family: " << ai.ai_family << '\n';
	std::cout << "ai_socktype: " << ai.ai_socktype << '\n';
	std::cout << "ai_protocol: " << ai.ai_protocol << '\n';
	std::cout << "ai_addrlen: " << ai.ai_addrlen << '\n';
	std::cout << "ai_canonname: " << (ai.ai_canonname ? ai.ai_canonname : "null") << '\n';
	std::cout << "ai_addr: " << ai.ai_addr << '\n';
	if (ai.ai_addr)
	{
		std::cout << "\tai_addr.sa_family: " << (ai.ai_addr)->sa_family << '\n';
		std::cout << "\tai-addr.sa_data: " << (ai.ai_addr)->sa_data << '\n';
	}
	std::cout << "ai_next: " << ai.ai_next << '\n';
	std::cout << '\n';
}

int main(int argc, char** argv)
{

	// intializing winsock
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartupfailed: " << iResult << '\n';
		return 1;
	}
	printWsaData(wsaData);

	// creating socket for client
	struct addrinfo *result = NULL,
			*ptr = NULL,
			hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	printAddrInfo(hints);

	iResult = getaddrinfo("..localmachine", DEFAULT_PORT, &hints, &result);
	if(iResult != 0)
	{
		std::cout << "getaddrinfo failed: " << iResult << '\n';
		WSACleanup();
		return 1;
	}
	std::cout << "getaddrinfo: " << iResult << '\n';
	std::cout << "hints: "; 
	printAddrInfo(hints);
	std::cout << "result: ";
	printAddrInfo(*result);

	SOCKET ConnectSocket = INVALID_SOCKET;
	int count = 0;
	for (ptr = result; ptr!=NULL; ptr = ptr->ai_next)
	{
		std::cout << "count: " << count << '\n';
		printAddrInfo(*ptr);
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			std::cout << "Error at socket(): " << WSAGetLastError();
			WSACleanup();
			return 1;
		}
		std::cout << "ConnectSocket: " << ConnectSocket << "\n\n";
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			std::cout << "connect error: " << WSAGetLastError() << '\n';
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			count++;
			continue;
		}
	}

	freeaddrinfo(result);
	if(ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Unable to connect to server!\n";
		WSACleanup();
		return 1;
	}

	int recvbuflen = DEFAULT_BUFLEN;
	const char *sendbuf = "this is a testttgredgvdgfd";
	char recvbuf[DEFAULT_BUFLEN];
	
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "send failed: " << WSAGetLastError() << '\n';
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "Bytes sents: " << iResult << '\n';

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "shutdown failed: " << WSAGetLastError() << '\n';
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	do
	{
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		//std::cout << "recvbuf: " << recvbuf << '\n';
		if (iResult > 0)
			std::cout << "Bytes received: " << iResult << '\n';
		else if (iResult == 0)
			std::cout << "Connection closed\n";
		else
			std::cout << "recv failed: " << WSAGetLastError() << '\n';
	} while (iResult > 0);

	closesocket(ConnectSocket);
	WSACleanup();
}
