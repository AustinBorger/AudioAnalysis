struct VSOut {
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

cbuffer Transform : register(b0) {
	float4 buf[128];
}

float4 main(VSOut input) : SV_Target {
	float2 uv = input.uv;
	uint pos = uv.x * 512;
	float element = sqrt(buf[pos >> 2][pos & 3]);

	if (1.0 - uv.y > log10(1.0f + element * 10000) / 10.0f) {
		return float4(uv.y, 1.0, 0.0, 1.0);
	}

	return float4(1.0, 0.0, 1.0 - uv.y, 1.0);
}