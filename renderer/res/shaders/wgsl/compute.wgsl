@group(0) @binding(0) var<storage,read_write> outputBuffer: array<f32,512>;

@compute @workgroup_size(32)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    if (id.x == 0) {
        outputBuffer[id.x] = 1.0;
    }
    else if (id.x == 511)
    {
        outputBuffer[id.x] = 99.0;
    }
    else
    {
        outputBuffer[id.x] = 0.0;
    }
}

