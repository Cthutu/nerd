-- Declares primitive variables with explicit types and casts.
greeting: string = "Hello"
enabled: bool = yes
ratio: f32 = 42.as(f32)
weight: f64

main :: fn () => enabled.as(i32)
¬
1
¬

¬
global greeting
global enabled
global ratio
global weight
fn main
$0 = cast bool:enabled
return i32:$0
end
init
greeting = string:"Hello"
enabled = bool:yes
$0 = cast i32:42
ratio = f32:$0
weight = f64:0
end
¬
string $greeting;
bool $enabled;
float $ratio;
double $weight;
int $main() {
    int $0 = (int)$enabled;
    return $0;
}
void init() {
    $greeting = (string){.data = (u8*)"Hello", .count = 5};
    $enabled = true;
    float $0 = (float)42;
    $ratio = $0;
    $weight = 0.0;
}
