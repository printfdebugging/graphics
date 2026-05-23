uniform vec3 light_color;
uniform vec3 light_position;  /* for diffuse calculation, this is in world space
                                 coordinates as well */
uniform vec3 camera_position; /* for specular calculation, this is also world
                                 space coordinate.*/

uniform vec3 material_specular;
uniform float material_shininess;
uniform sampler2D material_diffuse_map;

in vec3 position; /* in world space coordinates. */
in vec3 color;
in vec2 uv;
in vec3 normal; /* from vertex shader, for diffuse calculation*/

out vec4 outColor;

void
main()
{
   vec3 ambient_lighting = light_color * texture(material_diffuse_map, uv).rgb;

   vec3 unit_light_direction = normalize(light_position - position);
   vec3 unit_normal = normalize(normal);
   float diffuse_factor = max(dot(unit_normal, unit_light_direction), 0.0);
   vec3 diffuse_lighting =
      light_color * (diffuse_factor * texture(material_diffuse_map, uv).rgb);

   vec3 unit_view_direction = normalize(camera_position - position);
   vec3 unit_reflected_direction =
      normalize(reflect(-unit_light_direction, unit_normal));
   float specular_factor =
      pow(max(dot(unit_view_direction, unit_reflected_direction), 0.0),
          material_shininess);
   vec3 specular_lighting = light_color * (specular_factor * material_specular);

   vec3 result = (ambient_lighting + diffuse_lighting + specular_lighting);

   outColor = vec4(result, 1.0);
}
