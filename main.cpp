#include "websocket/websocket.h"

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

    return 0;
}