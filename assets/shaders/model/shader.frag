uniform vec3 camera_position;
uniform vec3 camera_front;

uniform float material_shininess;
uniform sampler2D material_diffuse_map;
uniform sampler2D material_specular_map;
uniform sampler2D material_emission_map;

uniform vec3 light_position;
// uniform vec3 light_direction;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform float light_attr_constant;
uniform float light_attr_linear;
uniform float light_attr_quadratic;

uniform float time;

in vec3 position; /* in world space coordinates. */
in vec3 color;
in vec2 uv;
in vec3 normal; /* from vertex shader, for diffuse calculation*/

out vec4 outColor;

void main() {
        vec3 ambient_lighting = light_ambient * texture(material_diffuse_map, uv).rgb;

        vec3 unit_light_direction = normalize(light_position - position);
        vec3 unit_normal = normalize(normal);
        float diffuse_factor = max(dot(unit_normal, unit_light_direction), 0.0);
        vec3 diffuse_lighting = light_diffuse * (diffuse_factor * texture(material_diffuse_map, uv).rgb);

        vec3 texture_rgb = texture(material_specular_map, uv).rgb;
        vec3 unit_view_direction = normalize(camera_position - position);
        vec3 unit_reflected_direction = normalize(reflect(-unit_light_direction, unit_normal));
        float specular_factor = pow(max(dot(unit_view_direction, unit_reflected_direction), 0.0), material_shininess);
        vec3 specular_lighting = light_specular * (specular_factor * texture_rgb);

        // vec3 emission_lighting = vec3(0.0f);
        // if (texture_rgb == vec3(0.0f)) {
        //         emission_lighting = texture(material_emission_map, uv + vec2(sin(time), 0.0f)).rgb;
        // }

        float distance = length(light_position - position);
        float attenuation = 1.0 / (light_attr_constant + (light_attr_linear * distance) + (light_attr_quadratic * (distance * distance)));

        vec3 result = (ambient_lighting + diffuse_lighting + specular_lighting) * attenuation;

        outColor = vec4(result, 1.0);

        /* todo: properly define #defines to enable/disable features from c code */
        // float xfactor = max(dot(camera_front, -unit_view_direction), 0.0f);
        // float xray_factor = pow(xfactor, 320.0f);
        // if (dot(unit_normal, camera_front) < 0.0f && xray_factor > 0.5f) {
        //         discard;
        //         // outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        // }
}
