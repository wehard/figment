struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

struct GridData {
    model: mat4x4<f32>,
    invView: mat4x4<f32>,
    invProj: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;
@binding(1) @group(0) var<uniform> gridData: GridData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) uv: vec2<f32>,
    @location(1) @interpolate(flat) id: i32,
    @location(2) near: vec3<f32>,
    @location(3) far: vec3<f32>,
};

fn unproject(pos: vec3<f32>, invView: mat4x4<f32>, invProj: mat4x4<f32>) -> vec3<f32> {
    let unprojected = invView * invProj * vec4<f32>(pos, 1.0);
    return unprojected.xyz / unprojected.w;
}

@vertex
fn vs_main(@location(0) pos: vec3f, @location(1) uv: vec2f) -> VertexOutput {
    var output: VertexOutput;
    output.near = unproject(vec3<f32>(pos.x, pos.y, 0.0), gridData.invView, gridData.invProj);
    output.far = unproject(vec3<f32>(pos.x, pos.y, 1.0), gridData.invView, gridData.invProj);
//    output.pos = cameraData.proj * cameraData.view * gridData.model * vec4<f32>(pos, 1.0);
    output.pos = vec4<f32>(pos, 1.0);
    output.uv = uv;
    output.id = -1;
    return output;
}

struct FragmentOutput {
    @builtin(frag_depth) depth: f32,
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

fn compute_depth(pos: vec3<f32>) -> f32 {
    var clip = cameraData.proj * cameraData.view * vec4<f32>(pos, 1.0);
    return clip.z / clip.w;
}

@fragment
fn fs_main(@location(0) uv: vec2<f32>, @location(1)  @interpolate(flat) id: i32, @location(2) near: vec3<f32>, @location(3) far: vec3<f32>) -> FragmentOutput {
    var output: FragmentOutput;

    var t = -near.y / (far.y - near.y);
    if (t < 0.0) {
        discard;
    }
    var scale = 1.0;
    let pos = near + t * (far - near);
    let coord = pos.xz * scale;
    let deriv = fwidth(coord);
    let grid: vec2<f32> = abs(fract(coord - 0.5) - 0.5) / deriv;
    let ln = min(grid.x, grid.y);
    let minz = min(deriv.y, 1.0);
    let minx = min(deriv.x, 1.0);
    var g = 1.0 - min(ln, 1.0);

    if (g < 0.1) {
        discard;
    }

    var c = vec4<f32>(0.5, 0.5, 0.5, 1.0) * g;

    if (pos.x > -0.1 * minx && pos.x < 0.1 * minx) {
        c.z = 1.0;
    }
    if (pos.z > -0.1 * minz && pos.z < 0.1 * minz) {
        c.x = 1.0;
    }

    output.color = c;
    output.id = id;
    output.depth = compute_depth(pos);
    return output;
}
