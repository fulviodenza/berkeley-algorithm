#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#define EXTEND(array, number) realloc(array, sizeof(int) * number)

int calculate_average(int *clocks, int clocks_len, int server_clock);

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    int server_fd, new_socket, valread;

    int count_clients = 0;
    int *client_socket = malloc(sizeof(int) * count_clients);
    int *client_clocks = malloc(sizeof(int) * count_clients);

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    int opt = 1;

    char buffer[BUFFER_LEN];
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

    printf("waiting for a client to connect...\n");
    while (true)
    {
        memset(buffer, '\0', BUFFER_LEN);
        while (count_clients < NUM_CLIENTS)
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                     (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("connection accept\n");

            count_clients++;
            client_socket = EXTEND(client_socket, count_clients);
            client_clocks = EXTEND(client_clocks, count_clients);

            client_socket[count_clients - 1] = new_socket;
        }

        for (int i = 0; i < count_clients; i++)
        {
            valread = read(client_socket[i], buffer, BUFFER_LEN);
            printf("message payload: %s\n", buffer);

            int n = atoi(buffer);
            client_clocks[i] = n;
        }

        char *avg = calloc(BUFFER_LEN, sizeof(char));
        snprintf(avg, BUFFER_LEN, "%d", calculate_average(client_clocks, count_clients, r));

        for (int i = 0; i < count_clients; i++)
        {
            send(client_socket[i], avg, strlen(avg), 0);
            printf("%s message sent to client %d\n", avg, i);

            close(client_socket[i]);
            printf("client disconnected\n");
        }

        free(avg);
        count_clients = 0;
    }

    printf("closing server\n");
    shutdown(server_fd, SHUT_RDWR);
}

int calculate_average(int *clocks, int clocks_len, int server_clock)
{
    double avg = 0;
    for (int i = 0; i < clocks_len; i++)
    {
        avg += clocks[i];
    }

    return avg / clocks_len;
}