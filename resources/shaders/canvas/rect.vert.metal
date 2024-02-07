#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct Uniforms
{
    float2 screenSize;
};

struct vert_out
{
    float4 outColor [[user(locn0)]];
    float4 out_TRBRTLBL_BorderRadius [[user(locn1)]];
    float2 outUV [[user(locn2)]];
    float2 outPosition [[user(locn3)]];
    float2 outSize [[user(locn4)]];
    float2 outVertPos [[user(locn5)]];
    float4 gl_Position [[position]];
};

struct vert_in
{
    float2 vertPos [[attribute(0)]];
    float2 inUV [[attribute(1)]];
    float2 inSize [[attribute(2)]];
    float2 inPosition [[attribute(3)]];
    float4 inColor [[attribute(4)]];
    float4 in_TRBRTLBL_BorderRadius [[attribute(5)]];
};

vertex vert_out vert(vert_in in [[stage_in]], constant Uniforms& _23 [[buffer(1)]])
{
    vert_out out = {};
    float2 vertWorldPos = (in.vertPos * in.inSize) + in.inPosition;
    float2 normPos = vertWorldPos / _23.screenSize;
    out.outColor = in.inColor;
    out.out_TRBRTLBL_BorderRadius = in.in_TRBRTLBL_BorderRadius;
    out.outUV = in.inUV;
    out.outPosition = in.inPosition;
    out.outSize = in.inSize;
    out.outVertPos = vertWorldPos;
    out.gl_Position = float4(mix(float2(-1.0), float2(1.0), normPos), 0.0, 1.0);
    return out;
}

