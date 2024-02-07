struct ParticlesData {
    deltaTime: f32,
    time: f32,
    seed: vec2<f32>,
};

struct Particle
{
    position: vec3f,
    prevPosition: vec3f,
    acc: vec3f,
};

const ellipse_count: u32 = 16;

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Particle,32768>;
@group(0) @binding(1) var<uniform> data: ParticlesData;
var<private> rng_state: u32 = 0;

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

@compute @workgroup_size(32, 1, 1)
fn init(@builtin(global_invocation_id) id: vec3<u32>) {
    let points_per_ellipse = 32768 / ellipse_count;
    let ellipse_id = id.x / u32(points_per_ellipse);
    var angle = 2.0 * 3.14159265358979323846 * f32(f32(id.x) % f32(points_per_ellipse)) / f32(points_per_ellipse);
    let x_radius = 0.04 + f32(ellipse_id) * 0.04;
    let y_radius = 0.08 + f32(ellipse_id) * 0.08;
    let x = x_radius * sin(angle);
    let y = y_radius * cos(angle);

    let rotation_angle = f32(ellipse_id) * (3.14159265358979323846 / 2.0 / f32(ellipse_count - 1));
    var rotated_x = x * cos(rotation_angle) - y * sin(rotation_angle);
    var rotated_y = x * sin(rotation_angle) + y * cos(rotation_angle);

    var particle: Particle;
    particle.position = vec3<f32>(rotated_x, rotated_y, 0.0);

    rng_state = wang_hash(id.x);
    let rx = rand_f32_01() - 0.5;
    let ry = rand_f32_01() - 0.5;

    particle.position += vec3<f32>(rx * 0.1, ry * 0.1, 0.0);

    let d = normalize(cross(particle.position, vec3<f32>(0.0, 0.0, -1.0)));
    particle.prevPosition = particle.position + d * 0.002;
    particle.acc = vec3<f32>(0.0, 0.0, 0.0);

    vertexBuffer[id.x] = particle;
}

@compute @workgroup_size(32, 1, 1)
fn init2(@builtin(global_invocation_id) id: vec3<u32>) {
    var particle: Particle;
    rng_state = wang_hash(id.x);
    let x = rand_f32_01() - 0.5;
    let y = rand_f32_01() - 0.5;
    particle.position = vec3<f32>(x, y, 0.0);

//    let d = normalize(cross(particle.position, vec3<f32>(0.0, 0.0, -1.0)));
//    particle.prevPosition = particle.position + d * 0.002;
    particle.prevPosition = particle.position;
    particle.acc = vec3<f32>(0.0, 0.0, 0.0);

    vertexBuffer[id.x] = particle;
}

@compute @workgroup_size(32, 1, 1)
fn simulate(@builtin(global_invocation_id) id: vec3<u32>) {
    var particle: Particle = vertexBuffer[id.x];
//    let dir = normalize(particle.position);
//    particle.position += dir * data.deltaTime * 0.1;
//    particle.prevPosition = particle.position;
    let x: f32 = 2 * particle.position.x - particle.prevPosition.x + particle.acc.x * data.deltaTime * data.deltaTime;
    let y: f32 = 2 * particle.position.y - particle.prevPosition.y + particle.acc.y * data.deltaTime * data.deltaTime;

    particle.prevPosition = particle.position;

    particle.position.x = x;
    particle.position.y = y;

//	float3 dir = gp->xyz - p->pos.xyz;
//	float dist = length(dir);
//	float f = G * (gp->w / (dist * dist + 0.006544f));
//	float3 vel = normalize(dir) * f;
    let dir = -particle.prevPosition;
    let dist = length(dir);
    let f = 1.0 / (dist + 0.006544);

    particle.acc = normalize(dir) * f * 0.017;
    vertexBuffer[id.x] = particle;
}
