-(2-5)
¬
3
¬
¬
$0 = 2
$1 = 5
$2 = $0 - $1
$3 = -$2
return $3
¬
#include <stdio.h>

void pr(const char* str) { printf("%s", str); }

void prn(const char* str) { printf("%s\n", str); }

int $main() {
    int $0 = 2;
    int $1 = 5;
    int $2 = $0 - $1;
    int $3 = -$2;
    return $3;
}

int main() { return $main(); }
