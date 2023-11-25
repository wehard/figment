struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

struct FigmentData {
    time: f32
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;
@binding(1) @group(0) var<uniform> figmentData: FigmentData;

@vertex
fn vs_main(@location(0) in_vertex_position: vec3f) -> @builtin(position) vec4f {
    return cameraData.proj * cameraData.view * vec4f(in_vertex_position, 1.0);
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

@fragment
fn fs_main() -> FragmentOutput {
        var output : FragmentOutput;
        output.color = vec4<f32>(1.0, figmentData.time, 0.0, 1.0);
        output.id = -1;
        return output;
}
