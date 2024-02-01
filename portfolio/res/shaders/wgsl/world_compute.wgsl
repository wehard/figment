struct ParticlesData {
    deltaTime: f32,
    time: f32,
    seed: vec2<f32>,
};

struct Particle
{
    position: vec3f,
    color: vec4f,
};


@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Particle, 262144>;
@group(0) @binding(1) var<uniform> data: ParticlesData;
@group(0) @binding(2) var worldTexture: texture_2d<f32>;
@group(0) @binding(3) var sampler1: sampler;

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

const width: u32 = 512;
const height: u32 = 512;
const PI = radians(180.0);

@compute @workgroup_size(32, 1, 1)
fn init(@builtin(global_invocation_id) id: vec3<u32>) {

    let rowIdx = id.x / width;
    let colIdx = id.x % height;

    let theta = 2.0 * PI * f32(colIdx) / f32(width);
    let phi = PI * f32(rowIdx) / f32(height);
    let x = sin(phi) * cos(theta);
    let y = cos(phi);
    let z = sin(phi) * sin(theta);

    var particle: Particle;
    particle.position = vec3<f32>(x, y, z);

    let uv = vec2<f32>(f32(colIdx) / f32(width), f32(rowIdx) / f32(height));
    let worldColor = textureSampleLevel(worldTexture, sampler1, uv, 0.0);

//    let c = (dot(particle.position, vec3<f32>(0.0, 0.0, -1.0)) + 1.0) / 2.0;
    if (particle.position.z > 0.0) {
        particle.color = vec4<f32>(worldColor.r, worldColor.g, worldColor.b, 1.0) * 2.0;
    } else {
        particle.color = vec4<f32>(0.0, 0.0, 0.0, 0.0);
    }

    vertexBuffer[id.x] = particle;
}

@compute @workgroup_size(32, 1, 1)
fn simulate(@builtin(global_invocation_id) id: vec3<u32>) {
    var particle: Particle = vertexBuffer[id.x];
    vertexBuffer[id.x] = particle;
}
