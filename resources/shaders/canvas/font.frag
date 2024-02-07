#version 450 core

layout (location = 0) in vec4 Color;
layout (location = 1) in vec2 TexelCoord;
layout (location = 2) in centroid vec2 UV;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D Tex;

void main() {
    ivec2 iTexCoord = ivec2(TexelCoord);

    float col = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            col += texelFetch(Tex, ivec2(iTexCoord.x + i, iTexCoord.y + k), 0).r;
        }
    }

    col /= 9.;

    FragColor = col * Color;
}