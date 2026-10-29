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
    point4 := Vec4[f32].point(1.0, 2.0, 3.0)
    direction4 := Vec4[f32].vector(1.0, 2.0, 3.0)
    assert point4.xyz() == direction4.xyz()
    assert point4.w == 1 && direction4.w == 0
    assert point4.xyz().to_point() == point4
    assert direction4.xyz().to_vector() == direction4
    moved4 := translation.mul_vector(point4)
    unchanged4 := translation.mul_vector(direction4)
    assert moved4.xyz() == Vec3[f32] { x: 11 y: 22 z: 33 }
    assert moved4.w == 1
    assert unchanged4 == direction4

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
    point4 := Vec4[f64].point(1.0, 2.0, 3.0)
    direction4 := Vec4[f64].vector(1.0, 2.0, 3.0)
    assert point4.xyz() == direction4.xyz()
    assert point4.w == 1 && direction4.w == 0
    assert point4.xyz().to_point() == point4
    assert direction4.xyz().to_vector() == direction4
    moved4 := translation.mul_vector(point4)
    unchanged4 := translation.mul_vector(direction4)
    assert moved4.xyz() == Vec3[f64] { x: 11 y: 22 z: 33 }
    assert moved4.w == 1
    assert unchanged4 == direction4

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

check_edges_f32 :: fn () {
    tolerance: f64 = 0.00001
    pair := Vec2[f32] { x: -3 y: 4 }
    unit2_optional := pair.normalised()
    assert !(!unit2_optional)
    unit2 := unit2_optional \ Vec2[f32].zero()
    assert near(unit2.x.as(f64), -0.6, tolerance)
    assert near(unit2.y.as(f64), 0.8, tolerance)
    assert near(unit2.length().as(f64), 1.0, tolerance)
    assert !Vec2[f32].zero().normalised()
    assert !Vec4[f32].zero().normalised()
    unit4_optional := Vec4[f32] { x: 0 y: 0 z: -3 w: 4 }.normalised()
    assert !(!unit4_optional)
    unit4 := unit4_optional \ Vec4[f32].zero()
    assert near(unit4.z.as(f64), -0.6, tolerance)
    assert near(unit4.w.as(f64), 0.8, tolerance)
    assert near(unit4.length().as(f64), 1.0, tolerance)
    nan := sqrt(-1.0).as(f32)
    huge: f32 = 1e30
    infinity := huge * huge
    assert !is_finite(infinity.as(f64))
    bad2 := Vec2[f32] { x: 1 y: nan }
    bad3 := Vec3[f32] { x: 1 y: -infinity z: 2 }
    bad4 := Vec4[f32] { x: 1 y: 2 z: 3 w: infinity }
    assert !bad2.is_finite() && !bad2.normalised()
    assert !bad3.is_finite() && !bad3.normalised()
    assert !bad4.is_finite() && !bad4.normalised()
    prn("f32 vector normalisation and non-finite components")

    projective := Mat4[f32].identity()
    projective.c0.w = 1
    value := Vec3[f32] { x: 1 y: 2 z: 3 }
    raw := projective.mul_vector(value.to_point())
    assert raw.xyz() == value && raw.w == 2
    projected_optional := projective.transform_point(value)
    assert !(!projected_optional)
    projected := projected_optional \ Vec3[f32].zero()
    assert projected == Vec3[f32] { x: 0.5 y: 1 z: 1.5 }
    negative_w := projective.transform_point(Vec3[f32] { x: -2 y: 2 z: 4 }) \ Vec3[f32].zero()
    assert negative_w == Vec3[f32] { x: 2 y: -2 z: -4 }
    assert !projective.transform_point(Vec3[f32] { x: -1 y: 2 z: 3 })
    inverse_optional := projective.inverse()
    assert !(!inverse_optional)
    inverse := inverse_optional \ Mat4[f32].zero()
    expected := Mat4[f32].identity()
    expected.c0.w = -1
    assert inverse == expected
    assert projective.mul(inverse) == Mat4[f32].identity()
    assert inverse.mul(projective) == Mat4[f32].identity()
    assert (inverse.transform_point(projected) \ Vec3[f32].zero()) == value
    singular3 := Mat3[f32] { c0: { x: 1 y: 2 z: 3 } c1: { x: 2 y: 4 z: 6 } c2: { x: 0 y: 1 z: 0 } }
    assert !singular3.inverse()
    singular4 := Mat4[f32].identity()
    singular4.c1 = singular4.c0
    assert !singular4.inverse()
    invalid3 := Mat3[f32].identity()
    invalid3.c1.z = nan
    assert !invalid3.inverse()
    invalid4 := Mat4[f32].identity()
    invalid4.c2.w = nan
    assert !invalid4.inverse()
    invalid_point := Mat4[f32].identity()
    invalid_point.c3.x = nan
    assert !invalid_point.transform_point(value)
    prn("f32 projective division and invalid matrix inverses")

    identity := Quaternion[f32].identity()
    axis := Vec3[f32] { x: 0 y: 0 z: 7 }
    quarter_optional := Quaternion[f32].from_axis_angle(axis, half_pi)
    assert !(!quarter_optional)
    quarter := quarter_optional \ identity
    right := Vec3[f32].right()
    at_start := identity.slerp(quarter, 0) \ quarter
    at_end := identity.slerp(quarter, 1) \ identity
    assert near((at_start.rotate(right) \ Vec3[f32].zero()).x.as(f64), 1.0, tolerance)
    assert near((at_end.rotate(right) \ Vec3[f32].zero()).y.as(f64), 1.0, tolerance)
    beyond := identity.slerp(quarter, 2) \ identity
    beyond_value := beyond.rotate(right) \ Vec3[f32].zero()
    assert near(beyond_value.x.as(f64), -1.0, tolerance)
    assert near(beyond_value.y.as(f64), 0.0, tolerance)
    before := identity.slerp(quarter, -1) \ identity
    before_value := before.rotate(right) \ Vec3[f32].zero()
    assert near(before_value.x.as(f64), 0.0, tolerance)
    assert near(before_value.y.as(f64), -1.0, tolerance)
    -- Nearby rotations take the normalised-linear interpolation branch.
    nearby := Quaternion[f32].from_axis_angle(axis, 0.0002) \ identity
    close_half := identity.slerp(nearby, 0.5) \ identity
    close_value := close_half.rotate(right) \ Vec3[f32].zero()
    assert near(close_value.x.as(f64), cos(0.0001), tolerance)
    assert near(close_value.y.as(f64), sin(0.0001), tolerance)
    assert near(close_half.length_squared().as(f64), 1.0, tolerance)
    negative := Quaternion[f32].from_axis_angle(axis, -half_pi) \ identity
    backward := negative.rotate(right) \ Vec3[f32].zero()
    assert near(backward.y.as(f64), -1.0, tolerance)
    opposite := quarter.scale(-3).rotate(right) \ Vec3[f32].zero()
    assert near(opposite.y.as(f64), 1.0, tolerance)
    assert (Quaternion[f32].from_axis_angle(axis, 0) \ identity) == identity
    assert !identity.slerp(quarter, nan)
    assert !identity.slerp(quarter, infinity)
    assert !Quaternion[f32].from_axis_angle(axis, infinity)
    assert !Quaternion[f32].from_axis_angle(Vec3[f32] { x: nan y: 0 z: 1 }, 0)
    invalid := Quaternion[f32] { v: { x: 0 y: 0 z: 0 } w: nan }
    assert !invalid.normalised() && !invalid.rotate(right) && !invalid.to_mat3()
    assert !invalid.slerp(identity, 0.5) && !identity.slerp(invalid, 0.5)
    prn("f32 quaternion endpoints, extrapolation and invalid inputs")
}

check_edges_f64 :: fn () {
    tolerance: f64 = 0.0000000001
    pair := Vec2[f64] { x: -3 y: 4 }
    unit2_optional := pair.normalised()
    assert !(!unit2_optional)
    unit2 := unit2_optional \ Vec2[f64].zero()
    assert near(unit2.x.as(f64), -0.6, tolerance)
    assert near(unit2.y.as(f64), 0.8, tolerance)
    assert near(unit2.length().as(f64), 1.0, tolerance)
    assert !Vec2[f64].zero().normalised()
    assert !Vec4[f64].zero().normalised()
    unit4_optional := Vec4[f64] { x: 0 y: 0 z: -3 w: 4 }.normalised()
    assert !(!unit4_optional)
    unit4 := unit4_optional \ Vec4[f64].zero()
    assert near(unit4.z.as(f64), -0.6, tolerance)
    assert near(unit4.w.as(f64), 0.8, tolerance)
    assert near(unit4.length().as(f64), 1.0, tolerance)
    nan := sqrt(-1.0).as(f64)
    huge: f64 = 1e300
    infinity := huge * huge
    assert !is_finite(infinity.as(f64))
    bad2 := Vec2[f64] { x: 1 y: nan }
    bad3 := Vec3[f64] { x: 1 y: -infinity z: 2 }
    bad4 := Vec4[f64] { x: 1 y: 2 z: 3 w: infinity }
    assert !bad2.is_finite() && !bad2.normalised()
    assert !bad3.is_finite() && !bad3.normalised()
    assert !bad4.is_finite() && !bad4.normalised()
    prn("f64 vector normalisation and non-finite components")

    projective := Mat4[f64].identity()
    projective.c0.w = 1
    value := Vec3[f64] { x: 1 y: 2 z: 3 }
    raw := projective.mul_vector(value.to_point())
    assert raw.xyz() == value && raw.w == 2
    projected_optional := projective.transform_point(value)
    assert !(!projected_optional)
    projected := projected_optional \ Vec3[f64].zero()
    assert projected == Vec3[f64] { x: 0.5 y: 1 z: 1.5 }
    negative_w := projective.transform_point(Vec3[f64] { x: -2 y: 2 z: 4 }) \ Vec3[f64].zero()
    assert negative_w == Vec3[f64] { x: 2 y: -2 z: -4 }
    assert !projective.transform_point(Vec3[f64] { x: -1 y: 2 z: 3 })
    inverse_optional := projective.inverse()
    assert !(!inverse_optional)
    inverse := inverse_optional \ Mat4[f64].zero()
    expected := Mat4[f64].identity()
    expected.c0.w = -1
    assert inverse == expected
    assert projective.mul(inverse) == Mat4[f64].identity()
    assert inverse.mul(projective) == Mat4[f64].identity()
    assert (inverse.transform_point(projected) \ Vec3[f64].zero()) == value
    singular3 := Mat3[f64] { c0: { x: 1 y: 2 z: 3 } c1: { x: 2 y: 4 z: 6 } c2: { x: 0 y: 1 z: 0 } }
    assert !singular3.inverse()
    singular4 := Mat4[f64].identity()
    singular4.c1 = singular4.c0
    assert !singular4.inverse()
    invalid3 := Mat3[f64].identity()
    invalid3.c1.z = nan
    assert !invalid3.inverse()
    invalid4 := Mat4[f64].identity()
    invalid4.c2.w = nan
    assert !invalid4.inverse()
    invalid_point := Mat4[f64].identity()
    invalid_point.c3.x = nan
    assert !invalid_point.transform_point(value)
    prn("f64 projective division and invalid matrix inverses")

    identity := Quaternion[f64].identity()
    axis := Vec3[f64] { x: 0 y: 0 z: 7 }
    quarter_optional := Quaternion[f64].from_axis_angle(axis, half_pi)
    assert !(!quarter_optional)
    quarter := quarter_optional \ identity
    right := Vec3[f64].right()
    at_start := identity.slerp(quarter, 0) \ quarter
    at_end := identity.slerp(quarter, 1) \ identity
    assert near((at_start.rotate(right) \ Vec3[f64].zero()).x.as(f64), 1.0, tolerance)
    assert near((at_end.rotate(right) \ Vec3[f64].zero()).y.as(f64), 1.0, tolerance)
    beyond := identity.slerp(quarter, 2) \ identity
    beyond_value := beyond.rotate(right) \ Vec3[f64].zero()
    assert near(beyond_value.x.as(f64), -1.0, tolerance)
    assert near(beyond_value.y.as(f64), 0.0, tolerance)
    before := identity.slerp(quarter, -1) \ identity
    before_value := before.rotate(right) \ Vec3[f64].zero()
    assert near(before_value.x.as(f64), 0.0, tolerance)
    assert near(before_value.y.as(f64), -1.0, tolerance)
    -- Nearby rotations take the normalised-linear interpolation branch.
    nearby := Quaternion[f64].from_axis_angle(axis, 0.0002) \ identity
    close_half := identity.slerp(nearby, 0.5) \ identity
    close_value := close_half.rotate(right) \ Vec3[f64].zero()
    assert near(close_value.x.as(f64), cos(0.0001), tolerance)
    assert near(close_value.y.as(f64), sin(0.0001), tolerance)
    assert near(close_half.length_squared().as(f64), 1.0, tolerance)
    negative := Quaternion[f64].from_axis_angle(axis, -half_pi) \ identity
    backward := negative.rotate(right) \ Vec3[f64].zero()
    assert near(backward.y.as(f64), -1.0, tolerance)
    opposite := quarter.scale(-3).rotate(right) \ Vec3[f64].zero()
    assert near(opposite.y.as(f64), 1.0, tolerance)
    assert (Quaternion[f64].from_axis_angle(axis, 0) \ identity) == identity
    assert !identity.slerp(quarter, nan)
    assert !identity.slerp(quarter, infinity)
    assert !Quaternion[f64].from_axis_angle(axis, infinity)
    assert !Quaternion[f64].from_axis_angle(Vec3[f64] { x: nan y: 0 z: 1 }, 0)
    invalid := Quaternion[f64] { v: { x: 0 y: 0 z: 0 } w: nan }
    assert !invalid.normalised() && !invalid.rotate(right) && !invalid.to_mat3()
    assert !invalid.slerp(identity, 0.5) && !identity.slerp(invalid, 0.5)
    prn("f64 quaternion endpoints, extrapolation and invalid inputs")
}

main :: fn () {
    assert near(degrees(pi), 180.0, 1e-10)
    assert near(radians(180.0), pi, 1e-10)
    assert near(atan2(1.0, 0.0), half_pi, 1e-10)
    assert near(asin(1.0), half_pi, 1e-10)
    assert near(acos(-1.0), pi, 1e-10)
    check_f32()
    check_f64()
    check_edges_f32()
    check_edges_f64()
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
f32 vector normalisation and non-finite components
f32 projective division and invalid matrix inverses
f32 quaternion endpoints, extrapolation and invalid inputs
f64 vector normalisation and non-finite components
f64 projective division and invalid matrix inverses
f64 quaternion endpoints, extrapolation and invalid inputs
¬
¬
