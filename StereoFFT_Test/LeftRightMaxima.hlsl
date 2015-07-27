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
	float4 buf[128 * 2];
}

cbuffer Time : register(b1) {
	float time;
}

float4 main(VSOut input) : SV_Target {
	float2 uv = input.uv; //uv coordinates of the pixel

	uint leftPos = pow(2, (1.0 - uv.y) * log2(512)) - 1;
	uint rightPos = leftPos + 512;

	float leftElement = buf[leftPos >> 2][leftPos & 3]; //grabs the power of the bin, which needs to be unpacked from float4
	float rightElement = buf[rightPos >> 2][rightPos & 3]; //does the same for the right channel

	if (leftPos > 0 && leftPos < 512) {
		float element_left = buf[(leftPos - 1) >> 2][(leftPos - 1) & 3];
		float element_right = buf[(leftPos + 1) >> 2][(leftPos + 1) & 3];

		if (element_left > leftElement || element_right > leftElement) {
			leftElement = 0;
		}
	} else if (leftPos == 0) {
		float element_right = buf[(leftPos + 1) >> 2][(leftPos + 1) & 3];

		if (element_right > leftElement) {
			leftElement = 0;
		}
	} else if (leftPos == 512) {
		float element_left = buf[(leftPos - 1) >> 2][(leftPos - 1) & 3];

		if (element_left > leftElement) {
			leftElement = 0;
		}
	}

	if (rightPos > 0 && rightPos < 512 + 512) {
		float element_left = buf[(rightPos - 1) >> 2][(rightPos - 1) & 3];
		float element_right = buf[(rightPos + 1) >> 2][(rightPos + 1) & 3];

		if (element_left > rightElement || element_right > rightElement) {
			rightElement = 0;
		}
	} else if (rightPos == 512 + 0) {
		float element_right = buf[(rightPos + 1) >> 2][(rightPos + 1) & 3];

		if (element_right > rightElement) {
			rightElement = 0;
		}
	} else if (rightPos == 512 + 512) {
		float element_left = buf[(rightPos - 1) >> 2][(rightPos - 1) & 3];

		if (element_left > rightElement) {
			rightElement = 0;
		}
	}

	float leftDB = 10.0f * log10(1.0f + leftElement) / 100.0f; //convert to a db scale
	float rightDB = 10.0f * log10(1.0f + rightElement) / 100.0f; //same for right

	float z = 0.001; //Choose how smoothly to interpolate

	float leftInterp = smoothstep(leftDB, leftDB + z, uv.x); //interpolate between bin pixel and background pixel
	float rightInterp = smoothstep(rightDB, rightDB + z, 1.0 - uv.x); //same for right

	float3 background = float3(uv.y, 1.0, 0.0); //The background color
	float3 leftBin = float3(0.75 - uv.y * 0.5 * (1 + sin(time * 128 / 60 * 2.0 * 3.14159)) / 2.0, 0.0, uv.x * 2.0); //The color of the bin pixel
	float3 rightBin = float3(0.75 - uv.y * 0.5 * (1 + sin(time * 128 / 60 * 2.0 * 3.14159)) / 2.0, 0.0, (1.0 - uv.x) * 2.0); //same for right

	float3 color = (leftInterp * rightInterp) * background +
				   (1.0 - leftInterp) * leftBin +
				   (1.0 - rightInterp) * rightBin;

	return float4(color, 1.0);
}