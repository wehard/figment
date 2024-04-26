struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

struct MeshInstanceData {
    @location(0) pos: vec3<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
};

@vertex
fn vs_main(@location(1) pos: vec3f, instance: MeshInstanceData) -> VertexOutput {
    var output: VertexOutput;
    let p = instance.pos + pos * 0.005;
    output.pos = cameraData.proj * cameraData.view * vec4<f32>(p, 1.0);
    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
};

@fragment
fn fs_main() -> FragmentOutput {
    var output: FragmentOutput;
    output.color = vec4<f32>(1.0, 1.0, 1.0, 0.5);
    return output;
}
