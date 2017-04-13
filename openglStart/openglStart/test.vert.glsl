in vec3 coord3d;
in vec3 v_color;
in vec2 texcoord;
uniform mat4 m_transform;
uniform mat4 mvp;
out vec3 f_color;
out vec2 f_texcoord;

void main(void) {
	gl_Position = mvp * vec4(coord3d, 1.0);
	f_texcoord = texcoord;
}
