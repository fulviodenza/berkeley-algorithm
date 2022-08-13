#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "config.h"

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024];
    char *hello = "Hello from server";
    int r = rand() % 10;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("socket configuration failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("waiting for a client to connect...");
    while (true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("connection accept");

        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("hello message sent\n");

        printf("client disconnected\n");
        // closing the connected socket
        close(new_socket);
    }

    printf("closing server\n");
    shutdown(server_fd, SHUT_RDWR);
}
