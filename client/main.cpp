
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <cstdio>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

const short BUFF_SIZE = 1024;

int main() {
    WSAData data;

    int error_stat = WSAStartup(MAKEWORD(2, 2), &data);
    if (error_stat != 0) {
        std::cout << "Error sock initialization" << WSAGetLastError();
        return 1;
    }
    else {
        std::cout << "Sock Initialization is OK \n";
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cout << "Error initialization socket " << WSAGetLastError() << "\n";
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "Socket initialization is OK \n";
    }

    in_addr ip_num{};

    error_stat = inet_pton(AF_INET, "127.0.0.1", &ip_num);
    if (error_stat <= 0) {
        std::cout << "Error Ip translation \n";
        return 1;
    }

    sockaddr_in server_info{};
    ZeroMemory(&server_info, sizeof(server_info));

    server_info.sin_family = AF_INET;
    server_info.sin_addr = ip_num;
    server_info.sin_port = htons(1024);

    error_stat = connect(client_socket, (sockaddr*)&server_info, sizeof(server_info));
    if (error_stat != 0) {
        std::cout << "Connection to Server is FAILED. Error " << WSAGetLastError() << "\n";
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "Connection SUCCESSFULLY. Ready to send a message to Server \n";
    }

    std::vector<char> server_buffer(BUFF_SIZE);
    std::vector<char> client_buffer(BUFF_SIZE);
    short packet_size = 0;

    while (true) {
        std::cout << "Your message to Server: ";
        fgets(client_buffer.data(), client_buffer.size(), stdin);

        if (client_buffer[0] == 'x' && client_buffer[1] == 'x' && client_buffer[2] == 'x') {
            shutdown(client_socket, SD_BOTH);
            closesocket(client_socket);
            WSACleanup();
            return 0;
        }

        packet_size = send(client_socket, client_buffer.data(), client_buffer.size(), 0);

        if (packet_size == SOCKET_ERROR) {
            std::cout << "Can't send message to Client. Error " << WSAGetLastError() << "\n";
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }

        packet_size = recv(client_socket, server_buffer.data(), server_buffer.size(), 0);

        if (packet_size == SOCKET_ERROR) {
            std::cout << "Can't receive message from Server. Error " << WSAGetLastError() << "\n";
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }
        else {
            std::cout << "Server message: " << server_buffer.data() << "\n";
        }
    }

    closesocket(client_socket);
    WSACleanup();
}