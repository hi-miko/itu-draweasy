/*
    Patrik Uher
    xuherp02
*/

#include <iostream>
#include "draw-view.hpp"
#include "gdkmm/pixbuf.h"
#include "gtkmm/drawingarea.h"
#include "gtkmm/enums.h"
#include "gtkmm/filedialog.h"
#include "gtkmm/image.h"
#include "gtkmm/popover.h"

using dv = DrawView;

dv::DrawView(Glib::RefPtr<Gtk::Application> application) : 
    app(application)
{
    dv::read_file();
}

void dv::read_file()
{
    // connects the activate signal to our ui builder function
    dv::uiData = Gtk::Builder::create();
    try
    {
        uiData->add_from_file("../ui/main.ui");
    }
    catch(const Glib::FileError& err)
    {
        std::cerr << "FileError: " << err.what() << std::endl;
        exit(1);
    }
    catch(const Glib::MarkupError& err)
    {
        std::cerr << "MarkupError: " << err.what() << std::endl;
        exit(1);
    }
    catch(const Gtk::BuilderError& err)
    {
        std::cerr << "BuilderError: " << err.what() << std::endl;
        exit(1);
    }

    dv::drawWin = dv::uiData->get_widget<Gtk::DrawingArea>("drawWin");
    dv::colorSwitch = dv::uiData->get_widget<Gtk::DrawingArea>("colorSwitchArrow");

    dv::setup_win();
    dv::setup_buttons();
    dv::setup_modal_windows();
    dv::setup_colors();
}

void dv::setup_modal_windows()
{
    // color picker dialog setup
    dv::colorDialog = Gtk::ColorDialog::create();
    if(dv::colorDialog == nullptr)
    {
        std::cerr << "Error: Failed to setup color dialog" << std::endl;
        exit(1);
    }

    dv::colorDialog->set_with_alpha(false);
    dv::colorDialog->set_modal(true);

    // file picker dialog setup
    dv::fileDialog = Gtk::FileDialog::create();
    if(dv::fileDialog == nullptr)
    {
        std::cerr << "Error: Failed to setup file dialog" << std::endl;
        exit(1);
    }

    dv::fileDialog->set_modal(true);
}

void dv::setup_win()
{
    dv::mainWin = dv::uiData->get_widget<Gtk::Window>("mainWin");

    if (dv::mainWin == nullptr)
    {
        std::cerr << "Error: Could not get the main window" << std::endl;
        exit(1);
    }

    mainWin->set_visible(true);
    mainWin->set_title("DrawEasy");
}

void dv::setup_buttons()
{
    Gtk::ToggleButton *tool = nullptr;

    for(int i = 0; i < TOOL_CNT; i++)
    {
        // sets up the toggle button
        tool = dv::uiData->get_widget<Gtk::ToggleButton>(dv::toolNames[i].first);
        if (tool == nullptr)
        {
            std::cerr << "Error: Could not get the tool toggle button" << std::endl;
            exit(1);
        }

        std::pair<Gtk::ToggleButton *, Gtk::Popover *> temp;

        auto pixbuf = Gdk::Pixbuf::create_from_file(dv::toolIcons[i], 28, 28, true);
        auto img = Gtk::Image(pixbuf);

        tool->set_child(img);

        temp.first = tool;

        // sets up the popover menu if there is one to setup
        if(dv::toolNames[i].second == "")
        {
            temp.second = nullptr;
            // copies the temporary variable data to the vector
            dv::toolVector.push_back(temp);
        }
        else
        {
            auto popMenu = dv::uiData->get_widget<Gtk::Popover>(toolNames[i].second);
            if(popMenu == nullptr)
            {
                std::cerr << "Error: Could not create popover menu" << std::endl;
                exit(1);
            }

            temp.second = popMenu;
            // copies the temporary variable data to the vector
            dv::toolVector.push_back(temp);

            popMenu->set_parent(*dv::toolVector[i].first);
            auto allocation = dv::toolVector[i].first->get_allocation();

            Gdk::Rectangle rect(allocation.get_x(), allocation.get_y(), 55, 50);

            popMenu->set_pointing_to(rect);
            popMenu->set_position(Gtk::PositionType::RIGHT);
            popMenu->set_autohide(true);

        }
    }

    dv::colorSelectMenu = dv::uiData->get_widget<Gtk::Button>("colorSelectMenu");
    if(dv::colorSelectMenu == nullptr)
    {
        std::cerr << "Error: Could not create color select button" << std::endl;
        exit(1);
    }

    auto pixbuf = Gdk::Pixbuf::create_from_file("../resources/icons/palette.png", 28, 28, true);
    auto img = Gtk::Image(pixbuf);

    dv::colorSelectMenu->set_child(img);
}

void dv::setup_colors()
{
    Gtk::DrawingArea *color = nullptr;

    for(int i = 0; i < COLOR_CNT; i++)
    {
        color = dv::uiData->get_widget<Gtk::DrawingArea>(dv::colorNames[i]);

        if (color == nullptr)
        {
            std::cerr << "Error: Could not get the color button" << std::endl;
            exit(1);
        }

        dv::colorVector.push_back(color);
    }
}

void dv::init_widgets()
{
    dv::app->add_window(*(dv::mainWin));
}
