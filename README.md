# WebSocket-Demo

Simple websocket server build using websocket and asio libraries.

## Requirements

- CMake 3.25 or higher
- Git
- Compiler ( GCC, Clang, MSVC )
- a bit cells of your brain lmao

## Building

```bash
git clone https://github.com/yoruakio/WebSocket-Server.git
cd WebSocket-Server && mkdir build && cd build
cmake ..
cmake --build . && ./WebSocketServer
```

## Information

This is client code for the WebSocket-Server. It is a simple websocket client that can be used to demonstrate how to use websockets in C++.

```cpp
typedef websocketpp::client<websocketpp::config::asio_client> client;

std::string uri = "ws://localhost:9002";
std::string auth_token = "your_secret_token"; // Replace with your actual token

client c;
c.init_asio();

c.set_open_handler([&c](websocketpp::connection_hdl hdl) {
    std::cout << "Connection opened" << std::endl;
    c.send(hdl, "Hello, Server!", websocketpp::frame::opcode::text);
});
c.set_message_handler([](websocketpp::connection_hdl, client::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
});

websocketpp::lib::error_code ec;
client::connection_ptr con = c.get_connection(uri, ec);

if (ec) {
    std::cout << "Could not create connection because: " << ec.message() << std::endl;
    return 1;
}

con->append_header("Authorization", auth_token);

c.connect(con);
c.run();
```

## Contact

If you have any questions or suggestions, feel free to contact me at:

- Discord: [@yoruakio](https://discord.com/users/919841186246692886)
- Telegram: [@yoruakio](https://t.me/yoruakio) or [HTF](https://t.me/htf_public)

## LICENSE

WebSocket-Server is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
