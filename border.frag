//
// Force Based Thinning shader

uniform float DX;
uniform float DY;
uniform sampler2D img;
//uniform sampler2D borders;

//uniform int num_borders;
//uniform vec2* borders;

vec4 sample(float dx, float dy)
{
   return texture2D(img,gl_TexCoord[0].st+vec2(dx,dy));
}

void main()
{
   bool b = false;

   b = b || (sample(-DX,-DY) == vec4(0.0,0.0,0.0,1.0));
   b = b || (sample(-DX,0.0) == vec4(0.0,0.0,0.0,1.0));
   b = b || (sample(-DX,+DY) == vec4(0.0,0.0,0.0,1.0));
   b = b || (sample(0.0,-DY) == vec4(0.0,0.0,0.0,1.0));
   b = b || (sample(0.0,+DY) == vec4(0.0,0.0,0.0,1.0));
   b = b || (sample(+DY,-DY) == vec4(0.0,0.0,0.0,1.0));
   b = b || (sample(+DY,0.0) == vec4(0.0,0.0,0.0,1.0));
   b = b || (sample(+DY,+DY) == vec4(0.0,0.0,0.0,1.0));
   b = b && (sample(0.0,0.0) == vec4(1.0,0.0,0.0,1.0));

   if (b)
      gl_FragColor = vec4(1.0,0.0,0.0,1.0);
   else
      gl_FragColor = vec4(0.0,0.0,0.0,1.0);
}
