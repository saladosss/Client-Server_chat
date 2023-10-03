#include "chat.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using chat::ChatService;
using chat::Request;
using chat::Response;

class Client {
public:
    Client(std::shared_ptr<Channel> channel)
        : stub_{ChatService::NewStub(channel)} {}

    std::string SendMessage(const std::string& request_field) {
        Request request;
        request.set_request_field(request_field);

        Response response;
        ClientContext client_context;
        Status status = stub_->SendMessage(&client_context, request, &response);

        if (status.ok()) {
            return response.response_field();
        }
        else {
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<ChatService::Stub> stub_;
};

int main(int argc, char** argv) {
    std::string server_address{"localhost:2510"};
    Client client{grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())};
    std::string request_filed{"word"};
    std::string response_field = client.SendMessage(request_filed);
    std::cout << "Client received: " << response_field << std::endl;

    return 0;
}


