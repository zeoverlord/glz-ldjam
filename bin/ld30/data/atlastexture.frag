#version 140


in vec4 txcoord;
out vec4 fragment_color;

uniform sampler2D texunit0; // atlas reference map
uniform sampler2D texunit1; // atlas map
uniform  int layer;
uniform  int anim;

uniform int width;  // height and width of the atlas reference map
uniform int height;

uniform int a_width;  // height and width of the atlas map
uniform int a_height;

void main() {
        
float sx=0.0;
float sy=0.0;

float pb=0.001;

//int width=16;  // height and width of the atlas reference map
//int height=16;

//int a_width=4;  // height and width of the atlas map
//int a_height=4;



vec4 arm = texture2D(texunit0, txcoord.xy);

if (layer==0)
{
sx=floor(arm.x*255.0);
sy=floor(arm.y*255.0);
}

if (layer==1)
{
sx=floor(arm.z*255.0);
sy=floor(arm.w*255.0);
}

float atl=0.0;

if (layer==2)
{
atl=floor(arm.x*255.0);
sx=mod(atl,16.0);
sy=floor(atl/16.0);
}


if (layer==3)
{
atl=floor(arm.y*255.0);
sx=mod(atl,16.0);
sy=floor(atl/16.0);
}


if (layer==4)
{
atl=floor(arm.z*255.0);
sx=mod(atl,16.0);
sy=floor(atl/16.0);
}

if (layer==5)
{
atl=floor(arm.w*255.0);
sx=mod(atl,16.0);
sy=floor(atl/16.0);
}


int  animate=0;
float extra=0.0;

//strip animation flag
if (sx>127) 
    { 
      sx-=128;
      animate=1;
    }

//strip extra flag
if (sy>127) 
    { 
      sy-=128;
      extra=1.0;
    }

//get animation block
float ani_b=0;
float ani_o=mod(modf(sx/4.0,ani_b)*4.0+anim,4.0);

if (animate==1) sx=(ani_b*4.0)+ani_o;

vec2 newtx;

if (extra==1)
newtx = vec2(fract(-txcoord.x*width),fract(txcoord.y*height))*vec2(1.0-(pb*2),1.0-(pb*2))+ vec2(pb,pb);
else
newtx = vec2(fract(txcoord.x*width),fract(txcoord.y*height))*vec2(1.0-(pb*2),1.0-(pb*2))+ vec2(pb,pb);

sy=(a_height-1)-sy;

newtx /= vec2(a_width,a_height);
newtx.x+=(1.0/a_width)*sx;
newtx.y+=(1.0/a_height)*sy;

vec4 am = texture2D(texunit1, newtx.xy);

fragment_color=am;
//fragment_color=vec4(newtx.xy,0.0,1.0);
//fragment_color=vec4(1.0,0.0,0.0,1.0);


}             