greeting: string = "Hello"
enabled: bool = 1.cast(bool)
ratio: f32 = 42.cast(f32)
weight: f64

main :: fn () => enabled.cast(i32)
¬
1
¬

¬
global greeting
global enabled
global ratio
global weight
init
greeting = "Hello"
$0 = cast bool:1
enabled = $0
$1 = cast f32:42
ratio = $1
weight = 0
end
fn main
$0 = cast i32:enabled
return $0
end
¬
string $greeting;
bool $enabled;
float $ratio;
double $weight;
void init() {
    $greeting = (string){.data = (u8*)"Hello", .count = 5};
    bool $0 = (bool)1;
    $enabled = $0;
    float $1 = (float)42;
    $ratio = $1;
    $weight = 0.0;
}
int $main() {
    int $0 = (int)$enabled;
    return $0;
}
