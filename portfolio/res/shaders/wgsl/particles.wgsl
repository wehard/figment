//struct FigmentData {
//    time: f32,
//    id: i32,
//    model: mat4x4<f32>,
//};

struct Particle
{
    position: vec3f,
};

// width and height constants
const width: u32 = 32;
const height: u32 = 32;
const ellipse_count: u32 = 16;

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Particle,16384>;
//@group(0) @binding(1) var<uniform> figmentData: FigmentData;

@compute @workgroup_size(32, 1, 1)
fn init(@builtin(global_invocation_id) id: vec3<u32>) {
    let points_per_ellipse = 16384 / ellipse_count;
    let ellipse_id = id.x / u32(points_per_ellipse);
    var angle = 2.0 * 3.14159265358979323846 * f32(f32(id.x) % f32(points_per_ellipse)) / f32(points_per_ellipse);
    let x_radius = 0.05 + f32(ellipse_id) * 0.05;
    let y_radius = 0.1 + f32(ellipse_id) * 0.1;
    let x = x_radius * cos(angle);
    let y = y_radius * sin(angle);

    let rotation_angle = f32(ellipse_id) * (3.14159265358979323846 / 2.0 / f32(ellipse_count - 1));
    var rotated_x = x * cos(rotation_angle) - y * sin(rotation_angle);
    var rotated_y = x * sin(rotation_angle) + y * cos(rotation_angle);

    vertexBuffer[id.x] = Particle(vec3<f32>(rotated_x, rotated_y, 0.0));
}

