#pragma pack_matrix( row_major )
struct ModelData
{
    float3 Kd; // diffuse reflectivity
    float d; // dissolve (transparency) 
    float3 Ks; // specular reflectivity
    float Ns; // specular exponent
    float3 Ka; // ambient reflectivity
    float sharpness; // local reflection map sharpness
    float3 Tf; // transmission filter
    float Ni; // optical density (index of refraction)
    float3 Ke; // emissive reflectivity
    uint illum; // illumination model
};
cbuffer Model : register(b0)
{
    uint vertexCount;
    uint indexCount;
    uint materialCount;
    uint meshCount;
    uint vertexStart;
    uint indexStart;
    uint materialStart;
    uint meshStart;
    uint batchStart;
};
cbuffer ModelDataBuffer : register(b1)
{
    float4x4 worldMatrix;
    ModelData data;
};
cbuffer Scene : register(b2)
{
    float4x4 viewMatrix;
    float4x4 pMatrix;
    float3 lightDirection;
    float3 lightColor;
    float4 SunAmbient;
    float4 camPos;
};

struct OUTPUT_TO_RASTERIER
{
    float4 posH : SV_Position; //Homogenous projection Space  
    float3 nrmW : NORMAL; // normal in world space(for lighting)
    float3 posW : WORLD; // position in world space(for lighting)
};
struct Vertex
{
    float3 position : Position; //positions
    float3 uvw : UVW; // UVWs
    float3 normals : NORMAL; // Normals
};
OUTPUT_TO_RASTERIER main(float3 inputVertex : POSITION, float3 inputT : TEXCOORD, float3 inputNom : NORMAL)
{
    OUTPUT_TO_RASTERIER output;
    float4 pos = float4(inputVertex, 1);
    pos = mul(pos, worldMatrix);
    output.posW = pos.xyz;
    pos = mul(pos, viewMatrix);
    pos = mul(pos, pMatrix);
    output.posH = pos;
    float4 norm = mul(float4(inputNom, 1), worldMatrix);
    output.nrmW = norm.xyz;
    return output;
}