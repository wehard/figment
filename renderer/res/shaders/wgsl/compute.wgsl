@group(0) @binding(0) var<storage,read_write> outputBuffer: array<f32,96>;

@compute @workgroup_size(32, 1, 1)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    outputBuffer[id.x] = f32(id.x);
}

