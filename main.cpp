#include "websocket/websocket.h"
// #include "ws_client_test.cpp"

int main() {
    WSServer& server = WSServer::Get();
    int port = 9002;

    if (server.listen(port)) {
        std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
        server.handle_packet();

        // Keep the main thread running
        std::this_thread::sleep_for(std::chrono::hours(24));
    } else {
        std::cerr << "Failed to start the server." << std::endl;
    }

    // Client code version

    // std::string uri = "ws://localhost:9002";
    // std::string auth_token = "your_secret_token";

    // WSClient client;
    // client.connect(uri, auth_token);

    return 0;
}