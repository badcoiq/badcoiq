/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
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

struct VSIn
{
	uint vertexID : SV_VertexID;
};

cbuffer cbVertex
{
	double4x4 VP;
	double4 P1;
	double4 P2;
	float4 Color;
};

struct VSOut
{
   float4 pos : SV_POSITION;
   float4 color : COLOR0;
};

struct PSOut
{
    float4 color : SV_Target;
};

VSOut VSMain(VSIn input)
{
	double4 vertices[2] = 
	{
		double4( P1.xyz, 1.0),
		double4( P2.xyz, 1.0)
	};

    VSOut output;
	output.pos  = mul(VP, vertices[input.vertexID]);
	output.color = Color;
	return output;
}

PSOut PSMain(VSOut input)
{
    PSOut output;
    output.color = input.color;
    return output;
}


/*
Texture2D tex2d_1;
SamplerState tex2D_sampler_1;
struct VSIn
{
   float3 position : POSITION;
   float2 uv : TEXCOORD;
};

cbuffer cbElement
{
	double4x4 W;
	double4x4 V;
	double4x4 P;
	double4x4 Vi;
	double4 Corners;
	float4 Color1;
	float4 UVs;
	float alphaDiscard;
	float padding[3];
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

struct PSOut
{
    float4 color : SV_Target;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	output.pos.xyz   = input.position.xyz;
	output.pos.w   = 1.f;
	output.uv   = input.uv;
	return output;
}

PSOut PSMain(VSOut input)
{
	PSOut output;
	output.color = saturate(tex2d_1.Sample(tex2D_sampler_1, input.uv) * input.color);
	if(output.color.w < alphaDiscard) discard;
		return output;
}

[maxvertexcount(4)]
void GSMain(point VSOut input[1], inout TriangleStream<VSOut> TriStream )
{
	VSOut Out;

	double4x4 V2 = Vi;
	V2._41 = 0.0;
	V2._42 = 0.0;
	V2._43 = 0.0;
	V2._44 = 1.0;
	V2._14 = 0.0;
	V2._24 = 0.0;
	V2._34 = 0.0;

   double4 v1 = double4(Corners.x, Corners.y, 0.0, 1.0);
   double4 v2 = double4(Corners.z, Corners.w, 0.0, 1.0);

	Out.pos   =  mul(W, mul(V2,double4(v2.x, v1.y, 0.0, 1.0)));
	Out.pos   =  mul(V, Out.pos);
	Out.pos   =  mul(P, Out.pos);
	Out.uv = float2(UVs.z,UVs.w); // 3
	Out.color = Color1;
	TriStream.Append(Out);

	Out.pos   =  mul(W, mul(V2,double4(v1.x, v1.y, 0.0, 1.0)));
	Out.pos   =  mul(V, Out.pos);
	Out.pos   =  mul(P, Out.pos);
	Out.uv = float2(UVs.x,UVs.w);// 4
	Out.color = Color1;
	TriStream.Append(Out);

	Out.pos   =  mul(W, mul(V2,double4(v2.x, v2.y,  0.0, 1.0)));
	Out.pos   =  mul(V, Out.pos);
	Out.pos   =  mul(P, Out.pos);
	Out.uv = float2(UVs.z,UVs.y); // 1
	Out.color = Color1;
	TriStream.Append(Out);

	Out.pos   =  mul(W, mul(V2,double4(v1.x, v2.y, 0.0,1.0)));
	Out.pos   =  mul(V, Out.pos);
	Out.pos   =  mul(P, Out.pos);
	Out.uv = float2(UVs.x,UVs.y); // 2
	Out.color = Color1;
	TriStream.Append(Out);

	TriStream.RestartStrip();
}
*/