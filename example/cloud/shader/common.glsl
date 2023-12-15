float Beer(float density, float absorptivity = 1)
{
    return exp(-density * absorptivity);
}

float BeerPowder(float density float absorptivity = 1)
{
    return 2.0f * exp(-density * absorptivity) * (1.0 - exp(-2.0f * density));
}

float GetLightEnergy(float density, float absorptivity, float darknessThreshold)
{
    float energy = BeerPowder(density, absorptivity);
    return darknessThreshold + (1.0 - darknessThreshold) * energy;
}

// Henyey-Greenstein
float HenyeyGreenstein(float angle, float g)
{
    float g2 = g * g;
    return (1 - g2) / (4 * 3.1415 * pow(1 + g2 - 2 * g * (angle), 1.5));
}

// float phase(float a)
// {
//     float blend = .5;
//     float hgBlend = hg(a, 0.72f) * (1 - blend) + hg(a, -1.f) * blend;
//     return 0.5f + hgBlend * 1.58f;
// }