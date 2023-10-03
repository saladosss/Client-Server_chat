#include "chat.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using chat::ChatService;
using chat::Response;
using chat::Request;

class ChatServiceImpl final : public ChatService::Service {
    Status SendMessage(ServerContext* context, const Request* request, Response* response) override {
        response->set_response_field("Hello " + request->request_field());
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

int main(int argc, char** argv) {
    RunServer();
    return 0;
}