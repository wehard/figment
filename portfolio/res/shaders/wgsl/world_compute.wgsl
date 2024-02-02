struct WorldParticlesData {
    deltaTime: f32,
    time: f32,
    mousePos: vec2<f32>,
};

struct WorldParticle
{
    position: vec3f,
    color: vec4f,
};

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<WorldParticle, 262144>;
@group(0) @binding(1) var<uniform> data: WorldParticlesData;
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
const PI: f32 = radians(180.0);

fn init_position(u: u32, v: u32) -> vec3<f32> {
    let theta = 2.0 * PI * f32(u) / f32(width);
    let phi = PI * f32(v) / f32(height);
    let x = sin(phi) * cos(theta);
    let y = cos(phi);
    let z = sin(phi) * sin(theta);
    return vec3<f32>(x, y, z);
}

@compute @workgroup_size(32, 1, 1)
fn init(@builtin(global_invocation_id) id: vec3<u32>) {

    let rowIdx = id.x / width;
    let colIdx = id.x % height;

    var particle: WorldParticle;
    particle.position = init_position(colIdx, rowIdx);

    let uv = vec2<f32>(f32(colIdx) / f32(width), f32(rowIdx) / f32(height));
    particle.color = textureSampleLevel(worldTexture, sampler1, uv, 0.0);

    vertexBuffer[id.x] = particle;
}

@compute @workgroup_size(32, 1, 1)
fn simulate(@builtin(global_invocation_id) id: vec3<u32>) {
    var particle: WorldParticle = vertexBuffer[id.x];

    let dir = normalize(vec3<f32>(data.mousePos.xy, 0.0) - particle.position);
    let dist = length(dir);

    particle.position += dir * (dist * dist * dist) * data.deltaTime * 0.5;
    let init_pos = init_position(id.x % width, id.x / width);
    particle.position = mix(particle.position, init_pos, data.deltaTime * 2.1);

    vertexBuffer[id.x] = particle;
}
