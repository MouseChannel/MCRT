struct hitPayload {
    vec3 hitValue;
    vec3 dir;
    vec3 indir;

    vec3 rayOrigin;
    uint seed;
    uint depth;
    vec3 rayDirection;
    vec3 weight;
    bool miss;
    vec3 point_position;
    vec3 point_normal;
};
struct hitPayload_light {

    vec3 world_normal;
    vec3 total_light_contribute;
};