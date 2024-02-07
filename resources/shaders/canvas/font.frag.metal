#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct frag_out
{
    float4 FragColor [[color(0)]];
};

struct frag_in
{
    float4 Color [[user(locn0)]];
    float2 TexelCoord [[user(locn1)]];
};

fragment frag_out frag(frag_in in [[stage_in]], texture2d<float> Tex [[texture(0)]], sampler TexSmplr [[sampler(0)]])
{
    frag_out out = {};
    int2 iTexCoord = int2(in.TexelCoord);
    float col = 0.0;
    for (int i = 0; i < 3; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            col += Tex.read(uint2(int2(iTexCoord.x + i, iTexCoord.y + k)), 0).x;
        }
    }
    col /= 9.0;
    out.FragColor = in.Color * col;
    return out;
}

