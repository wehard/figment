struct FigmentData {
    time: f32,
    id: i32,
    model: mat4x4<f32>,
};

struct Vertex
{
    position: vec3f,
    normal: vec3f,
    texCoord: vec2f,
    color: vec4f
};

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Vertex,16384>;
@group(0) @binding(1) var<uniform> figmentData: FigmentData;

@compute @workgroup_size(32, 1, 1)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    let n: u32 = 128;
    let m: u32 = 128;
    let rowIdx = id.x / n;
    let colIdx = id.x % n;
    let x = (f32(colIdx) / f32(n - 1) - 0.5) * 10.0;
    var y = (f32(rowIdx) / f32(m - 1) - 0.5) * 10.0;
    let z = sin(x * 0.9 + figmentData.time) * 0.5 + cos(y * 0.9 + figmentData.time) * 0.5;
    let vertex = Vertex(vec3<f32>(x, y, z), vec3<f32>(0.0, 0.0, 1.0), vec2<f32>(0.0, 0.0), vec4<f32>(1.0, 1.0, 1.0, 1.0));
    vertexBuffer[id.x] = vertex;
}

