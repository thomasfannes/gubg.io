#include "catch.hpp"
#include "gubg/gnuplot/Stream.hpp"
#include <cmath>

TEST_CASE("gnuplot tests", "[ut][gnuplot]")
{
    gubg::gnuplot::Stream gs;

    gs.name(0, "sinus").name(1, "cosinus");
    for (double t = -3.0; t <= 3.0; t += 0.01)
    {
        gs.data() << t << std::sin(t);
        gs.data(1) << t << std::cos(t);
    }

    gs.pause();
    gs.name(42, "sinc");
    for (double t = -10.0; t <= 10.0; t += 0.1)
        gs.data(42) << t << std::round(std::sin(t)) << std::sin(std::round(t));

    gs.save("test.gnuplot");
}
