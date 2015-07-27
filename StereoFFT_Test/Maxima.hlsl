/*
** Copyright (C) 2015 Austin Borger <aaborger@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
** API documentation is available here:
**		https://github.com/AustinBorger/StereoFFT
*/

struct VSOut {
	float4 position : SV_Position;
	float2 uv : TexCoord;
};

cbuffer Transform : register(b0) {
	float4 buf[128];
}

float4 main(VSOut input) : SV_Target {
	float2 uv = input.uv; //uv coordinates of the pixel
	
	uint uPos = pow(2, uv.x * log2(512)) - 1; //bin pos in array

	float element = buf[uPos >> 2][uPos & 3]; //grabs the power of the bin, which needs to be unpacked from float4

	if (uPos > 0 && uPos < 512) {
		float element_left = buf[(uPos - 1) >> 2][(uPos - 1) & 3];
		float element_right = buf[(uPos + 1) >> 2][(uPos + 1) & 3];

		if (element_left > element || element_right > element) {
			element = 0;
		}
	} else if (uPos == 0) {
		float element_right = buf[(uPos + 1) >> 2][(uPos + 1) & 3];

		if (element_right > element) {
			element = 0;
		}
	} else if (uPos == 512) {
		float element_left = buf[(uPos - 1) >> 2][(uPos - 1) & 3];

		if (element_left > element) {
			element = 0;
		}
	}

	float db = 10.0f * log10(1.0f + element) / 50.0f; //convert to a db scale
	float z = 0.001; //Choose how smoothly to interpolate
	float interp = smoothstep(db, db + z, 1.0 - uv.y); //interpolate between bin pixel and background pixel

	float3 background = float3(uv.y, 1.0, 0.0); //The background color
	float3 bin = float3(0.25 + uv.x * 0.5, 0.0, 1.0 - uv.y); //The color of the bin pixel
	float3 color = interp * background + (1.0 - interp) * bin; //The interpolated color

	return float4(color, 1.0);
}