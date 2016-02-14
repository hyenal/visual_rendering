#version 330 core

// Interpolated values from the vertex shaders
in vec2 fragmentUV;

// Values that stay constant
uniform sampler2D myTextureSamplerVolume;
uniform sampler2D myTextureSamplerNormals;

// Rotation angle
uniform mat3 rot_mat;
// Iso value
uniform float iso;

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

// Input: (x,y,angle) coordinates of the point in the volume and rotation angle, between (0,0,0) and (1,1,2Pi)
// Output: (rotx, roty) rotate pixels
vec2 rotation(vec2 xy)
{
  vec2 newPos = mat2(rot_mat)*(xy-0.5f)+0.5f;
  return clamp(newPos,0.0f,1.0f);
}

void main()
{ 
      vec2 pixCoord;
      vec2 rotPix;
      float x,y,z;
	  x = fragmentUV.x;
      y = fragmentUV.y;

	  //extract one horizontal slice (x and y vary with fragment coordinates, z is fixed)
      /*z = 82./100.; //extract 82nd slice
      pixCoord = pixel_coordinate(x,y,z);
      color = texture(myTextureSamplerVolume, pixCoord).rgb;
	  */
    
      //Accumulate all horizontal slices 
      /*vec3 h_slices = vec3(0.f, 0.f, 0.f);
      for(int i=0; i< 100; ++i) {
        z = float(i)/100.f;
        pixCoord = pixel_coordinate(x,y,z);
        h_slices += texture(myTextureSamplerVolume, pixCoord).rgb;
      }
      h_slices /= 100.f;
      color = h_slices;*/

      //Extract one vertical slice (x and z vary with fragment coordinates, y is fixed)
      /*z = 100./256.; //extract 100th pixel
      pixCoord = pixel_coordinate(x,z,y);
      color = texture(myTextureSamplerVolume, pixCoord).rgb;
	  */

      //Accumulate all vertical slices 
      /*vec3 v_slices = vec3(0.f, 0.f, 0.f);
      for(int i=0; i< 256; ++i) {
        // Get the ith y direction
        z = float(i)/256.f;
        // Get pixel coordinate
        pixCoord = pixel_coordinate(x,z,y);
        // Accumulate result
        v_slices += texture(myTextureSamplerVolume, pixCoord).rgb;
      }
      // Normalize
      v_slices /= 256.f;
      color = v_slices;*/


      //Accumulate all vertical slices after rotation by 
      // rot_mat matrix around the z axis
      vec3 slices = vec3(0.f, 0.f, 0.f);
      for(int i=0; i< 256; ++i) {
        // Get ith y pixel
        z = float(i)/256.f;
        // Rotate domain
        rotPix = rotation(vec2(x,z));
        // Get pixel coordinate
        pixCoord = pixel_coordinate(rotPix.x,rotPix.y,y);
        // Accumulate result
        slices += texture(myTextureSamplerVolume, pixCoord).rgb;
      }
      // Normalize color
      slices /= 256.f;
      color = slices;


	  //Part 2
      //Ray marching until density above a threshold (i.e., extract an iso-surface)
      float isInside = 0.0f;
      for(int i=0; i< 256; ++i) {
        // Same thing as before
        z = float(i)/256.f;
        rotPix = rotation(vec2(x,z));
        pixCoord = pixel_coordinate(rotPix.x,rotPix.y,y);
        // Check if we have reached the threshold
        if(texture(myTextureSamplerVolume, pixCoord).r > iso){
        	isInside = 1.0f;
        	break;
        }
      }
	  //Display the wole object in dark grey + the volume inside the isosurface in a lighter shader
      //color *= (1.0+isInside);


	 //Part 3
     //Ray marching until density above a threshold, display iso-surface normals
     //color = isInside*texture(myTextureSamplerVolume, pixCoord).rgb;
	 color = isInside*texture(myTextureSamplerNormals, pixCoord).rgb;


  
/* 
    //Ray marching until density above a threshold, display shaded iso-surface
    //...
*/


}
