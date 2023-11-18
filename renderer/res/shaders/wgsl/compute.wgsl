struct FigmentData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

@group(0) @binding(0) var<storage,read_write> outputBuffer: array<vec4<f32>,64>;
@group(0) @binding(1) var<uniform> figmentData: FigmentData;

@compute @workgroup_size(32, 1, 1)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    let angle = radians((f32(id.x) / f32(64)) * 360.0);
    let x = f32(0) + 10.8 * cos(angle);
    let y = f32(0) + 10.8 * sin(angle);
    let z = f32(0);
    outputBuffer[id.x] = vec4<f32>(x, y, z, 1.0);
}

