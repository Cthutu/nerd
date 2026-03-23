123+44
¬
167
¬
¬
$0 = 123
$1 = 44
$2 = $0 + $1
return $2
¬
#include <stdio.h>

void pr(const char* str) { printf("%s", str); }

void prn(const char* str) { printf("%s\n", str); }

int $main() {
    int $0 = 123;
    int $1 = 44;
    int $2 = $0 + $1;
    return $2;
}

int main() { return $main(); }
