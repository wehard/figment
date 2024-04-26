struct ParticlesData {
    deltaTime: f32,
    time: f32,
    seed: vec2<f32>,
};

struct Particle
{
    position: vec3f,
};

const ellipse_count: u32 = 32;

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Particle,32768>;
@group(0) @binding(1) var<uniform> data: ParticlesData;
var<private> rng_state: u32 = 0;
const PI: f32 = radians(180.0);

fn wang_hash(seed: u32) -> u32 {
    var x = seed;
    x = (x ^ 61) ^ (x >> 16);
    x *= 9;
    x = x ^ (x >> 4);
    x *= 0x27d4eb2d;
    x = x ^ (x >> 15);
    return x;
}

fn rand_xorshift() -> u32 {
    rng_state = rng_state ^ (rng_state << 13);
    rng_state = rng_state ^ (rng_state >> 17);
    rng_state = rng_state ^ (rng_state << 5);
    return rng_state;
}

fn rand_f32_01() -> f32 {
    return f32(rand_xorshift()) * (1.0 / 4294967296.0);
}

fn bezier_blend(t: f32) -> f32 {
    return t * t * (3.0 - 2.0 * t);
}

fn rotate(v: vec2<f32>, angle: f32) -> vec2<f32> {
    let x = v.x * cos(angle) - v.y * sin(angle);
    let y = v.x * sin(angle) + v.y * cos(angle);
    return vec2<f32>(x, y);
}

fn point_ellipse(x_radius: f32, y_radius: f32, angle: f32) -> vec2<f32> {
    let x = x_radius * sin(angle);
    let y = y_radius * cos(angle);
    return vec2<f32>(x, y);
}

@compute @workgroup_size(32, 1, 1)
fn init(@builtin(global_invocation_id) id: vec3<u32>) {
    let points_per_ellipse = 32768 / ellipse_count;
    let ellipse_id = id.x / u32(points_per_ellipse);

    var angle = 2.0 * PI * f32(f32(id.x) % f32(points_per_ellipse)) / f32(points_per_ellipse);
    let x_radius = 0.004 + f32(ellipse_id) * 0.04;
    let y_radius = 0.008 + f32(ellipse_id) * 0.08;

    let p = point_ellipse(x_radius, y_radius, angle);

    let rangle = f32(ellipse_id) * (PI / f32(ellipse_count - 1));
    var rotated = rotate(vec2<f32>(p.x, p.y), rangle);

    var particle: Particle;
    particle.position = vec3<f32>(rotated.xy, 0.0);

    rng_state = wang_hash(id.x);
    let rx = rand_f32_01() - 0.5;
    let ry = rand_f32_01() - 0.5;

    particle.position += vec3<f32>(rx * 0.2, ry * 0.2, 0.0);

    let t = length(particle.position.xyz) / 1.0;
    let rz = rand_f32_01() - 0.5;
    particle.position.z = rz * (1.0 - bezier_blend(t) * bezier_blend(t)) * 0.2;

    vertexBuffer[id.x] = particle;
}

@compute @workgroup_size(32, 1, 1)
fn simulate(@builtin(global_invocation_id) id: vec3<u32>) {
    let points_per_ellipse = 32768 / ellipse_count;
    let ellipse_id = id.x / u32(points_per_ellipse);

    var angle = 2.0 * PI * f32(f32(id.x) % f32(points_per_ellipse)) / f32(points_per_ellipse);
    let x_radius = 0.04 + f32(ellipse_id) * 0.04;
    let y_radius = 0.03 + f32(ellipse_id) * 0.03;

    angle += data.time * (1.0 - f32(ellipse_id) / f32(ellipse_count)) * 0.5;
    let p = point_ellipse(x_radius, y_radius, angle);

    var rangle = f32(ellipse_id) * (PI / f32(ellipse_count - 1));
    var rotated = rotate(vec2<f32>(p.x, p.y), rangle);

    var particle: Particle;
    particle.position = vec3<f32>(rotated.xy, 0.0);

    rng_state = wang_hash(id.x);
    let rx = rand_f32_01() - 0.5;
    let ry = rand_f32_01() - 0.5;

    particle.position += vec3<f32>(rx * 0.05, ry * 0.05, 0.0);

//    let t = length(particle.position.xyz) / 1.0;
//    let rz = rand_f32_01() - 0.5;
//    particle.position.z = rz * (1.0 - bezier_blend(t) * bezier_blend(t)) * 0.2;

    vertexBuffer[id.x] = particle;
}
