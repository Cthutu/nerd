# Maths

Import `std.maths` as before. Its `mod.n` and sibling source files share one
scope; the split does not introduce public child imports.

| File | Contents |
| --- | --- |
| `mod.n` | Untyped `pi`, `tau`, `half_pi` and the coordinate convention |
| `trig.n` | Radian trigonometry and degree/radian conversion |
| `scalar.n` | Scalar helpers and square root |
| `rect.n` | Existing `Point[T]`, `Rect[T]`, `PointI32`, `RectI32` and methods |
| `vector.n` | `Vec2[T]`, `Vec3[T]`, `Vec4[T]` |
| `matrix.n` | `Mat3[T]`, `Mat4[T]` |
| `quaternion.n` | `Quaternion[T]` |
| `tests.n` | Source-level scalar and coordinate contracts |

`sin`, `cos`, `tan`, `asin`, `acos`, `atan2(y, x)`, `sqrt`, `abs`, `min`, `max`,
`clamp`, `radians` and `degrees` use `f64`. `lerp` infers its component type.
`clamp` expects ordered bounds; interpolation extrapolates outside `[0, 1]`.
The existing point, rectangle and trigonometric APIs remain available.

## Coordinate and storage conventions

Geometry is **right-handed: XY ground, +X right, +Y forward, +Z up**.
Right cross forward is up. Positive angles follow the right-hand rule: a
positive quarter-turn about +Z rotates +X into +Y. Angles are radians.

Matrices store columns (`c0`, `c1`, `c2`, and `c3` for `Mat4`) and act on column
vectors. `a.mul(b)` applies `b` first, then `a`. Translation occupies
`Mat4.c3.xyz`; the affine bottom row is `(0, 0, 0, 1)`. `Mat3` describes a 3D
linear transform. No graphics API clip-space/depth convention is imposed.

The types use `plex [T] #c` to preserve field/column order. Geometric operations
support `f32` and `f64`; integer types are not intended for normalisation,
inverses or rotations. Each type has concrete aliases, for example `Vec3F32`,
`Vec3F64`, `Mat4F32`, `Mat4F64`, `QuaternionF32` and `QuaternionF64`.

## Vectors

Vectors have the appropriate public `x`, `y`, `z`, `w` fields and provide
`zero`, `add`, `sub`, `negated`, `scale`, `divide`, `dot`, `length_squared`,
`length`, `normalised`, `is_finite` and `lerp`. Arithmetic returns a new value;
`divide` requires a nonzero divisor. `length_squared` and `length` can overflow
on large components. `normalised` scales first to avoid intermediate overflow
and returns `nil` for zero or non-finite input.

`Vec2.cross` returns a signed scalar; `Vec3.cross` returns a right-handed
vector. `Vec3.right()`, `forward()` and `up()` return the unit basis.
`Vec4.point(x, y, z)` constructs a homogeneous point with `w = 1`, while
`Vec4.vector(x, y, z)` constructs a direction with `w = 0`. Affine translation
therefore moves the point and leaves the direction unchanged. Existing `Vec3`
values convert with `to_point()` and `to_vector()`. For example:

```nerd
point := Vec4F32.point(1.0, 2.0, 3.0)
direction := Vec4F32.vector(1.0, 2.0, 3.0)
```

`Vec3.extend(w)` remains available for an arbitrary homogeneous component;
`Vec4.xyz()` drops `w` without perspective division. These constructors encode
the homogeneous component; they do not introduce separate point/vector types.

## Matrices

Both matrix types provide `zero`, `identity`, `mul_vector`, `mul`, `transpose`,
`scaling` and `inverse`. `Mat3` adds `determinant`; `Mat4` adds `translation`,
`from_mat3`, `transform_direction` and `transform_point`.

`transform_direction` uses homogeneous `w = 0`, ignoring translation.
`transform_point` uses `w = 1` and divides by the resulting `w`; zero/non-finite
`w` or a non-finite result returns `nil`. `mul_vector` performs raw homogeneous
multiplication when the caller needs to handle clipping itself.

`inverse` returns an optional. The 3x3 implementation uses cofactors and rejects
zero/non-finite determinants and non-finite results. The 4x4 implementation uses
partial pivoting, rejecting zero/non-finite pivots and non-finite results.
Neither imposes a fixed epsilon for near-singular matrices: applications must
also consider conditioning and precision. Default storage is zero; call
`identity()` explicitly for an identity transform.

## Quaternions

`Quaternion[T]` stores vector part `v: Vec3[T]` and scalar part `w: T`. This
makes its structure distinct from `Vec4`. It provides `identity`, `dot`,
`length_squared`, `conjugate`, `negated`, `scale`, `mul`, `normalised`,
`from_axis_angle`, `rotate`, `to_mat3` and `slerp`.

`from_axis_angle` normalises the axis and rejects zero/non-finite axes or
non-finite angles. `rotate` and `to_mat3` normalise first, so non-unit
quaternions describe the same rotation as their unit form. Zero and non-finite
quaternions return `nil`. A unit quaternion's `conjugate` is its inverse.
`q` and `q.negated()` represent the same rotation.

`a.mul(b)` applies `b` first. `slerp` takes the shortest arc, handles opposite
representations, uses normalised linear interpolation for nearby rotations,
and permits extrapolation. Invalid inputs, non-finite interpolation parameters
or unrepresentable results return `nil`. Default zero storage is not a
rotation; use `identity()` explicitly.

## Example

```nerd
use std.maths

main :: fn () -> ?void {
    right := Vec3F64.right()
    rotation := QuaternionF64.from_axis_angle(Vec3F64.up(), half_pi)?
    forward := rotation.rotate(right)?
    assert abs(forward.x) < 0.000001
    assert abs(forward.y - 1.0) < 0.000001

    linear := rotation.to_mat3()?
    transform := Mat4F64.translation(Vec3F64 { x: 10 y: 20 z: 30 })
        .mul(Mat4F64.from_mat3(linear))
    point := transform.transform_point(right)?
    assert abs(point.x - 10.0) < 0.000001
    assert abs(point.y - 21.0) < 0.000001
    assert abs(point.z - 30.0) < 0.000001
}
```

The grouped maths runtime fixture checks both precisions and LLVM/C backends,
including handedness, multiplication order, pivot swaps, homogeneous division,
quaternion/matrix agreement and invalid inputs.
