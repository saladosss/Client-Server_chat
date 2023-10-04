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
    Status SendMessage(ServerContext* context, ServerReaderWriter<Response, Request>* stream /*const Request* request, Response* response*/) override {
       /* std::cout << "Message from client: " << request->content() << std::endl;

        std::cout << "Your message: ";
        std::string message;
        std::cin >> message;
        response->set_content(message);
        */

        Request request;
        while (stream->Read(&request)) {
            std::unique_lock<std::mutex> lock(mutex_);
            for (const auto& client_message : request.content()) {
                std::cout << "Client message: " << client_message << std::endl;
            }
        }

        std::string message;
        std::cout << "Your message: ";
//        std::cin >> message;
        getline(std::cin, message);

        Response response;
        response.add_content(message);
//        for (const auto& response_ : response.content()) {
//        }
        stream->Write(response);

        return Status::OK;
    }
private:
    std::mutex mutex_;
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