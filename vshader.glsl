attribute vec3 vertex;
uniform mat4 matrix;

void main()
{
  gl_Position = matrix * vec4(vertex, 1.0);
}
