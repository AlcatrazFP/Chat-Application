#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>

#define PORT 8080

using namespace std;

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char file_buffer[1024];
    string file_name;
    ifstream file;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket to the specified port
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accepting incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                             (socklen_t *) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Receiving and sending messages
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            break;
        }
        if (strcmp(buffer, "file") == 0) {
            // Receiving file
            memset(buffer, 0, sizeof(buffer));
            valread = read(new_socket, buffer, 1024);
            if (valread <= 0) {
                break;
            }
            file_name = buffer;
            file.open(file_name.c_str(), ios::binary);
            if (!file.is_open()) {
                cout << "File not found" << endl;
                continue;
            }
            while (!file.eof()) {
                file.read(file_buffer, 1024);
                write(new_socket, file_buffer, file.gcount());
            }
            file.close();
        } else {
            // Sending message
            cout << "Client: " << buffer << endl;
            string reply;
            cout << "Server: ";
            getline(cin, reply);
            send(new_socket, reply.c_str(), reply.length(), 0);
        }
    }
    close(new_socket);
    close(server_fd);
    return 0;
}
