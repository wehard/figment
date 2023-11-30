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

// width and height constants
const width: u32 = 32;
const height: u32 = 32;

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Vertex,1024>;
@group(0) @binding(1) var<uniform> figmentData: FigmentData;

@compute @workgroup_size(32, 1, 1)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    let rowIdx = id.x / width;
    let colIdx = id.x % height;
    let x = (f32(colIdx) / f32(width - 1) - 0.5) * 10.0;
    var y = (f32(rowIdx) / f32(height - 1) - 0.5) * 10.0;
    let z = sin(x * 0.9 + figmentData.time) * 0.5 + cos(y * 0.9 + figmentData.time) * 0.5;
    var color = vec4<f32>(x * 0.1 + 0.5, y * 0.1 + 0.5, z * 0.1 + 0.5, 1.0);

    let u = f32(colIdx) / f32(width - 1);
    let v = f32(rowIdx) / f32(height - 1);
    let uv = vec2<f32>(u, v);

    let vertex = Vertex(vec3<f32>(x, y, z), vec3<f32>(0.0, 0.0, 1.0), uv, color);
    vertexBuffer[id.x] = vertex;
}

