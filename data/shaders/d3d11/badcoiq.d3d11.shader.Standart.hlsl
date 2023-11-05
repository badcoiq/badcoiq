/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

Texture2D tex2d_1;
SamplerState tex2D_sampler_1;

struct VSIn
{
   float3 position : POSITION;
	float2 uv : TEXCOORD;
   float3 normal : NORMAL;
   float3 binormal : BINORMAL;
   float3 tangent : TANGENT;
   float4 color : COLOR;
   uint4 boneInds : BONES;
   float4 boneWeights : WEIGHTS;
};

cbuffer cbVertex
{
	double4x4 WVP;
	double4x4 W;
};

cbuffer cbPixel
{
	double4 SunPosition;
	float4 BaseColor;
	float4 AmbientColor;
	float alphaDiscard;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 vColor : COLOR0;
	float3 normal : NORMAL0;
};

struct PSOut
{
    float4 color : SV_Target;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	output.pos   = mul(WVP, double4(input.position.x, input.position.y, input.position.z, 1.0));
	output.uv.x    = input.uv.x;
	output.uv.y    = input.uv.y;
	output.vColor    = input.color;
	output.vColor.w    = 1.f;
	output.normal    = normalize(mul((double3x3)W, input.normal));
	return output;
}

#define MAX_BONE_MATRICES 255

cbuffer cbAnimMatrices
{
	double4x4 g_mConstBoneWorld[MAX_BONE_MATRICES];
};

VSOut VSMainSk(VSIn input)
{
	VSOut output;

	float4 skinnedPos = float4(0.f, 0.f, 0.f, 1.f);
	float3 skinnedNorm = input.normal;
	float4 Pos = float4(input.position,1);
	uint iBone = input.boneInds.x;
	float fWeight = input.boneWeights.x;
	double4x4 m = g_mConstBoneWorld[iBone];
	skinnedPos += fWeight * mul( m, Pos );
	skinnedNorm = fWeight * mul( (float3x3)m, input.normal );

	iBone = input.boneInds.y;
	fWeight = input.boneWeights.y;
	m = g_mConstBoneWorld[iBone];
	skinnedPos += fWeight * mul( m, Pos );
	skinnedNorm += fWeight * mul( (float3x3)m, input.normal );

	iBone = input.boneInds.z;
	fWeight = input.boneWeights.z;
	m = g_mConstBoneWorld[iBone];
	skinnedPos += fWeight * mul( m, Pos );
	skinnedNorm += fWeight * mul( (float3x3)m, input.normal );

	iBone = input.boneInds.w;
	fWeight = input.boneWeights.w;
	m = g_mConstBoneWorld[iBone];
	skinnedPos += fWeight * mul( m, Pos );
	skinnedNorm += fWeight * mul( (float3x3)m, input.normal );

	skinnedPos.w   = 1.f;
	output.pos   = mul(WVP, skinnedPos);
	output.uv.x    = input.uv.x;
	output.uv.y    = input.uv.y;
	output.vColor    = input.color;
	output.normal    = normalize(mul((double3x3)W, skinnedNorm));
	return output;
}

PSOut PSMain(VSOut input)
{
	float3 lightDir = normalize(-SunPosition.xyz);
	float diff = max(dot(input.normal, lightDir), 0.0);

   PSOut output;
   output.color = AmbientColor;

   float4 textureColor = tex2d_1.Sample(tex2D_sampler_1, input.uv) * BaseColor;

	if(diff>1.f)
		diff = 1.f;
		
	output.color +=  BaseColor * diff;
	output.color *=  textureColor;

	//if(output.color.w < alphaDiscard)
	//	discard;
		
    return output;
}
