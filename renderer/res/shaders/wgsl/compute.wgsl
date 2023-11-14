@group(0) @binding(0) var<storage,read_write> outputBuffer: array<vec4<f32>,64>;

@compute @workgroup_size(32, 1, 1)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
//    outputBuffer[id.x] = vec4<f32>(f32(id.x), f32(id.x), 0.0, 1.0);
outputBuffer[id.x] = vec4<f32>(f32(id.x), f32(id.x), 0.0, 1.0);
}

