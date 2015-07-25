struct VSOut {
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

cbuffer Transform : register(b0) {
	float4 buf[128];
}

float4 main(VSOut input) : SV_Target{
	float2 uv = input.uv; //uv coordinates of the pixel
	uint uPos = 512 - (log2(1.0f + (1.0 - uv.x) * 512) / log2(512) * 512);

	float element = buf[uPos >> 2][uPos & 3]; //grabs the power of the bin, which needs to be unpacked from float4

	float db = log10(1.0f + element * 10000) / 10.0f; //convert to a cb scale
	float z = 0.001; //Choose how smoothly to interpolate
	float interp = smoothstep(db, db + z, 1.0 - uv.y); //interpolate between bin pixel and background pixel

	float3 background = float3(uv.y, 1.0, 0.0); //The background color
	float3 bin = float3(0.25 + uv.x * 0.5, 0.0, 1.0 - uv.y); //The color of the bin pixel
	float3 color = interp * background + (1.0 - interp) * bin; //The interpolated color

	return float4(color, 1.0);
}