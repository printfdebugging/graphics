uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;  /* for diffuse calculation, this is in world space coordinates as well */
uniform vec3 camera_position; /* for specular calculation, this is also world space coordinate.*/

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

        float specular_strength   = 0.5;  // this is ideally a property of the material.
        vec3  unit_view_direction = normalize(camera_position - position);
        /* here the light direction is negative because previously we calculated it from fragment to light
         * for the dot product, but reflect expects it to be from source to fragment like any incident ray */
        vec3 unit_reflected_direction = normalize(reflect(-unit_light_direction, normal));
        /* the tutorial says that this power of 32 thing is the shininess value of highlight, higher this
         * value, higher light gets reflected. so we are essentially raising 32 to the power of a number between
         * 0 and 1. so at max the value will be 32 and at least 1. spec = [1, 32] */
        float specular = pow(max(dot(unit_view_direction, unit_reflected_direction), 0.0), 32);
        /* so half the light is reflected (0.5 = specular_strength) * a number from range [1, 32](= shininess_number let's say) * light_color */
        /* = 1/2 * shininess_number * light_color = (shininess_number/shininess_strength) * light_color = some multipe of light_color */
        vec3 specular_lighting = specular_strength * specular * light_color;

        vec3 result = (ambient_lighting + diffuse_lighting + specular_lighting) * object_color;

        outColor = vec4(result, 1.0);
}
