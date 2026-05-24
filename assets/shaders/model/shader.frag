uniform vec3 camera_position;

uniform float material_shininess;
uniform sampler2D material_diffuse_map;
uniform sampler2D material_specular_map;
uniform sampler2D material_emission_map;

uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;
uniform float time;

in vec3 position; /* in world space coordinates. */
in vec3 color;
in vec2 uv;
in vec3 normal; /* from vertex shader, for diffuse calculation*/

out vec4 outColor;

void
main()
{
   vec3 ambient_lighting =
      light_ambient * texture(material_diffuse_map, uv).rgb;

   vec3 unit_light_direction = normalize(light_position - position);
   vec3 unit_normal = normalize(normal);
   float diffuse_factor = max(dot(unit_normal, unit_light_direction), 0.0);
   vec3 diffuse_lighting =
      light_diffuse * (diffuse_factor * texture(material_diffuse_map, uv).rgb);

   vec3 unit_view_direction = normalize(camera_position - position);
   vec3 unit_reflected_direction =
      normalize(reflect(-unit_light_direction, unit_normal));
   float specular_factor =
      pow(max(dot(unit_view_direction, unit_reflected_direction), 0.0),
          material_shininess);

   vec3 specular_lighting =
      light_specular *
      (specular_factor * texture(material_specular_map, uv).rgb);

   vec3 emission_lighting = vec3(0.0f);
   if (texture(material_specular_map, uv).rgb == vec3(0.0f)) {
      emission_lighting =
         texture(material_emission_map, uv + vec2(sin(time), 0.0f)).rgb;
   }

   vec3 result = (ambient_lighting + diffuse_lighting + specular_lighting) +
                 emission_lighting;

   outColor = vec4(result, 1.0);
}
