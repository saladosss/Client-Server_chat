#include "chat.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <iostream>
#include <thread>

using grpc::Channel;
using grpc::ClientReaderWriter;
using grpc::ClientReader;
using grpc::ClientContext;
using grpc::Status;
using chat::ChatService;
using chat::Request;
using chat::Response;

class Client {
public:
    Client(std::shared_ptr<Channel> channel)
        : stub_{ChatService::NewStub(channel)} {}
public:
    bool SendMessage(std::string message) {
      /*  Request request;
        request.set_content(message);

        Response response;

        Status status = stub_->SendMessage(&context, request, &response);

        if (status.ok()) {
            return response.content();
        }
        else {
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }*/

        ClientContext context;
        std::shared_ptr<ClientReaderWriter<Request, Response>> stream(stub_->SendMessage(&context));

        std::thread writer([&] () {
            Request request;
//            request.set_content(message);
            request.add_content(message);
            stream->Write(request);

            stream->WritesDone();
        });

        std::thread reader([&] () {
            Response response;
            while(stream->Read(&response)) {
//                std::cout << "Server message: " << response.content() << std::endl;
                    for (const auto& server_message : response.content()) {
                        std::cout << "Server message: " << server_message << std::endl;
                    }
            }
        });

        writer.join();
        reader.join();

        Status status = stream->Finish();
        if (!status.ok()) {
            std::cout << status.error_message() << " " << status.error_code() << std::endl;
            return false;
        }

        return true;
    }

private:
    std::unique_ptr<ChatService::Stub> stub_;
};

int main() {
    std::string server_address{"localhost:2510"};
    Client client{grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())};
//    client.SendMessage();
//
    while (true) {
        std::cout << "Your message: ";
        std::string message;
//        std::cin >> message;
        getline(std::cin, message);

//        std::cout << "Server message: " << client.SendMessage(message) << std::endl;
        if (!client.SendMessage(message)) {
            break;
        }
    }

    return 0;
}


