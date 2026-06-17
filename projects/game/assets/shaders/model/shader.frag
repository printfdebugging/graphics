/**!
 * material properties
 * @var mat_diffuse_map  -   index of the texture containing the diffuse colors each fragment reflects
 * @var mat_specular_map -   index of the (typically) monochrome texture representing the shiny parts of the object
 * @var mat_shininess    -   shininess value, larger the shininess, lesser scattering of light, clear reflection, more metallic
 */
uniform sampler2D mat_diff_map;
uniform sampler2D mat_spec_map;
uniform sampler2D mat_emit_map;
uniform float mat_shininess;

/**!
 * camera properties
 * @var cam_pos         - camera position in world space coordinates
 * @var cam_front_dir   - the direction vector of the camera's face i.e. where the player is looking
 */
uniform vec3 cam_pos;
uniform vec3 cam_front_dir;

/**!
 * directional light properties
 * @var dlt_dir  - directional light direction
 * @var dlt_amb  - ambient intensity of the directional light
 * @var dlt_diff - diffuse intensity of the directional light
 * @var dlt_spec - specular intensity of the directional light
 */
uniform vec3 dlt_dir;
uniform vec3 dlt_amb;
uniform vec3 dlt_diff;
uniform vec3 dlt_spec;

/**!
 * vertex shader output variables
 *  @param [in] position - position of the fragment in world space coordinates
 *  @param [in] uv       - texture coordinates
 *  @param [in] normal   - normal at the fragment surface
 */
in vec3 position;
in vec2 uv;
in vec3 normal;

/**!
 * fragment shader output variables;
 * @param [out] out_color  - output color from the fragment shader
 */
out vec4 out_color;

/**!
 * point light properties
 * @attr plt_attr_const, plt_attr_linear, plt_attr_quad - attenuation factors to vary intensity based on distance
 * @attr plt_amb, plt_diff, plt_spec                    - light intensities of a point light
 * @attr plt_pos                                        - position in world space coordinates
 */
uniform vec3 plt_pos;
uniform vec3 plt_amb;
uniform vec3 plt_diff;
uniform vec3 plt_spec;

uniform float plt_att_const;
uniform float plt_att_linear;
uniform float plt_att_quad;

/**!
 * @param [in] lightpos - position of the point light (in world space coordinates)
 * @param [in] amb      - ambient component of the light
 * @param [in] diff     - diffuse component of the light
 * @param [in] spec     - specular component of the light
 * @param [in] cons     - attenuation constant factor
 * @param [in] linear   - attenuation linear factor (wrt distance)
 * @param [in] quad     - attenuation quadratic factor (wrt distance)
 * @param [in] _cam_dir - unit vector pointing from the fragment towards the camera
 * @param [in] _normal  - unit normal vector at the fragment
 */
vec3 calc_plt(vec3 lightpos, vec3 amb, vec3 diff, vec3 spec, float cons, float linear, float quad, vec3 _cam_dir, vec3 _normal);

/**!
 * @param [in] lightdir - direction of the light, from a source infinitely far away towards the fragments
 * @param [in] amb      - ambient component of the light
 * @param [in] diff     - diffuse component of the light
 * @param [in] spec     - specular component of the light
 * @param [in] _cam_dir - unit vector pointing from the fragment towards the camera
 * @param [in] _normal  - unit normal vector at the fragment
 */
vec3 calc_dlt(vec3 lightdir, vec3 amb, vec3 diff, vec3 spec, vec3 _cam_dir, vec3 _normal);

/**!
 * @info variables define with an '_' (underscore) at the front are normalized
 */
void main() {
	vec3 _normal = normalize(normal);
	vec3 _cam_dir = normalize(cam_pos - position);

	vec3 color = calc_plt(plt_pos, plt_amb, plt_diff, plt_spec, plt_att_const, plt_att_linear, plt_att_quad, _cam_dir, _normal);
	color += calc_dlt(dlt_dir, dlt_amb, dlt_diff, dlt_spec, _cam_dir, _normal);
	out_color = vec4(color, 1.0);
}

vec3 calc_plt(vec3 lightpos, vec3 amb, vec3 diff, vec3 spec, float cons, float linear, float quad, vec3 _cam_dir, vec3 _normal) {
	vec3 _dir = normalize(lightpos - position); /* unit vector from fragment to light */
	vec3 _reflected = normalize(reflect(-_dir, _normal));
	float dist = distance(position, lightpos);
	float att = 1.0 / ((cons) + (linear * dist) + (quad * (dist * dist)));

	float fdiff = max(dot(_dir, _normal), 0.0);
	float fspec = pow(max(dot(_reflected, _cam_dir), 0.0f), mat_shininess);

	vec3 light_amb = amb * texture(mat_diff_map, uv).rgb;
	vec3 light_diff = diff * fdiff * texture(mat_diff_map, uv).rgb;
	vec3 light_spec = spec * fspec * texture(mat_spec_map, uv).rgb;

	return (light_amb + light_diff + light_spec) * att;
}

vec3 calc_dlt(vec3 lightdir, vec3 amb, vec3 diff, vec3 spec, vec3 _cam_dir, vec3 _normal) {
	vec3 _dir = normalize(-lightdir); /* unit vector from fragment towards light source */
	vec3 _reflected = normalize(reflect(-_dir, _normal));

	float fdiff = max(dot(_dir, _normal), 0.0f);
	float fspec = pow(max(dot(_reflected, _cam_dir), 0.0f), mat_shininess);

	vec3 light_amb = amb * texture(mat_diff_map, uv).rgb;
	vec3 light_diff = diff * fdiff * texture(mat_diff_map, uv).rgb;
	vec3 light_spec = spec * fspec * texture(mat_spec_map, uv).rgb;
	return (light_amb + light_diff + light_spec);
}
