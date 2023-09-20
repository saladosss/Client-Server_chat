#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

const short BUFF_SIZE = 1024;

int main() {
    WSAData data;

    int error_stat = WSAStartup(MAKEWORD(2, 2), &data);
    if (error_stat != 0) {
        std::cout << "Error sock initialization. Error " << WSAGetLastError();
        return 1;
    }
    else {
        std::cout << "Sock Initialization is OK \n";
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cout << "Error initialization socket. Error " << WSAGetLastError() << "\n";
        closesocket(server_socket);
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

    error_stat = bind(server_socket, (sockaddr*)&server_info, sizeof(server_info));
    if (error_stat != 0) {
        std::cout << "Error socket binding to server info. Error " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "Binding socket to server is OK \n";
    }

    error_stat = listen(server_socket, SOMAXCONN);
    if (error_stat != 0) {
        std::cout << "Can't start to listen to. Error " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "Listening... \n";
    }

    sockaddr_in client_info{};

    int client_info_size = sizeof(client_info);
    ZeroMemory(&client_info, sizeof(client_info));

    SOCKET client_conn = accept(server_socket, (sockaddr*)&client_info, &client_info_size);
    if (client_conn == INVALID_SOCKET) {
        std::cout << "Client detected, but cant connect to a client. Error " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        closesocket(client_conn);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "Connection to a client successfully \n";
    }

    std::vector<char> server_buffer(BUFF_SIZE);
    std::vector<char> client_buffer(BUFF_SIZE);
    short packet_size = 0;

    while (true) {
        packet_size = recv(client_conn, server_buffer.data(), server_buffer.size(), 0);

        std::cout << "Client message: " << server_buffer.data() << "\n";

        std::cout << "Your message: ";
        fgets(client_buffer.data(), client_buffer.size(), stdin);

        if (client_buffer[0] == 'x' && client_buffer[1] == 'x' && client_buffer[2] == 'x') {
            shutdown(client_conn, SD_BOTH);
            closesocket(server_socket);
            closesocket(client_conn);
            WSACleanup();
            return 0;
        }

        packet_size = send(client_conn, client_buffer.data(), client_buffer.size(), 0);

        if (packet_size == SOCKET_ERROR) {
            std::cout << "Can't send message to Client. Error " << WSAGetLastError() << "\n";
            closesocket(server_socket);
            closesocket(client_conn);
            WSACleanup();
            return 1;
        }
    }

    closesocket(server_socket);
    closesocket(client_conn);
    WSACleanup();
}