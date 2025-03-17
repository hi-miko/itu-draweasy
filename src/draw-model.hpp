/*
    Patrik Uher
    xuherp02
*/

#ifndef DRAW_MODEL_HPP
#define DRAW_MODEL_HPP

#include <gtkmm.h>

#include <cairomm/cairomm.h>
#include "cairomm/pattern.h"
#include "cairomm/surface.h"
#include "glibmm/refptr.h"
#include "utils.hpp"

class DrawModel
{
    public:
    // vector that holds values of the colors
    std::vector<std::string> colorNames =
    {
        // primary color
        "000000",
        // secondary color
        "f6f5f4",
        // default six colors
        "181a1f",      // black
        "f6f5f4",      // bg white
        "e86671",      // red
        "98c379",      // green
        "61afef",      // blue
        "e5c07b",      // yellow
        // color temp
        "f6f5f4",      // bg white
    };
    
    struct HistoryT
    {
        uint32_t toolUsed;
        std::string color;
        uint32_t width;
        utils::PixelCords pixBegin;
        utils::PixelCords pixEnd;
    };

    std::vector<HistoryT> drawHistory;
    uint32_t history_index = 0;

    int xOffset = 0;
    int yOffset = 0;
    int lastXOffset = 0;
    int lastYOffset = 0;
    
    uint32_t penWidth = 8;

    uint32_t polygonTypeSel = utils::rectangleType;

    uint32_t selectedCol = utils::colorPrimary;
    uint32_t activeCol = utils::colorPrimary;

    uint32_t selectedReserveColor = utils::color1;

    uint32_t overrideCol = utils::colorTemp;
    bool overrideSet = false;

    // sets the first (default) element as the pen
    uint32_t activeTool = utils::penTog;

    utils::PixelCords currentPixel;
    utils::PixelCords dragPixelStart;
    utils::PixelCords dragPixelPrev;

    bool cr_changed = false;
    // default is to true, so that the screen gets wiped at the beginning
    // is resize is refreshed
    bool refresh_scr = true;

    std::string filePath;
    bool save_file = false;

    // TODO maybe remove this cause right now it seems poitless
    Glib::RefPtr<Cairo::Surface> cs;
    Glib::RefPtr<Cairo::SurfacePattern> csp;

    public:
    // the model communicates with the view through signals
    // the question being, can I get the vectors without view
    // maybe pass all important widgets to here via vectors
    DrawModel(uint32_t width, uint32_t height);
};

#endif // DRAW_MODEL_HPP
