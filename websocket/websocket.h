#pragma once
#define ASIO_STANDALONE

#include <memory>
#include <thread>
#include <atomic>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <string>

typedef websocketpp::server<websocketpp::config::asio> server;

const std::string AUTH_TOKEN = "your_secret_token";
const size_t RATE_LIMIT = 25;   // messages
const size_t TIME_WINDOW = 30;  // seconds

struct ClientInfo {
    size_t message_count;
    std::chrono::steady_clock::time_point start_time;
};

struct connection_hdl_hash {
    std::size_t operator()(const websocketpp::connection_hdl& hdl) const {
        auto ptr = hdl.lock();
        return std::hash<void*>()(ptr.get());
    }
};

struct connection_hdl_equal {
    bool operator()(const websocketpp::connection_hdl& lhs, const websocketpp::connection_hdl& rhs) const {
        return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
    }
};

std::unordered_map<websocketpp::connection_hdl, ClientInfo, connection_hdl_hash, connection_hdl_equal> client_info_map;

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class WSServer {
public:
    WSServer() : running(false) {}
    ~WSServer() {
        stop();
    }

    static WSServer& Get() {
        static WSServer ret;
        return ret;
    }

    bool listen(int port) {
        ws_server = make_unique<server>();

        ws_server->set_http_handler([this](websocketpp::connection_hdl hdl) {
            server::connection_ptr con = ws_server->get_con_from_hdl(hdl);
            std::string auth_token = con->get_request_header("Authorization");

            if (auth_token != AUTH_TOKEN) {
                con->set_status(websocketpp::http::status_code::unauthorized);
                con->set_body("Unauthorized");
            } else {
                con->set_status(websocketpp::http::status_code::ok);
            }
        });

        std::cout << "Listening on port " << port << std::endl;

        ws_server->init_asio();
        ws_server->listen(port);
        ws_server->start_accept();

        running = true;
        server_thread = std::thread([this]() {
            ws_server->run();
        });

        return true;
    }

    void stop() {
        if (running) {
            ws_server->stop_listening();
            ws_server->stop();
            if (server_thread.joinable()) {
                server_thread.join();
            }
            running = false;
        }
    }

    void handle_packet() {
        ws_server->set_message_handler([this](websocketpp::connection_hdl hdl, server::message_ptr msg) {
            auto now = std::chrono::steady_clock::now();
            auto& client_info = client_info_map[hdl];

            if (client_info.message_count == 0) {
                client_info.start_time = now;
            }

            auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - client_info.start_time).count();

            if (elapsed_time > TIME_WINDOW) {
                client_info.message_count = 0;
                client_info.start_time = now;
            }

            if (client_info.message_count >= RATE_LIMIT) {
                std::cerr << "Rate limit exceeded for client" << std::endl;
                ws_server->close(hdl, websocketpp::close::status::policy_violation, "Rate limit exceeded");
                return;
            }

            client_info.message_count++;

            std::cout << "Received message: " << msg->get_payload() << std::endl;

            ws_server->send(hdl, msg->get_payload(), msg->get_opcode());
        });
    }

private:
    std::unique_ptr<server> ws_server;
    std::thread server_thread;
    std::atomic<bool> running;
};