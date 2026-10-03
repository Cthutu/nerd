// Native ABI assertions for the declarations in os.socket.
#include <stddef.h>
#ifdef _WIN32
#    include <winsock2.h>
_Static_assert(sizeof(SOCKET) == 8, "Windows SOCKET must be pointer-sized");
_Static_assert(sizeof(WSADATA) == 408, "Expected x64 Winsock startup layout");
_Static_assert(offsetof(WSADATA, lpVendorInfo) == 8, "WSADATA vendor offset");
_Static_assert(offsetof(WSADATA, szDescription) == 16,
               "WSADATA description offset");
_Static_assert(offsetof(WSADATA, szSystemStatus) == 273,
               "WSADATA status offset");
_Static_assert(FIONBIO == 0x8004667e, "Winsock nonblocking request");
#else
#    include <netinet/in.h>
#    include <sys/ioctl.h>
#    include <sys/socket.h>
_Static_assert(sizeof(socklen_t) == 4, "Expected Linux socklen_t width");
_Static_assert(sizeof(ssize_t) == 8, "Expected Linux x64 ssize_t width");
_Static_assert(MSG_NOSIGNAL == 0x4000, "Linux send flag");
_Static_assert(MSG_TRUNC == 0x20, "Linux receive flag");
_Static_assert(FIONBIO == 0x5421, "Linux nonblocking request");
#endif
_Static_assert(sizeof(struct sockaddr_in) == 16, "Expected IPv4 layout");
_Static_assert(offsetof(struct sockaddr_in, sin_family) == 0, "Family offset");
_Static_assert(offsetof(struct sockaddr_in, sin_port) == 2, "Port offset");
_Static_assert(offsetof(struct sockaddr_in, sin_addr) == 4, "Address offset");
_Static_assert(AF_INET == 2 && SOCK_STREAM == 1 && SOCK_DGRAM == 2,
               "Socket constants");
int main(void) { return 0; }
