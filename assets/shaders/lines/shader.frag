
in vec3 color;

out vec4 out_color;

void main()
{
    out_color = vec4(color.rgb, 1.0);
}
