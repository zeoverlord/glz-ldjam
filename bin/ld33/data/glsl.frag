#version 140


in vec4 color;
out vec4 fragment_color;
uniform sampler2D texunit0;
uniform vec4 tint;

void main() {
        
vec4 base = texture2D(texunit0, color.xy);

fragment_color=base*tint;
//fragment_color=vec4(1.0,0.0,0.0,1.0);

}             