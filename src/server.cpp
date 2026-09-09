#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv) {
    const int port = argc > 1 ? std::stoi(argv[1]) : 8080;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { std::cerr << "socket: " << std::strerror(errno) << '\n'; return 1; }

    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) return 1;
    if (listen(server_fd, 8) < 0) return 1;

    std::cout << "Listening on port " << port << '\n';
    while (true) {
        int client = accept(server_fd, nullptr, nullptr);
        if (client < 0) continue;

        char buffer[1024];
        ssize_t count = read(client, buffer, sizeof(buffer));
        if (count > 0) write(client, buffer, static_cast<size_t>(count));
        close(client);
    }
}
