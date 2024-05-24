#pragma once

namespace division_engine::canvas
{
struct EdgeInsets
{
    float top = 0, bottom = 0, left = 0, right = 0;

    static EdgeInsets vertical(float value)
    {
        return EdgeInsets { .top = value, .bottom = value, .left = 0, .right = 0 };
    }

    static EdgeInsets horizontal(float value)
    {
        return EdgeInsets { .top = 0, .bottom = 0, .left = value, .right = value };
    }

    static EdgeInsets symmetric(float horizontal, float vertical)
    {
        return EdgeInsets {
            .top = vertical,
            .bottom = vertical,
            .left = horizontal,
            .right = horizontal,
        };
    }

    static EdgeInsets all(float value)
    {
        return EdgeInsets { value, value, value, value };
    }
};
}