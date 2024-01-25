struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

struct FigmentData {
    time: f32,
    id: i32,
    model: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;
@binding(1) @group(0) var<uniform> figmentData: FigmentData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) normal: vec3<f32>,
    @location(1) uv: vec2<f32>,
    @location(2) color: vec4<f32>
};

@vertex
fn vs_main(@location(0) in_position: vec3f, @location(1) in_normal: vec3f, @location(2) in_uv: vec2f, @location(3) in_color: vec4f) -> VertexOutput {
    var output : VertexOutput;
    output.pos = cameraData.proj * cameraData.view * figmentData.model * vec4<f32>(in_position, 1.0);
    output.normal = in_normal;
    output.uv = in_uv;
    output.color = in_color;
    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
        var output : FragmentOutput;
        output.color = vec4<f32>(input.uv, 0.0, 1.0);
        output.id = figmentData.id;
        return output;
}
