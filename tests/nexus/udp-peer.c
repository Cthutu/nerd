/* Exchange a typed message and an empty datagram with the public Nerd API.
 * UPSTREAM_NEXUS uses the original library instead of native datagram calls. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char payload[] = {
    0, 0, 0, 42, 0, 0, 0, 5, 'n', 'e', 'x', 'u', 's'};

#ifdef UPSTREAM_NEXUS
#    include <nexus/nexus.h>
int run(int argc, char** argv)
{
    assert(argc == 2);
    char url[64];
    snprintf(url, sizeof(url), "udp://127.0.0.1:%u", (unsigned)atoi(argv[1]));
    Net_Socket sock = net_socket();
    assert(net_set_option(&sock, NET_OPT_RECV_TIMEOUT_MS, 2000) == NET_OK);
    assert(net_connect(&sock, url) == NET_OK);
    Net_Message msg = net_message_create(&sock);
    net_message_append_u32(&msg, 42);
    net_message_append_string(&msg, S("nexus"));
    assert(msg.length == sizeof(payload));
    assert(memcmp(msg.data, payload, sizeof(payload)) == 0);
    assert(net_send(&msg) == NET_OK);
    assert(net_recv(&msg) == NET_OK);
    assert(msg.length == sizeof(payload));
    assert(memcmp(msg.data, payload, sizeof(payload)) == 0);
    net_message_clear(&msg);
    assert(net_send(&msg) == NET_OK);
    assert(net_recv(&msg) == NET_OK && msg.length == 0);
    net_message_done(&msg);
    net_close(&sock);
    puts("C UDP peer passed");
    return 0;
}
#else
#    ifdef _WIN32
#        include <winsock2.h>
#        include <ws2tcpip.h>
typedef SOCKET PeerSocket;
#        define peer_close closesocket
#    else
#        include <arpa/inet.h>
#        include <sys/socket.h>
#        include <sys/time.h>
#        include <unistd.h>
typedef int PeerSocket;
#        define peer_close close
#    endif
int main(int argc, char** argv)
{
    assert(argc == 2);
#    ifdef _WIN32
    WSADATA data;
    assert(WSAStartup(MAKEWORD(2, 2), &data) == 0);
#    endif
    PeerSocket sock = socket(AF_INET, SOCK_DGRAM, 0);
#    ifdef _WIN32
    assert(sock != INVALID_SOCKET);
    DWORD timeout = 2000;
#    else
    assert(sock >= 0);
    struct timeval timeout = {2, 0};
#    endif
    assert(setsockopt(sock,
                      SOL_SOCKET,
                      SO_RCVTIMEO,
                      (const char*)&timeout,
                      sizeof(timeout)) == 0);
    struct sockaddr_in address = {0};
    address.sin_family         = AF_INET;
    address.sin_port           = htons((unsigned short)atoi(argv[1]));
    address.sin_addr.s_addr    = htonl(INADDR_LOOPBACK);
    assert(connect(sock, (struct sockaddr*)&address, sizeof(address)) == 0);
    assert(send(sock, (const char*)payload, sizeof(payload), 0) ==
           sizeof(payload));
    unsigned char output[64];
    assert(recv(sock, (char*)output, sizeof(output), 0) == sizeof(payload));
    assert(memcmp(output, payload, sizeof(payload)) == 0);
    assert(send(sock, "", 0, 0) == 0);
    assert(recv(sock, (char*)output, sizeof(output), 0) == 0);
    peer_close(sock);
#    ifdef _WIN32
    WSACleanup();
#    endif
    puts("C UDP peer passed");
    return 0;
}
#endif
