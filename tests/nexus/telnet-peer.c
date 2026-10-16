/* Built only against the unchanged pinned cthutu/dev Nexus implementation.
 * Exercise each implementation as both a Telnet client and a Telnet server. */
#include <assert.h>
#include <nexus/nexus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#    include <winsock2.h>
#else
#    include <arpa/inet.h>
#    include <sys/socket.h>
#endif

int run(int argc, char** argv)
{
    assert(argc == 2);
    Net_Socket sock = net_telnet_socket();
    assert(net_set_option(&sock, NET_OPT_CONNECT_TIMEOUT_MS, 2000) == NET_OK);
    assert(net_set_option(&sock, NET_OPT_SEND_TIMEOUT_MS, 2000) == NET_OK);
    assert(net_set_option(&sock, NET_OPT_RECV_TIMEOUT_MS, 2000) == NET_OK);
    Net_Message msg = net_message_create(&sock);
    if (strcmp(argv[1], "server") == 0) {
        assert(net_bind(&sock, "tcp://127.0.0.1:0") == NET_OK);
        struct sockaddr_in address = {0};
#ifdef _WIN32
        int length = sizeof(address);
        assert(getsockname(
                   (SOCKET)sock.fd, (struct sockaddr*)&address, &length) == 0);
#else
        socklen_t length = sizeof(address);
        assert(getsockname((int)sock.fd, (struct sockaddr*)&address, &length) ==
               0);
#endif
        printf("%u\n", (unsigned)ntohs(address.sin_port));
        fflush(stdout);
        assert(net_recv(&msg) == NET_OK);
        assert(msg.length == 2 && memcmp(msg.data, "xy", 2) == 0);
        net_message_clear(&msg);
        net_message_append(&msg, "final", 5);
        assert(net_send(&msg) == NET_OK);
        assert(net_recv(&msg) == NET_OK);
        assert(msg.length == 3 && memcmp(msg.data, "ack", 3) == 0);
    } else {
        char url[64];
        snprintf(
            url, sizeof(url), "tcp://127.0.0.1:%u", (unsigned)atoi(argv[1]));
        assert(net_connect(&sock, url) == NET_OK);
        net_message_append(&msg, "hello from telnet", 17);
        assert(net_send(&msg) == NET_OK);
        assert(net_recv(&msg) == NET_OK);
        assert(msg.length == 23 &&
               memcmp(msg.data, "echo: hello from telnet", 23) == 0);
        net_message_clear(&msg);
        net_message_append(&msg, "ack", 3);
        assert(net_send(&msg) == NET_OK);
        assert(net_recv(&msg) == NET_CLOSED);
    }
    net_message_done(&msg);
    net_close(&sock);
    puts("C Telnet peer passed");
    return 0;
}
