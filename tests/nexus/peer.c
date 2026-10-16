/* Nexus wire peer. UPSTREAM_NEXUS builds this against the unmodified pinned
 * cthutu/dev implementation; otherwise it is a standalone golden-wire peer. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char golden[] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde,
                                       0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32,
                                       0x10, 0,    0,    0,    5,    'n',  'e',
                                       'x',  'u',  's',  0,    0,    0,    0};

#ifdef UPSTREAM_NEXUS
#    include <nexus/nexus.h>
int run(int argc, char** argv)
{
    assert(argc == 2);
    char url[64];
    snprintf(url, sizeof(url), "tcp://127.0.0.1:%u", (unsigned)atoi(argv[1]));
    Net_Socket socket = net_socket();
    assert(net_set_option(&socket, NET_OPT_CONNECT_TIMEOUT_MS, 2000) == NET_OK);
    assert(net_set_option(&socket, NET_OPT_SEND_TIMEOUT_MS, 2000) == NET_OK);
    assert(net_set_option(&socket, NET_OPT_RECV_TIMEOUT_MS, 2000) == NET_OK);
    assert(net_connect(&socket, url) == NET_OK);
    Net_Message message = net_message_create(&socket);
    net_message_append_u8(&message, 0x12);
    net_message_append_u16(&message, 0x3456);
    net_message_append_u32(&message, 0x789abcde);
    net_message_append_u64(&message, 0xfedcba9876543210ull);
    net_message_append_string(&message, S("nexus"));
    net_message_append_string(&message, S(""));
    assert(message.length == sizeof(golden));
    assert(memcmp(message.data, golden, sizeof(golden)) == 0);
    assert(net_send(&message) == NET_OK);
    assert(net_recv(&message) == NET_OK);
    assert(message.length == sizeof(golden));
    assert(memcmp(message.data, golden, sizeof(golden)) == 0);
    u8     a;
    u16    b;
    u32    c;
    u64    d;
    string text;
    assert(net_message_read_u8(&message, &a) && a == 0x12);
    assert(net_message_read_u16(&message, &b) && b == 0x3456);
    assert(net_message_read_u32(&message, &c) && c == 0x789abcde);
    assert(net_message_read_u64(&message, &d) && d == 0xfedcba9876543210ull);
    assert(net_message_read_string(&message, &text) && text.count == 5);
    assert(memcmp(text.data, "nexus", 5) == 0);
    assert(net_message_read_string(&message, &text) && text.count == 0);
    net_message_clear(&message);
    assert(net_send(&message) == NET_OK);
    assert(net_recv(&message) == NET_OK && message.length == 0);
    net_message_done(&message);
    net_close(&socket);
    puts("C peer passed");
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
#        include <unistd.h>
typedef int PeerSocket;
#        define peer_close close
#    endif

static void
transfer(PeerSocket socket, unsigned char* bytes, size_t length, int writing)
{
    size_t offset = 0;
    while (offset < length) {
        int count =
            writing
                ? (int)send(socket,
                            (const char*)bytes + offset,
                            (int)(length - offset),
                            0)
                : (int)recv(
                      socket, (char*)bytes + offset, (int)(length - offset), 0);
        assert(count > 0);
        offset += (size_t)count;
    }
}

int main(int argc, char** argv)
{
    assert(argc == 2);
#    ifdef _WIN32
    WSADATA data;
    assert(WSAStartup(MAKEWORD(2, 2), &data) == 0);
#    endif
    PeerSocket socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#    ifdef _WIN32
    assert(socket_fd != INVALID_SOCKET);
#    else
    assert(socket_fd >= 0);
#    endif
    struct sockaddr_in address = {0};
    address.sin_family         = AF_INET;
    address.sin_port           = htons((unsigned short)atoi(argv[1]));
    address.sin_addr.s_addr    = htonl(0x7f000001);
    assert(connect(socket_fd, (struct sockaddr*)&address, sizeof(address)) ==
           0);
    unsigned char frame[4 + sizeof(golden)] = {0, 0, 0, sizeof(golden)};
    memcpy(frame + 4, golden, sizeof(golden));
    /* Fragment the prefix, then transmit the payload separately. */
    transfer(socket_fd, frame, 2, 1);
    transfer(socket_fd, frame + 2, sizeof(frame) - 2, 1);
    unsigned char reply[sizeof(frame)];
    transfer(socket_fd, reply, sizeof(reply), 0);
    assert(memcmp(reply, frame, sizeof(frame)) == 0);
    unsigned char empty[4] = {0};
    transfer(socket_fd, empty, 4, 1);
    transfer(socket_fd, reply, 4, 0);
    assert(memcmp(reply, empty, 4) == 0);
    assert(peer_close(socket_fd) == 0);
#    ifdef _WIN32
    assert(WSACleanup() == 0);
#    endif
    puts("C peer passed");
    return 0;
}
#endif
