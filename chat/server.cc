#include "chat.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using grpc::Server;
using grpc::ServerBuilder;
using  grpc::ServerReaderWriter;
using grpc::ServerContext;
using grpc::Status;
using chat::ChatService;
using chat::Request;
using chat::Response;

class ChatServiceImpl final : public ChatService::Service {
public:
    Status SendMessage(ServerContext* context, ServerReaderWriter<Response, Request>* stream) override {
        Request request;
        while (stream->Read(&request)) {
            for (const auto& client_message : request.content()) {
                std::cout << "Client message: " << client_message << std::endl;
            }
        }

        std::string message;
        std::cout << "Your message: ";
        getline(std::cin, message);

        Response response;
        response.add_content(message);
        stream->Write(response);

        return Status::OK;
    }
};

void RunServer() {
    std::string server_address{"localhost:2510"};
    ChatServiceImpl service;

    ServerBuilder server_builder;
    server_builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    server_builder.RegisterService(&service);
    std::unique_ptr<Server> server{server_builder.BuildAndStart()};

    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}