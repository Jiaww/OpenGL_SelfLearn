uniform float fade;
in vec3 f_color;
in vec2 f_texcoord;
uniform sampler2D mytexture;

void main(void) {
	vec2 flipped_texcoord = vec2(f_texcoord.x, 1.0 - f_texcoord.y);
	gl_FragColor = texture2D(mytexture, flipped_texcoord);
	//gl_FragColor[3] = floor(mod(gl_FragCoord.y, 2.0));
}
