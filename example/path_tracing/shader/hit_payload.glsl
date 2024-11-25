struct hitPayload {
    vec3 hitValue;

    vec3 rayOrigin;
    uint seed;
    uint depth;
    vec3 rayDirection;
    vec3 weight;
    bool miss;
    vec3 point_position;
    vec3 point_normal;
};