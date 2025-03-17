/*
    Patrik Uher
    xuherp02
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#define COLOR_CNT 8
#define TOOL_CNT 4

namespace utils {

    enum colorIndexes
    {
        colorPrimary = 0,
        colorSecondary,
        color1,
        color2,
        color3,
        color4,
        color5,
        color6,
        colorTemp
    };

    enum toolIndexes
    {
        penTog = 0,
        eraserTog,
        clearButn,
        polygonTog,
        testTog,
        rectangleType,
        elipseType,
        lineType
    };

    struct PixelCords 
    {
        double x;
        double y;
    };

}
#endif //UTILS_HPP
