#version 300 es
precision mediump float;

uniform vec2 offset;
uniform vec2 pitch;
uniform float zoom;

in vec4 o_col;
out vec4 color;

bool onGridline(int distFrom, int spacing)
{
	return mod(float(distFrom), float(spacing)) == 0.0;
}

void main()
{
	// float zoom = 1.0; // uniform
	int spacingStd = int(pitch.x);
	int effGrid = int(floor((float(spacingStd) * zoom) + 0.5));

	int fragX = int(gl_FragCoord.x - 0.5)-640;
	int fragY = int(gl_FragCoord.y - 0.5)-360;
	int width = 1280; // uniform
	int height = 720; // uniform

	int xDistFrom = int(abs(float((-offset.x * float(width)) - float(fragX))));
	int yDistFrom = int(abs(float((-offset.y * float(height)) - float(fragY))));

	if (xDistFrom == 0 || yDistFrom == 0) {
		color = vec4(0.0, 0.2, 0.4, 1.0);
		return;
	}
	else if (onGridline(xDistFrom, effGrid) || onGridline(yDistFrom, effGrid))
	{
		color = o_col;
	}
	else 
	{
		discard;
	}



	// float ox = (gl_FragCoord.x) + (offset.x * 1280.0);
	// float oy = (gl_FragCoord.y) + (offset.y * 720.0);
	// if (int(mod(ox, pitch.x)) == 0 || int(mod(oy, pitch.y)) == 0)
	// {
	// 	color = o_col;
	// }
	// else
	// {
	// 	discard;
	// }
}
