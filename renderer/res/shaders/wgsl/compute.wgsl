struct FigmentData {
    time: f32
};

struct Vertex
{
    position: vec3f,
    normal: vec3f,
    texCoord: vec2f,
    color: vec4f
};

@group(0) @binding(0) var<storage,read_write> vertexBuffer: array<Vertex,4>;
@group(0) @binding(1) var<uniform> figmentData: FigmentData;

@compute @workgroup_size(32, 1, 1)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    let x = f32(id.x % 2);
    var y = 0.0;
    if (id.x >= 2) {
        y = 1.0;
    }
    let z = 0.0;
    let vertex = Vertex(vec3<f32>(x + sin(figmentData.time), y, z), vec3<f32>(0.0, 0.0, 1.0), vec2<f32>(0.0, 0.0), vec4<f32>(1.0));
    vertexBuffer[id.x] = vertex;
}

