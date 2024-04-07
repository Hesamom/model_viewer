   #version 330

in vec4 v_position;
uniform mat4 u_MVP;

void main() 
{
    gl_Position = u_MVP * v_position;
}
