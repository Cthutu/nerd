use std.maths

near :: fn (left: f64, right: f64, tolerance: f64) -> bool { return abs(left - right) < tolerance }

check_f32 :: fn () {
    tolerance: f64 = 0.00001
    x := Vec3[f32].right()
    y := Vec3[f32].forward()
    z := Vec3[f32].up()
    assert x.cross(y) == z
    assert y.cross(z) == x
    assert z.cross(x) == y
    assert x.cross(x) == Vec3[f32].zero()
    assert x.dot(y) == 0
    assert x.add(y).sub(y) == x
    assert x.negated().scale(-1) == x
    assert x.lerp(y, 0).x == 1
    assert x.lerp(y, 1).y == 1
    assert Vec2[f32] { x: 1 y: 0 }.cross(Vec2[f32] { x: 0 y: 1 }) == 1
    assert Vec4[f32] { x: 1 y: 2 z: 3 w: 4 }.dot(Vec4[f32] { x: 1 y: 1 z: 1 w: 1 }) == 10
    diagonal := Vec3[f32] { x: 3 y: 4 z: 0 }
    assert near(diagonal.length().as(f64), 5.0, tolerance)
    unit := diagonal.normalised() \ Vec3[f32].zero()
    assert near(unit.length().as(f64), 1.0, tolerance)
    assert near(unit.x.as(f64), 0.6, tolerance)
    assert near(unit.y.as(f64), 0.8, tolerance)
    assert !Vec3[f32].zero().normalised()
    huge := Vec4[f32] { x: 1e30 y: 1e30 z: 1e30 w: 1e30 }
    huge_unit := huge.normalised() \ Vec4[f32].zero()
    assert near(huge_unit.x.as(f64), 0.5, tolerance)
    tiny := Vec3[f32] { x: 1e-30 y: 0 z: 0 }
    assert (tiny.normalised() \ Vec3[f32].zero()).x == 1
    invalid := Vec3[f32] { x: sqrt(-1.0).as(f32) y: 1 z: 2 }
    assert !invalid.normalised()
    prn("f32 vectors and right-handed basis")

    identity := Mat3[f32].identity()
    assert identity.mul_vector(diagonal) == diagonal
    linear := Mat3[f32] { c0: { x: 1 y: 2 z: 3 } c1: { x: 0 y: 1 z: 4 } c2: { x: 5 y: 6 z: 0 } }
    assert near(linear.determinant().as(f64), 1.0, tolerance)
    inverse3 := linear.inverse() \ Mat3[f32].zero()
    assert near(inverse3.c0.x.as(f64), -24.0, tolerance)
    assert near(inverse3.c1.y.as(f64), -15.0, tolerance)
    product3 := linear.mul(inverse3)
    assert near(product3.c0.x.as(f64), 1.0, tolerance)
    assert near(product3.c1.y.as(f64), 1.0, tolerance)
    assert near(product3.c2.z.as(f64), 1.0, tolerance)
    assert near(product3.c0.y.as(f64), 0.0, tolerance)
    assert linear.transpose().transpose() == linear
    assert !Mat3[f32].zero().inverse()
    scale := Mat4[f32].scaling(Vec3[f32] { x: 2 y: 3 z: 4 })
    translation := Mat4[f32].translation(Vec3[f32] { x: 10 y: 20 z: 30 })
    transform := translation.mul(scale)
    position := Vec3[f32] { x: 1 y: 2 z: 3 }
    moved := transform.transform_point(position) \ Vec3[f32].zero()
    assert moved.x == 12 && moved.y == 26 && moved.z == 42
    assert transform.transform_direction(position) == Vec3[f32] { x: 2 y: 6 z: 12 }
    inverse4 := transform.inverse() \ Mat4[f32].zero()
    recovered := inverse4.transform_point(moved) \ Vec3[f32].zero()
    assert near(recovered.x.as(f64), 1.0, tolerance)
    assert near(recovered.y.as(f64), 2.0, tolerance)
    assert near(recovered.z.as(f64), 3.0, tolerance)
    -- This matrix requires a pivot row swap, and is its own inverse.
    pivoted := Mat4[f32] { c0: { x: 0 y: 1 z: 0 w: 0 } c1: { x: 1 y: 0 z: 0 w: 0 } c2: { x: 0 y: 0 z: 1 w: 0 } c3: { x: 0 y: 0 z: 0 w: 1 } }
    assert (pivoted.inverse() \ Mat4[f32].zero()) == pivoted
    assert transform.transpose().transpose() == transform
    assert !Mat4[f32].zero().inverse()
    assert !Mat4[f32].zero().transform_point(position)
    prn("f32 matrix composition, inverse and homogeneous transforms")

    identity_q := Quaternion[f32].identity()
    assert (identity_q.rotate(position) \ Vec3[f32].zero()) == position
    quarter := Quaternion[f32].from_axis_angle(z, half_pi) \ identity_q
    turned := quarter.rotate(x) \ Vec3[f32].zero()
    assert near(turned.x.as(f64), 0.0, tolerance)
    assert near(turned.y.as(f64), 1.0, tolerance)
    assert near(turned.z.as(f64), 0.0, tolerance)
    reversed := quarter.conjugate().rotate(y) \ Vec3[f32].zero()
    assert near(reversed.x.as(f64), 1.0, tolerance)
    assert near(reversed.y.as(f64), 0.0, tolerance)
    scaled_q := quarter.scale(7)
    same_rotation := scaled_q.rotate(x) \ Vec3[f32].zero()
    assert near(same_rotation.y.as(f64), 1.0, tolerance)
    matrix_q := quarter.to_mat3() \ Mat3[f32].zero()
    matrix_turned := matrix_q.mul_vector(x)
    assert near(matrix_turned.y.as(f64), 1.0, tolerance)
    assert near(matrix_turned.x.as(f64), 0.0, tolerance)
    affine_q := Mat4[f32].from_mat3(matrix_q)
    assert near(affine_q.transform_direction(x).y.as(f64), 1.0, tolerance)
    turn_x := Quaternion[f32].from_axis_angle(x, half_pi) \ identity_q
    combined := turn_x.mul(quarter)
    composed := combined.rotate(x) \ Vec3[f32].zero()
    assert near(composed.z.as(f64), 1.0, tolerance)
    assert near(composed.x.as(f64), 0.0, tolerance)
    assert near(composed.y.as(f64), 0.0, tolerance)
    halfway := identity_q.slerp(quarter, 0.5) \ identity_q
    halfway_x := halfway.rotate(x) \ Vec3[f32].zero()
    assert near(halfway_x.x.as(f64), sqrt(0.5), tolerance)
    assert near(halfway_x.y.as(f64), sqrt(0.5), tolerance)
    shortest := quarter.slerp(quarter.negated(), 0.5) \ identity_q
    assert near((shortest.rotate(x) \ Vec3[f32].zero()).y.as(f64), 1.0, tolerance)
    assert !Quaternion[f32].from_axis_angle(Vec3[f32].zero(), 0)
    zero_q := Quaternion[f32] { v: { x: 0 y: 0 z: 0 } w: 0 }
    assert !zero_q.normalised()
    assert !zero_q.rotate(x)
    assert !zero_q.to_mat3()
    assert !zero_q.slerp(identity_q, 0.5)
    prn("f32 quaternion rotation, composition and shortest-arc interpolation")
}

check_f64 :: fn () {
    tolerance: f64 = 0.0000000001
    x := Vec3[f64].right()
    y := Vec3[f64].forward()
    z := Vec3[f64].up()
    assert x.cross(y) == z
    assert y.cross(z) == x
    assert z.cross(x) == y
    assert x.cross(x) == Vec3[f64].zero()
    assert x.dot(y) == 0
    assert x.add(y).sub(y) == x
    assert x.negated().scale(-1) == x
    assert x.lerp(y, 0).x == 1
    assert x.lerp(y, 1).y == 1
    assert Vec2[f64] { x: 1 y: 0 }.cross(Vec2[f64] { x: 0 y: 1 }) == 1
    assert Vec4[f64] { x: 1 y: 2 z: 3 w: 4 }.dot(Vec4[f64] { x: 1 y: 1 z: 1 w: 1 }) == 10
    diagonal := Vec3[f64] { x: 3 y: 4 z: 0 }
    assert near(diagonal.length().as(f64), 5.0, tolerance)
    unit := diagonal.normalised() \ Vec3[f64].zero()
    assert near(unit.length().as(f64), 1.0, tolerance)
    assert near(unit.x.as(f64), 0.6, tolerance)
    assert near(unit.y.as(f64), 0.8, tolerance)
    assert !Vec3[f64].zero().normalised()
    huge := Vec4[f64] { x: 1e300 y: 1e300 z: 1e300 w: 1e300 }
    huge_unit := huge.normalised() \ Vec4[f64].zero()
    assert near(huge_unit.x.as(f64), 0.5, tolerance)
    tiny := Vec3[f64] { x: 1e-300 y: 0 z: 0 }
    assert (tiny.normalised() \ Vec3[f64].zero()).x == 1
    invalid := Vec3[f64] { x: sqrt(-1.0).as(f64) y: 1 z: 2 }
    assert !invalid.normalised()
    prn("f64 vectors and right-handed basis")

    identity := Mat3[f64].identity()
    assert identity.mul_vector(diagonal) == diagonal
    linear := Mat3[f64] { c0: { x: 1 y: 2 z: 3 } c1: { x: 0 y: 1 z: 4 } c2: { x: 5 y: 6 z: 0 } }
    assert near(linear.determinant().as(f64), 1.0, tolerance)
    inverse3 := linear.inverse() \ Mat3[f64].zero()
    assert near(inverse3.c0.x.as(f64), -24.0, tolerance)
    assert near(inverse3.c1.y.as(f64), -15.0, tolerance)
    product3 := linear.mul(inverse3)
    assert near(product3.c0.x.as(f64), 1.0, tolerance)
    assert near(product3.c1.y.as(f64), 1.0, tolerance)
    assert near(product3.c2.z.as(f64), 1.0, tolerance)
    assert near(product3.c0.y.as(f64), 0.0, tolerance)
    assert linear.transpose().transpose() == linear
    assert !Mat3[f64].zero().inverse()
    scale := Mat4[f64].scaling(Vec3[f64] { x: 2 y: 3 z: 4 })
    translation := Mat4[f64].translation(Vec3[f64] { x: 10 y: 20 z: 30 })
    transform := translation.mul(scale)
    position := Vec3[f64] { x: 1 y: 2 z: 3 }
    moved := transform.transform_point(position) \ Vec3[f64].zero()
    assert moved.x == 12 && moved.y == 26 && moved.z == 42
    assert transform.transform_direction(position) == Vec3[f64] { x: 2 y: 6 z: 12 }
    inverse4 := transform.inverse() \ Mat4[f64].zero()
    recovered := inverse4.transform_point(moved) \ Vec3[f64].zero()
    assert near(recovered.x.as(f64), 1.0, tolerance)
    assert near(recovered.y.as(f64), 2.0, tolerance)
    assert near(recovered.z.as(f64), 3.0, tolerance)
    -- This matrix requires a pivot row swap, and is its own inverse.
    pivoted := Mat4[f64] { c0: { x: 0 y: 1 z: 0 w: 0 } c1: { x: 1 y: 0 z: 0 w: 0 } c2: { x: 0 y: 0 z: 1 w: 0 } c3: { x: 0 y: 0 z: 0 w: 1 } }
    assert (pivoted.inverse() \ Mat4[f64].zero()) == pivoted
    assert transform.transpose().transpose() == transform
    assert !Mat4[f64].zero().inverse()
    assert !Mat4[f64].zero().transform_point(position)
    prn("f64 matrix composition, inverse and homogeneous transforms")

    identity_q := Quaternion[f64].identity()
    assert (identity_q.rotate(position) \ Vec3[f64].zero()) == position
    quarter := Quaternion[f64].from_axis_angle(z, half_pi) \ identity_q
    turned := quarter.rotate(x) \ Vec3[f64].zero()
    assert near(turned.x.as(f64), 0.0, tolerance)
    assert near(turned.y.as(f64), 1.0, tolerance)
    assert near(turned.z.as(f64), 0.0, tolerance)
    reversed := quarter.conjugate().rotate(y) \ Vec3[f64].zero()
    assert near(reversed.x.as(f64), 1.0, tolerance)
    assert near(reversed.y.as(f64), 0.0, tolerance)
    scaled_q := quarter.scale(7)
    same_rotation := scaled_q.rotate(x) \ Vec3[f64].zero()
    assert near(same_rotation.y.as(f64), 1.0, tolerance)
    matrix_q := quarter.to_mat3() \ Mat3[f64].zero()
    matrix_turned := matrix_q.mul_vector(x)
    assert near(matrix_turned.y.as(f64), 1.0, tolerance)
    assert near(matrix_turned.x.as(f64), 0.0, tolerance)
    affine_q := Mat4[f64].from_mat3(matrix_q)
    assert near(affine_q.transform_direction(x).y.as(f64), 1.0, tolerance)
    turn_x := Quaternion[f64].from_axis_angle(x, half_pi) \ identity_q
    combined := turn_x.mul(quarter)
    composed := combined.rotate(x) \ Vec3[f64].zero()
    assert near(composed.z.as(f64), 1.0, tolerance)
    assert near(composed.x.as(f64), 0.0, tolerance)
    assert near(composed.y.as(f64), 0.0, tolerance)
    halfway := identity_q.slerp(quarter, 0.5) \ identity_q
    halfway_x := halfway.rotate(x) \ Vec3[f64].zero()
    assert near(halfway_x.x.as(f64), sqrt(0.5), tolerance)
    assert near(halfway_x.y.as(f64), sqrt(0.5), tolerance)
    shortest := quarter.slerp(quarter.negated(), 0.5) \ identity_q
    assert near((shortest.rotate(x) \ Vec3[f64].zero()).y.as(f64), 1.0, tolerance)
    assert !Quaternion[f64].from_axis_angle(Vec3[f64].zero(), 0)
    zero_q := Quaternion[f64] { v: { x: 0 y: 0 z: 0 } w: 0 }
    assert !zero_q.normalised()
    assert !zero_q.rotate(x)
    assert !zero_q.to_mat3()
    assert !zero_q.slerp(identity_q, 0.5)
    prn("f64 quaternion rotation, composition and shortest-arc interpolation")
}

main :: fn () {
    assert near(degrees(pi), 180.0, 1e-10)
    assert near(radians(180.0), pi, 1e-10)
    assert near(atan2(1.0, 0.0), half_pi, 1e-10)
    assert near(asin(1.0), half_pi, 1e-10)
    assert near(acos(-1.0), pi, 1e-10)
    check_f32()
    check_f64()
}
¬
0
¬
f32 vectors and right-handed basis
f32 matrix composition, inverse and homogeneous transforms
f32 quaternion rotation, composition and shortest-arc interpolation
f64 vectors and right-handed basis
f64 matrix composition, inverse and homogeneous transforms
f64 quaternion rotation, composition and shortest-arc interpolation
¬
¬
