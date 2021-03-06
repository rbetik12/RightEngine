#version 450 core
layout (location = 0) out vec4 aAlbedo;
layout (location = 1) out vec4 aNormal;

in vec2 f_UV;
in vec3 f_Normal;
in vec3 f_WorldPos;
in mat3 f_TBN;

layout(std140, binding = 0) uniform MaterialData
{
    vec4 u_Albedo;
    float u_Metallic;
    float u_Roughness;
    bool u_HasAlbedo;
    bool u_HasNormal;
    bool u_HasMetallic;
    bool u_HasRoughness;
    bool u_HasAO;
};

uniform sampler2D u_Textures[5];
uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BRDFLUT;

struct Light
{
    vec4 color;
    vec4 position;
    vec4 rotation;
    float intensity;
    int type;
    vec2 _padding_;
};

layout(std140, binding = 1) uniform LightBuffer
{
// X - Overall lights amount
// Y - Directional lights amount
// Z - Point lights amount
// W - TBD
    ivec4 u_LightsAmount;
    Light u_Light[30];
};

uniform vec3 u_CameraPosition;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_Textures[1], f_UV).xyz;
    tangentNormal = tangentNormal * 2.0 - 1.0;
    return normalize(f_TBN * tangentNormal);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 albedo;
    vec3 N;
    float metallic;
    float roughness;
    float ao;

    if (u_HasAlbedo)
    {
        albedo = pow(texture(u_Textures[0], f_UV).rgb, vec3(2.2));
    }
    else
    {
        albedo = u_Albedo.rgb;
    }

    if (u_HasNormal)
    {
        N = getNormalFromMap();
    }
    else
    {
        N = f_Normal;
    }

    if (u_HasMetallic)
    {
        metallic = texture(u_Textures[2], f_UV).r;
    }
    else
    {
        metallic = u_Metallic;
    }

    if (u_HasRoughness)
    {
        roughness = texture(u_Textures[3], f_UV).r;
    }
    else
    {
        roughness = u_Roughness;
    }

    if (u_HasAO)
    {
        ao = texture(u_Textures[4], f_UV).r;
    }
    else
    {
        ao = 1.0f;
    }

    vec3 V = normalize(u_CameraPosition - f_WorldPos);
    vec3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < u_LightsAmount.x; ++i)
    {
        if (u_Light[i].type > 0)
        {
            albedo += 0.1f;
        }
        if (u_Light[i]._padding_.x > 0)
        {
            albedo += 0.1f;
        }
        // calculate per-light radiance
        vec3 L = normalize(vec3(u_Light[i].position) - f_WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(vec3(u_Light[i].position) - f_WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = vec3(u_Light[i].color) * u_Light[i].intensity * attenuation;
        //        albedo = u_Light[i].color * u_Light[i].intensity;
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;// + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(u_IrradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    const float MAX_REFLECTION_LOD = 1.2;
    vec3 prefilteredColor = textureLod(u_PrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(u_BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    //    vec2 brdf = vec2(1.0, 1.0);
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    aAlbedo = vec4(color, 1.0);
    aNormal = vec4(N, 1.0);
}