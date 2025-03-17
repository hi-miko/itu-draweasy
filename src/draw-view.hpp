/*
    Patrik Uher
    xuherp02
*/

#ifndef DRAW_VIEW_HPP
#define DRAW_VIEW_HPP

#include <gtkmm.h>
#include <vector>
#include "glibmm/refptr.h"
#include "draw-model.hpp"
#include "gtkmm/popover.h"

class DrawView //: Gtk::Window
{
    public:
    Glib::RefPtr<Gtk::Builder> uiData; //all the data in the ui file
    Glib::RefPtr<Gtk::Application> app;

    DrawModel *cmodel;

    // color indexes are indexed by their number
    std::vector<std::string> colorNames =
    {
        "colorPrimary",
        "colorSecondary",
        "color1",
        "color2",
        "color3",
        "color4",
        "color5",
        "color6"
    };

    std::vector<Gtk::DrawingArea *> colorVector;
    
    std::vector<std::pair<std::string, std::string>> toolNames =
    {
        {"penTog", "penTogMenu"},
        {"eraserTog", "eraserTogMenu"},
        {"clearButn", ""},
        {"polygonTog", "polygonTogMenu"}
    };

    std::vector<std::string> toolIcons =
    {
        "../resources/icons/pen.png",
        "../resources/icons/eraser.png",
        "../resources/icons/trash.png",
        "../resources/icons/polygon.png"
    };

    std::vector<std::pair<Gtk::ToggleButton *, Gtk::Popover *>> toolVector;

    Gtk::Window *mainWin;
    Glib::RefPtr<Gtk::ColorDialog> colorDialog;
    Glib::RefPtr<Gtk::FileDialog> fileDialog;
    Gtk::DrawingArea *drawWin;

    Gtk::DrawingArea *colorSwitch;
    Gtk::Button *colorSelectMenu;

    public:
    DrawView(Glib::RefPtr<Gtk::Application>);
    void init_widgets();

    private:
    void read_file(void);
    void setup_win(void);
    void setup_buttons(void);
    void setup_colors(void);
    void setup_icons(void);
    void setup_modal_windows(void);
};

#endif // DRAW_VIEW_HPP
