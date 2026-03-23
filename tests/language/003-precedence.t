1+2*3
¬
7
¬
¬
$0 = 1
$1 = 2
$2 = 3
$3 = $1 * $2
$4 = $0 + $3
return $4
¬
#include <stdio.h>

void pr(const char* str) { printf("%s", str); }

void prn(const char* str) { printf("%s\n", str); }

int $main() {
    int $0 = 1;
    int $1 = 2;
    int $2 = 3;
    int $3 = $1 * $2;
    int $4 = $0 + $3;
    return $4;
}

int main() { return $main(); }
