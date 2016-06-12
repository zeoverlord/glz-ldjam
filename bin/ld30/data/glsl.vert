#version 140


in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;
uniform  mat4 projMat;
out vec4 color;


void main() {

	vec4 pos=vec4(Position.xyz,1.0);
	gl_Position=projMat*pos;
	color=vec4(TexCoord.xy,0.0,1.0);

}   