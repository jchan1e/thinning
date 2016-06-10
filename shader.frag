//
// Force Based Thinning shader

uniform float DX;
uniform float DY;
uniform sampler2D img;
uniform sampler2D borders;

vec4 sample(sampler2D tex, float dx, float dy)
{
   return texture2D(tex,gl_TexCoord[0].st+vec2(dx,dy));
}

void main()
{
   if (sample(borders, 0.0,0.0) != vec4(1.0,0.0,0.0,1.0))
   {
      vec2 Ful = vec2(0.0,0.0);
      vec2 Fur = vec2(0.0,0.0);
      vec2 Fbl = vec2(0.0,0.0);
      vec2 Fbr = vec2(0.0,0.0);
#pragma unroll
      for (float j = -20.0; j <= 20.0; j = j + 1.0)
      {
#pragma unroll
         for (float i = -20.0; i <= 20.0; i = i + 1.0)
         {
            if (vec2(i,j) != vec2(0.0,0.0) && sample(borders, i,j) == vec4(1.0,0.0,0.0,1.0))
            {
               //bool outofsight = false;
               //for (vec2 there = vec2(i,j); abs(there.x) > 0.0; there = there - vec2(i,j)/max(abs(i),abs(j)))
               //   outofsight = outofsight || (sample(borders, there.x, there.y).r > 0.5);
               //if (!outofsight)
               //{
                  float d;
                  d = length(vec2(-0.5,-0.5) - vec2(i,j));
                  Ful = Ful + normalize(vec2(-0.5,-0.5) - vec2(i,j)) / (d*d);
                  d = length(vec2(+0.5,-0.5) - vec2(i,j));
                  Fur = Fur + normalize(vec2(+0.5,-0.5) - vec2(i,j)) / (d*d);
                  d = length(vec2(-0.5,+0.5) - vec2(i,j));
                  Fbl = Fbl + normalize(vec2(-0.5,+0.5) - vec2(i,j)) / (d*d);
                  d = length(vec2(+0.5,+0.5) - vec2(i,j));
                  Fbr = Fbr + normalize(vec2(+0.5,+0.5) - vec2(i,j)) / (d*d);
               //}
            }
         }
      }
      float DN = dot(Ful,Fur);
      float DS = dot(Fbl,Fbr);
      float DE = dot(Fur,Fbr);
      float DW = dot(Ful,Fbl);
      float DX = dot(Ful,Fbr);
      float DY = dot(Fur,Fbl);
      if (DN < 0.0 || DS < 0.0 || DE < 0.0 || DW < 0.0 || DX < 0.0 || DY < 0.0)
      {
         gl_FragColor = vec4(0.0,0.0,0.0,1.0);
      }
      else
      {
         if (sample(img, 0.0,0.0) == vec4(1.0,0.0,0.0,1.0))
         {
            gl_FragColor = vec4(1.0,1.0,1.0,1.0);
         }
         else
         {
            gl_FragColor = vec4(0.0,1.0,0.0,1.0);
         }
      }
   }
   else
   {
      gl_FragColor = vec4(1.0,0.0,0.0,1.0);
   }
}
