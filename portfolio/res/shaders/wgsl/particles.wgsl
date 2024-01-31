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

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Particle,16384>;
@group(0) @binding(1) var<uniform> data: ParticlesData;

// function to generate a random number between 0 and 1
fn rand(seed: vec2<f32>) -> f32 {
    let x = sin(dot(seed, vec2<f32>(12.9898, 78.233))) * 43758.5453;
    return fract(x);
}

fn randInsideUnitCircle(seed: vec2<f32>) -> vec2<f32> {
    let r = rand(seed);
    let theta = rand(seed + vec2<f32>(1.0, 1.0)) * 2.0 * 3.14159265358979323846;
    return vec2<f32>(r * cos(theta), r * sin(theta));
}


@compute @workgroup_size(32, 1, 1)
fn init(@builtin(global_invocation_id) id: vec3<u32>) {
    let points_per_ellipse = 16384 / ellipse_count;
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

    // offset position randomly
    let offsetX = rand(data.seed * f32(id.x)) - 0.5;
    let offsetY = rand(data.seed / f32(id.x)) - 0.5;
    particle.position += vec3<f32>(offsetX * 0.1, offsetY * 0.1, 0.0);


    // set prev position to to be tangent to the ellipse
//    let prev_rotation_angle = rotation_angle + 0.01;
//    var prev_rotated_x = x * cos(prev_rotation_angle) - y * sin(prev_rotation_angle);
//    var prev_rotated_y = x * sin(prev_rotation_angle) + y * cos(prev_rotation_angle);
//    particle.prevPosition = vec3<f32>(prev_rotated_x, prev_rotated_y, 0.0);

    let d = normalize(cross(particle.position, vec3<f32>(0.0, 0.0, -1.0)));
    particle.prevPosition = particle.position + d * 0.002;
    particle.acc = vec3<f32>(0.0, 0.0, 0.0);

    vertexBuffer[id.x] = particle;
}

@compute @workgroup_size(32, 1, 1)
fn init2(@builtin(global_invocation_id) id: vec3<u32>) {
    var particle: Particle;
    let p = randInsideUnitCircle(data.seed * f32(id.x));
    particle.position = vec3<f32>(p.x, p.y, 0.0);

    let d = normalize(cross(particle.position, vec3<f32>(0.0, 0.0, -1.0)));
    particle.prevPosition = particle.position + d * 0.002;
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
