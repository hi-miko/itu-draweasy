/*
    Patrik Uher
    xuherp02
*/

#ifndef DRAW_CONTROLLER_HPP
#define DRAW_CONTROLLER_HPP

#include <cstdint>
#include <string>
#include <gtkmm.h>
#include "draw-view.hpp"
#include "draw-model.hpp"

class DrawController
{
    public:
    DrawView *cview;
    DrawModel *cmodel;

    private:
    struct rgb_color
    {
        float r;
        float g;
        float b;
    };

    std::vector<Gtk::ToggleButton *> typeTogs;

    std::vector<Glib::RefPtr<Gtk::GestureClick>> clickCtrls;

    Glib::RefPtr<Gtk::GestureClick> onTestClick;

    Glib::RefPtr<Gtk::GestureClick> onToolClick;
    Glib::RefPtr<Gtk::GestureClick> onToolRelease;
    Glib::RefPtr<Gtk::GestureClick> onDrawArrowClick;

    Glib::RefPtr<Gtk::GestureDrag> onDrawLeftDrag;
    Glib::RefPtr<Gtk::GestureDrag> onDrawRightDrag;
    Glib::RefPtr<Gtk::GestureDrag> onDrawMiddleDrag;

    Glib::RefPtr<Gtk::GestureDrag> onDrawLeftBegin;
    Glib::RefPtr<Gtk::GestureDrag> onDrawRightBegin;
    Glib::RefPtr<Gtk::GestureDrag> onDrawMiddleBegin;

    Glib::RefPtr<Gtk::GestureDrag> onDrawLeftEnd;
    Glib::RefPtr<Gtk::GestureDrag> onDrawRightEnd;
    Glib::RefPtr<Gtk::GestureDrag> onDrawMiddleEnd;

    public:
    DrawController(DrawView *, DrawModel *);
    void handle_tog_buttons(uint32_t);
    void handle_tog_subtype_buttons(uint32_t);
    void handle_color_buttons(const Cairo::RefPtr<Cairo::Context>&, int, int, uint32_t);
    void color_buttons_clicked(int n_press, double x, double y, uint32_t);
    void on_draw_right_click(int, double, double);
    void on_draw_left_click(int, double, double);

    void on_draw_drag_begin(double, double, char);
    void on_draw_drag_update(double, double, char);
    void on_draw_drag_end(double, double, char);

    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);

    void on_draw_arrow_click(int, double, double);
    void on_draw_arrow(const Cairo::RefPtr<Cairo::Context>&, int, int);

    void test_menu(int, double, double);

    void export_menu(char);
    void import_menu(void);
    void color_menu(void);
    void on_rgba_chosen(const Glib::RefPtr<Gio::AsyncResult>&);
    void on_file_chosen(const Glib::RefPtr<Gio::AsyncResult>&);

    private:
    void connect_signals(void);
    void connect_popover_signals(void);
    void setup_gestures(void);
    void handle_penwidth(Gtk::SpinButton *);
    void handle_pen(void);
    void handle_polygon(void);
    void handle_eraser(void);
    // rename this to clear screen
    void handle_test(void);

    rgb_color hex2rgb(std::string);
    std::string rgb2hex(double, double, double);

    void draw_state(const Cairo::RefPtr<Cairo::Context>& cr);
    // widgets that need a default state
    void init_function();

};

#endif // DRAW_CONTROLLER_HPP
