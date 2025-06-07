shader_type canvas_item;

uniform uint bucket_count = 500;
uniform float[1000] bucket_data;
uniform float height_scale = 1.0;
uniform vec4 bg_color: source_color = vec4(0.0);
uniform vec4 fg_color: source_color = vec4(1.0);
uniform bool bars = false;

float lerp(float v0, float v1, float t) {
	t = clamp(0.0, 1.0, t);
	return (1.0-t)*v0 + t*v1;
}

void fragment() {

	float bucket_value = float(bucket_count) * UV.x;
	float bucket_index_float = floor(bucket_value);
	float bucket_progress = bucket_value - bucket_index_float;
	uint bucket_index = uint(bucket_index_float);

	float height = 0.0;

	if (!bars) {

		uint next_bucket_index = min(bucket_index+uint(1), bucket_count-uint(1));

		float current_height = bucket_data[bucket_index];
		float next_height = bucket_data[next_bucket_index];

		height = lerp(current_height,next_height, smoothstep(0.0,1.0,bucket_progress));


	} else {
		height = bucket_data[bucket_index];
	}



	height *= height_scale;

	float y = (1.0-UV.y);

	if (y < height) {
		COLOR = fg_color;
	} else {
		COLOR = bg_color;
	}



}

