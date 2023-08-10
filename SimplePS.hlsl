// an ultra simple hlsl pixel shader
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
float4 main(OUTPUT_TO_RASTERIER outer) : SV_TARGET
{
	outer.nrmW = normalize(outer.nrmW);

// Initialize the final color
float4 finalColor = float4(0, 0, 0, 1);

   // Loop over instances
   for (uint instanceIndex = 0; instanceIndex < vertexCount; ++instanceIndex)
   {
   	// Compute instance-specific data using instanceIndex
   
   	// Example: Compute a unique color based on instanceIndex
   	float4 instanceColor = float4(instanceIndex / float(vertexCount), 1, 1, 1);
   
   	// Calculate lighting for this instance (replace this with your lighting calculations)
   	float3 normal = normalize(outer.nrmW);
   	float3 viewDirection = normalize(camPos.xyz - outer.posW);
   	float intensity = max(dot(normal, lightDirection), 0);
   	float3 lightingColor = intensity * lightColor;
   
   	// Combine instance-specific color with lighting calculations
   	float4 instanceResult = instanceColor * float4(lightingColor, 1);
   
   	// Accumulate the instance result into the final color
   	finalColor += instanceResult;
   }

return finalColor;
}
