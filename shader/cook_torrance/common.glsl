
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float D_GGX(float NoH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float b = (NoH2 * (alpha2 - 1.0) + 1.0);
    return alpha2 / (PI * b * b);
}
float G1_GGX_Schlick(float NdotV, float roughness)
{
    // float r = roughness; // original
    float r = 0.5 + 0.5 * roughness; // Disney remapping
    float k = (r * r) / 2.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float G_Smith(float NoV, float NoL, float roughness)
{
    float g1_l = G1_GGX_Schlick(NoL, roughness);
    float g1_v = G1_GGX_Schlick(NoV, roughness);
    return g1_l * g1_v;
}
vec3 microfacetBRDF(in vec3 L, in vec3 V, in vec3 N, in vec3 baseColor, in float metallicness, in float fresnelReflect, in float roughness)
{

    vec3 H = normalize(V + L); // half vector

    // all required dot products
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    float NoL = clamp(dot(N, L), 0.0, 1.0);
    float NoH = clamp(dot(N, H), 0.0, 1.0);
    float VoH = clamp(dot(V, H), 0.0, 1.0);

    // F0 for dielectics in range [0.0, 0.16]
    // default FO is (0.16 * 0.5^2) = 0.04
    vec3 f0 = vec3(0.16 * (fresnelReflect * fresnelReflect));
    // in case of metals, baseColor contains F0
    f0 = mix(f0, baseColor, metallicness);

    // specular microfacet (cook-torrance) BRDF
    vec3 F = fresnelSchlick(VoH, f0);
    float D = D_GGX(NoH, roughness);
    float G = G_Smith(NoV, NoL, roughness);
    vec3 spec = (D * G * F) / max(4.0 * NoV * NoL, 0.001);

    // diffuse
    vec3 notSpec = vec3(1.0) - F; // if not specular, use as diffuse
    notSpec *= 1.0 - metallicness; // no diffuse for metals
    vec3 diff = notSpec * baseColor / PI;

    return diff + spec;
}
