
#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

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
	std::cout << "ai_next: " << ai.ai_next << '\n';
	std::cout << '\n';
}

int main()
{
	// intializing winsock
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartupfailed: %d\n", iResult);
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

	iResult = getaddrinfo("www.google.com", "27015", &hints, &result);
	if(iResult != 0)
	{
		std::cout << "getaddrinfo failed: " << iResult << '\n';
		WSACleanup();
		return 1;
	}
	std::cout << "hints: "; 
	printAddrInfo(hints);
	std::cout << "result: ";
	printAddrInfo(*result);

	SOCKET ConnectSocket = INVALID_SOCKET;
	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	std::cout << "ConnectSocket: " << ConnectSocket << '\n';
	std::cout << '\n';

	if(ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << WSAGetLastError();
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	std::cout << "ptr: ";
	printAddrInfo(*ptr);

	// connect to socket
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if(iResult == SOCKET_ERROR)
	{
		std::cout << "connect error: " << WSAGetLastError() << '\n';
		std::cout << '\n';
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if(ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Unable to connect to server!\n";
		WSACleanup();
		return 1;
	}


}
