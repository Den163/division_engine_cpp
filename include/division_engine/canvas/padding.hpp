#pragma once

namespace division_engine::canvas
{
struct Padding
{
    float top = 0, bottom = 0, left = 0, right = 0;

    static Padding vertical(float value)
    {
        return Padding { .top = value, .bottom = value, .left = 0, .right = 0 };
    }

    static Padding horizontal(float value)
    {
        return Padding { .top = 0, .bottom = 0, .left = value, .right = value };
    }

    static Padding symmetric(float horizontal, float vertical)
    {
        return Padding {
            .top = vertical,
            .bottom = vertical,
            .left = horizontal,
            .right = horizontal,
        };
    }

    static Padding all(float value)
    {
        return Padding { value, value, value, value };
    }
};
}