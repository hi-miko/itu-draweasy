/*
    Patrik Uher
    xuherp02
*/

#include <gtkmm.h>
#include "draw-view.hpp"
#include "draw-controller.hpp"

int main(int argc, char **argv)
{
    auto app = Gtk::Application::create("org.itu.draweasy");

    DrawView view = DrawView(app);
    DrawModel model = DrawModel(view.drawWin->get_width(), view.drawWin->get_height());
    DrawController contr = DrawController(&view, &model);

    app->signal_startup().connect([&view] () {view.init_widgets();});

    return app->run(argc, argv);
}
