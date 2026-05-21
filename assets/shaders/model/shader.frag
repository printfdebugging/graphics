uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position; /* for diffuse calculation, this is in world space coordinates as well */

in vec3 position; /* in world space coordinates. */
in vec3 color;
in vec2 uv;
in vec3 normal; /* from vertex shader, for diffuse calculation*/

out vec4 outColor;

void main() {
        /* we are basically using 1/10th of the light color as ambient light :)*/
        float ambient_strength = 0.1;
        vec3  ambient_lighting = ambient_strength * light_color;

        /* this points towards the light. i think this is so because the normal points outwards
         *  and when they both point in the same direction, the angle between them will remain between 0
         * and 90. */
        vec3  unit_light_direction = normalize(light_position - position);
        vec3  unit_normal          = normalize(normal);
        float diffuse              = max(dot(unit_normal, unit_light_direction), 0.0); /* diffuse impact of light */
        vec3  diffuse_lighting     = diffuse * light_color;

        vec3 result = (ambient_lighting + diffuse_lighting) * object_color;

        outColor = vec4(result, 1.0);
}
