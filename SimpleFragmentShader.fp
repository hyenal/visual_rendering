#version 330 core

// Interpolated values from the vertex shaders
in vec2 fragmentUV;

// Values that stay constant
uniform sampler2D myTextureSamplerVolume;

// Ouput data
out vec3 color;

// Input: z coordinate of the point in the volume, between 0 and 1
// Output: grid coordinates (i,j) of the slice where the point lies, between (0,0) and (9,9)
// Warning: use the fonction "floor" to make sure that the coordinates of the slice are integer. For instance, for z = 0.823, the function should return (i=2,j=8) because the closest slice is the 82nd one.
vec2 slice_coordinate(float z)
{
      //rescale z
      float z2 = z*100.;

      //coordinate of the slice
      float j = floor(z2/10.);
      float i = floor(z2 - 10.*j);

      return vec2(i,j);
}

// Input: (x,y,z) coordinates of the point in the volume, between (0,0,0) and (1,1,1)
// Output: (u,v) coordinates of the pixel in the texture
vec2 pixel_coordinate(float x, float y, float z)
{
      vec2 sliceCoord = slice_coordinate(z);

      //coordinate of the pixel in the slice
      float u = x/10.;
      float v = y/10.;

      return vec2(u,v)+slice_coordinate(z)/10.;
}

void main()
{ 
      vec2 pixCoord;
      float x,y,z;

      //extract one horizontal slice (x and y vary with fragment coordinates, z is fixed)
      x = fragmentUV.x;
      y = fragmentUV.y;
      z = 82./100.; //extract 82nd slice
      pixCoord = pixel_coordinate(x,y,z);
      color = texture(myTextureSamplerVolume, pixCoord).rgb;


/*     
      //Accumulate all horizontal slices 
      //...
*/


/*
      //extract one vertical slice (x and z vary with fragment coordinates, y is fixed)
      //...
*/

 
/*
      //Accumulate all vertical slices 
      //...
*/


/*
      //Accumulate all vertical slices after rotation by rotationAngle around the z axis
      //...
*/


/*
     //Ray marching until density above a threshold (i.e., extract an iso-surface)
     //...
*/


/*
     //Ray marching until density above a threshold, display iso-surface normals
     //...
*/

  
/* 
    //Ray marching until density above a threshold, display shaded iso-surface
    //...
*/


}