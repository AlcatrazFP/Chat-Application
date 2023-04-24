#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

using namespace std;

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0}, file_buffer[1024] = {0};
    string message;

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connecting to the server
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Sending and receiving messages
    cout << "Connected to server" << endl;
    cout << "Type 'file' to send a file, or type your message: " << endl;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        cout << "Client: ";
        getline(cin, message);
        send(sock, message.c_str(), message.length(), 0);
        if (message == "file") {
            // Sending file
            string file_name;
            cout << "Enter file name: ";
            getline(cin, file_name);
            ifstream file(file_name.c_str(), ios::binary);
            if (!file.is_open()) {
                cout << "File not found" << endl;
                continue;
            }
            send(sock, file_name.c_str(), file_name.length(), 0);
            while (!file.eof()) {
                file.read(file_buffer, 1024);
                send(sock, file_buffer, file.gcount(), 0);
            }
            file.close();
        }
            else {
            // Receiving message
            memset(buffer, 0, sizeof(buffer));
            valread = read(sock, buffer, 1024);
            if (valread <= 0) {
                break;
            }
            cout << "Server: " << buffer << endl;
        }
    }
    close(sock);
    return 0;
}
