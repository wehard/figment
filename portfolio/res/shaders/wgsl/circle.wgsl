struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) wp: vec3<f32>,
    @location(1) color: vec4<f32>,
    @location(2) uv: vec2<f32>,
    @location(3) @interpolate(flat) id: i32
};

@vertex
fn vs_main(@location(0) in_world_pos: vec3f, @location(1) in_local_pos: vec3f, @location(2) in_color: vec4f, @location(3) in_id: i32) -> VertexOutput {
    var output: VertexOutput;
    output.pos = cameraData.proj * cameraData.view * vec4<f32>(in_world_pos, 1.0);
    output.wp = in_world_pos;
    output.uv = in_local_pos.xy;
    output.color = in_color;
    output.id = in_id;
    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

@fragment
fn fs_main(@location(0) wp: vec3<f32>, @location(1) color: vec4<f32>, @location(2) uv: vec2<f32>, @location(3)  @interpolate(flat) id: i32) -> FragmentOutput {
    var output: FragmentOutput;
    var len: f32 = abs(length(uv));
    if(len > 0.5) {
        discard;
    }

    var lightDir: vec3<f32> = normalize(vec3<f32>(1.0, 1.0, -1.0));
    var normal: vec3<f32> = normalize(vec3<f32>(uv, -sqrt(1.0 - dot(uv, uv))));
    var ndotl: f32 = max(dot(normal, lightDir), 0.25);
    output.color = color * (1.0 - len * 0.5) * ndotl;

    output.id = id;
    return output;
}
