/*
   Patrik Uher
   xuherp02
*/

#include "draw-model.hpp"

using dm = DrawModel;

dm::DrawModel(uint32_t width, uint32_t height)
{
    dm::cs = Cairo::ImageSurface::create(Cairo::ImageSurface::Format::ARGB32, width, height);
    dm::csp = Cairo::SurfacePattern::create(dm::cs);
}
