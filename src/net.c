#include <net.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ijvm.h>
#include <stdlib.h>
#include <unistd.h>



word_t net_bind(int port) {
    struct sockaddr_in server;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) return 0;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr *) &server, sizeof(server)) < 0) return 0;
    if (listen(sock, 128) != 0) return 0;

    return accept(sock, NULL, NULL);
}

word_t net_connect(int port, unsigned int host) {
    struct sockaddr_in server;
    char *ipv4_string;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) return 0;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    ipv4_string = inet_ntoa((struct in_addr) { host });
    inet_pton(AF_INET, ipv4_string, &(server.sin_addr));

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0) return 0;
    else return sock;
}

char net_recv(word_t netref) {
    char buf;
    return (char) recv(netref, &buf, 1, 0);
}

int net_send(word_t netref, char output) {
    return (int) send(netref, &output, sizeof(output), 0);
}

int net_close(word_t netref) {
    return close(netref);
}
