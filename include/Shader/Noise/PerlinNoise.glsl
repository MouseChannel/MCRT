float fade(float t)
{
    return t * t * t * (t * (t *  6.f -  15.f) + 10.f);
}
float grad(int hash, float x, float y, float z)
{
    // Convert LO 4 bits of hash code into 12 gradient directions
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
 