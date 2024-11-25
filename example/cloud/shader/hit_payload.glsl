struct hitPayload {
    vec3 hitValue;
    int depth;
    vec3 ray_origin;
    vec3 ray_dir;
    int hashit;
};
struct CloudPayload {
    float total_density;
    int depth;
    vec3 ray_origin;
    vec3 ray_dir;
    vec3 color;
};
struct light_payload {
    vec3 ray_origin;
    // vec3 ray_end;
    vec3 ray_dir;
    int steps;
    float total_density;
};

/*
    missIndex = 0 skybox
    missIndex = 1 raymarching cloud cur_density
    missIndex = 2 raymarching from cloud_point to light
*/