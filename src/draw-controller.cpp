/*
    Patrik Uher
    xuherp02
*/

#include <cstdint>
#include <iostream>
#include <ostream>
#include "draw-controller.hpp"

using dc = DrawController;

dc::DrawController(DrawView *view, DrawModel *model) :
    cview(view), cmodel(model)
{
    dc::setup_gestures();
    dc::connect_signals();
}

void dc::setup_gestures()
{
    Glib::RefPtr<Gtk::GestureClick> tempGes;

    for(int i = 0; i < COLOR_CNT; i++)
    {
        tempGes = Gtk::GestureClick::create();
        if(tempGes == nullptr)
        {
            std::cerr << "Error: creating gesture clicks" << std::endl;
            exit(1);
        }
        
        clickCtrls.push_back(tempGes);

        tempGes->signal_pressed().connect(
            sigc::bind(
                sigc::mem_fun(*this, &dc::color_buttons_clicked), i
            )
        );
    }

    // Clicks
    dc::onDrawArrowClick = Gtk::GestureClick::create();
    dc::onDrawArrowClick->set_button(1);
    dc::onDrawArrowClick->signal_pressed().connect(
        sigc::mem_fun(*this, &dc::on_draw_arrow_click)
    );
    
    // Drag Begin
    dc::onDrawRightBegin = Gtk::GestureDrag::create();
    dc::onDrawRightBegin->set_button(3);
    dc::onDrawRightBegin->signal_drag_begin().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_begin), 'r'
        )
    );

    dc::onDrawLeftBegin = Gtk::GestureDrag::create();
    dc::onDrawLeftBegin->set_button(1);
    dc::onDrawLeftBegin->signal_drag_begin().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_begin), 'l'
        )
    );

    dc::onDrawMiddleBegin = Gtk::GestureDrag::create();
    dc::onDrawMiddleBegin->set_button(2);
    dc::onDrawMiddleBegin->signal_drag_begin().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_begin), 'm'
        )
    );

    // // Drag Update
    dc::onDrawRightDrag = Gtk::GestureDrag::create();
    dc::onDrawRightDrag->set_button(3);
    dc::onDrawRightDrag->signal_drag_update().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_update), 'r'
        )
    );

    dc::onDrawLeftDrag = Gtk::GestureDrag::create();
    dc::onDrawLeftDrag->set_button(1);
    dc::onDrawLeftDrag->signal_drag_update().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_update), 'l'
        )
    );

    dc::onDrawMiddleDrag = Gtk::GestureDrag::create();
    dc::onDrawMiddleDrag->set_button(2);
    dc::onDrawMiddleDrag->signal_drag_update().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_update), 'm'
        )
    );

    // // Drag End
    dc::onDrawRightEnd = Gtk::GestureDrag::create();
    dc::onDrawRightEnd->set_button(3);
    dc::onDrawRightEnd->signal_drag_end().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_end), 'r'
        )
    );

    dc::onDrawLeftEnd = Gtk::GestureDrag::create();
    dc::onDrawLeftEnd->set_button(1);
    dc::onDrawLeftEnd->signal_drag_end().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_end), 'l'
        )
    );

    dc::onDrawMiddleEnd = Gtk::GestureDrag::create();
    dc::onDrawMiddleEnd->set_button(2);
    dc::onDrawMiddleEnd->signal_drag_end().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::on_draw_drag_end), 'm'
        )
    );

    // Clicks
    dc::cview->colorSwitch->add_controller(dc::onDrawArrowClick);

    // Drag Begin
    dc::cview->drawWin->add_controller(dc::onDrawLeftBegin);
    dc::cview->drawWin->add_controller(dc::onDrawRightBegin);
    dc::cview->drawWin->add_controller(dc::onDrawMiddleBegin);

    // // Drag Update
    dc::cview->drawWin->add_controller(dc::onDrawLeftDrag);
    dc::cview->drawWin->add_controller(dc::onDrawRightDrag);
    dc::cview->drawWin->add_controller(dc::onDrawMiddleDrag);
    //
    // // Drag End
    dc::cview->drawWin->add_controller(dc::onDrawLeftEnd);
    dc::cview->drawWin->add_controller(dc::onDrawRightEnd);
    dc::cview->drawWin->add_controller(dc::onDrawMiddleEnd);
}

void dc::connect_signals()
{
    dc::cview->drawWin->set_draw_func(sigc::mem_fun(*this, &dc::on_draw));

    dc::cview->colorSwitch->set_draw_func(sigc::mem_fun(*this, &dc::on_draw_arrow));

    // buttons connect
    for(int i = 0; i < TOOL_CNT; i++)
    {
        dc::cview->toolVector[i].first->signal_clicked().connect(
            sigc::bind(
                sigc::mem_fun(*this, &dc::handle_tog_buttons), i)
            );
    }

    dc::connect_popover_signals();

    for(int i = 0; i < COLOR_CNT; i++)
    {
        dc::cview->colorVector[i]->set_draw_func(
            sigc::bind(
                sigc::mem_fun(*this, &dc::handle_color_buttons), i)
            );
        dc::cview->colorVector[i]->add_controller(dc::clickCtrls[i]);
    }

    dc::cview->colorSelectMenu->signal_clicked().connect(
        sigc::mem_fun(*this, &dc::color_menu)
    );

    dc::cview->app->add_action("importAct", sigc::mem_fun(*this, &dc::import_menu));
    dc::cview->app->add_action("exportAllAct",
        sigc::bind(
            sigc::mem_fun(*this, &dc::export_menu), 'a')
    );

    dc::cview->app->add_action("exportVisibleAct", 
        sigc::bind(
            sigc::mem_fun(*this, &dc::export_menu), 'v')
    );

    dc::init_function();
}

void dc::connect_popover_signals()
{
    // pen popover handle
    auto tempSpin = dynamic_cast<Gtk::SpinButton *>(dc::cview->toolVector[utils::penTog].second->get_child()->get_first_child());

    if(tempSpin == nullptr)
    {
        std::cerr << "Error: Could not find requested spin button" << std::endl;
        exit(1);
    }

    tempSpin->signal_value_changed().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::handle_penwidth), tempSpin)
    );
    
    // eraser popever handle
    tempSpin = dynamic_cast<Gtk::SpinButton *>(dc::cview->toolVector[utils::eraserTog].second->get_child()->get_first_child());

    if(tempSpin == nullptr)
    {
        std::cerr << "Error: Could not find requested spin button" << std::endl;
        exit(1);
    }

    tempSpin->signal_value_changed().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::handle_penwidth), tempSpin)
    );

    // handle polygon
    tempSpin = dynamic_cast<Gtk::SpinButton *>(dc::cview->toolVector[utils::polygonTog].second->get_child()->get_first_child()->get_first_child());
    if(tempSpin == nullptr)
    {
        std::cerr << "Error: Could not find requested spin button" << std::endl;
        exit(1);
    }

    tempSpin->signal_value_changed().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::handle_penwidth), tempSpin)
    );

    auto tempTog = dynamic_cast<Gtk::ToggleButton *>(dc::cview->toolVector[utils::polygonTog].second->get_child()->get_first_child()->get_first_child()->get_next_sibling()->get_first_child());

    if(tempTog == nullptr)
    {
        std::cerr << "Error: Could not find requested toggle button" << std::endl;
        exit(1);
    }

    dc::typeTogs.push_back(tempTog);

    auto pixbuf = Gdk::Pixbuf::create_from_file("../resources/icons/square.png", 28, 28, true);
    auto img = Gtk::Image(pixbuf);

    tempTog->set_child(img);

    tempTog->signal_clicked().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::handle_tog_subtype_buttons), utils::rectangleType)
    );

    tempTog = dynamic_cast<Gtk::ToggleButton *>(dc::cview->toolVector[utils::polygonTog].second->get_child()->get_first_child()->get_first_child()->get_next_sibling()->get_first_child()->get_next_sibling());

    if(tempTog == nullptr)
    {
        std::cerr << "Error: Could not find requested toggle button" << std::endl;
        exit(1);
    }

    dc::typeTogs.push_back(tempTog);

    pixbuf = Gdk::Pixbuf::create_from_file("../resources/icons/circle.png", 28, 28, true);
    img = Gtk::Image(pixbuf);

    tempTog->set_child(img);

    tempTog->signal_clicked().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::handle_tog_subtype_buttons), utils::elipseType)
    );

    tempTog = dynamic_cast<Gtk::ToggleButton *>(dc::cview->toolVector[utils::polygonTog].second->get_child()->get_first_child()->get_first_child()->get_next_sibling()->get_next_sibling());

    if(tempTog == nullptr)
    {
        std::cerr << "Error: Could not find requested toggle button" << std::endl;
        exit(1);
    }

    dc::typeTogs.push_back(tempTog);

    pixbuf = Gdk::Pixbuf::create_from_file("../resources/icons/line.png", 28, 28, true);
    img = Gtk::Image(pixbuf);

    tempTog->set_child(img);

    tempTog->signal_clicked().connect(
        sigc::bind(
            sigc::mem_fun(*this, &dc::handle_tog_subtype_buttons), utils::lineType)
    );
}

void dc::init_function()
{
    dc::cview->toolVector[dc::cmodel->activeTool].first->set_active(true);
    // initializes the model with first values
    handle_tog_buttons(dc::cmodel->activeTool);
}

void dc::handle_penwidth(Gtk::SpinButton *spinButton)
{
    dc::cmodel->penWidth = spinButton->get_value();
}

void dc::color_menu()
{
    // show window and wait for response (because window is modal)
    dc::cview->colorDialog->choose_rgba(*dc::cview->mainWin, sigc::mem_fun(*this, &dc::on_rgba_chosen));
}

void dc::on_rgba_chosen(const Glib::RefPtr<Gio::AsyncResult>& result)
{
    try
    {
        auto color = dc::cview->colorDialog->choose_rgba_finish(result);
        
        dc::cmodel->colorNames[dc::cmodel->selectedCol] = dc::rgb2hex(color.get_red(), color.get_green(), color.get_blue());
        dc::cmodel->colorNames[dc::cmodel->selectedReserveColor] = dc::rgb2hex(color.get_red(), color.get_green(), color.get_blue());

        dc::cview->colorVector[dc::cmodel->selectedCol]->queue_draw();
        dc::cview->colorVector[dc::cmodel->selectedReserveColor]->queue_draw();
    }
    catch (const Gtk::DialogError& err)
    {
        std::cout << "No color selected. " << err.what() << std::endl;
    }
    catch (const Glib::Error& err)
    {
        std::cout << "Unexpected exception. " << err.what() << std::endl;
    }
}

void dc::on_file_chosen(const Glib::RefPtr<Gio::AsyncResult>& result)
{

}

void dc::export_menu(char submenu)
{
    if(submenu == 'a')
    {
        std::cout << "export all picked" << std::endl;
    }
    else
    {
        std::cout << "export current picked" << std::endl;
    }
}

void dc::import_menu()
{
    std::cout << "import file menu picked" << std::endl;
}

void dc::color_buttons_clicked(int n_press, double x, double y, uint32_t colorIdnx)
{
    switch(colorIdnx)
    {
        case utils::colorPrimary:
            cmodel->selectedCol = utils::colorPrimary;
            break;
        case utils::colorSecondary:
            cmodel->selectedCol = utils::colorSecondary;
            break;
        default:
            cmodel->colorNames[cmodel->selectedCol] = cmodel->colorNames[colorIdnx];
            cmodel->selectedReserveColor = colorIdnx;
            break;
    }

    // redraw all the colors when a new one is picked
    for(int i = 0; i < COLOR_CNT; i++)
    {
        dc::cview->colorVector[i]->queue_draw();
    }

}

void dc::handle_color_buttons(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height, uint32_t colorIndex)
{
    std::string hex_color = dc::cmodel->colorNames[colorIndex];
    std::string outline_color_hex = "3f8ae5";

    dc::rgb_color color = dc::hex2rgb(hex_color);
    dc::rgb_color outline_color = dc::hex2rgb(outline_color_hex);

    cr->set_source_rgb(color.r, color.g, color.b);
    cr->paint();

    // if the color is selected add a standard blue border
    if((colorIndex == dc::cmodel->selectedReserveColor) or (colorIndex == dc::cmodel->selectedCol))
    {
        cr->set_source_rgb(outline_color.r, outline_color.g, outline_color.b);
        cr->set_line_width(6);
        cr->rectangle(0, 0, width, height);
        cr->stroke();
    }
    // add a black border if color is close to white
    else if(color.r + color.b + color.g >= 2.6f)
    {
        // set border color to black
        cr->set_source_rgb(0,0,0);

        cr->set_line_width(2);
        cr->rectangle(0, 0, width, height);
        cr->stroke();
    }
}

void dc::handle_tog_subtype_buttons(uint32_t buttonIndx)
{
    // turns off every other toggle button except the one chosen
    for(auto togButton : dc::typeTogs)
    {
        togButton->set_active(false);
    }

    // user should not be allowed to turn off a button
    // -5 is subtype offset in utils types
    dc::typeTogs[buttonIndx-5]->set_active(true);

    switch(buttonIndx)
    {
        case utils::rectangleType:
            dc::cmodel->polygonTypeSel = utils::rectangleType;
            break;
        case utils::elipseType:
            dc::cmodel->polygonTypeSel = utils::elipseType;
            break;
        case utils::lineType:
            dc::cmodel->polygonTypeSel = utils::lineType;
            break;
    }
}

void dc::handle_tog_buttons(uint32_t buttonIndx)
{
    // loop turns off all toggle buttons other than the index button
    for(uint32_t i = 0; i < TOOL_CNT; i++)
    {
        dc::cview->toolVector[i].first->set_active(false);

        if(dc::cview->toolVector[i].second != nullptr)
        {
            dc::cview->toolVector[i].second->popdown();
        }
    }

    cview->toolVector[buttonIndx].first->set_active(true);

    // turn off color override when buttons change
    dc::cmodel->overrideSet = false;

    if(dc::cview->toolVector[buttonIndx].second != nullptr)
    {
        dc::cview->toolVector[buttonIndx].second->popup();
    }

    // turn on buttons submenu somewhere
    switch(buttonIndx)
    {
        case utils::penTog:
            dc::handle_pen();
            break;
        case utils::eraserTog:
            dc::handle_eraser();
            break;
        case utils::polygonTog:
            dc::handle_polygon();
            break;
        case utils::clearButn:
        {
            auto old_active = dc::cmodel->activeTool;
            dc::handle_test();
            cview->toolVector[buttonIndx].first->set_active(false);
            dc::handle_tog_buttons(old_active);
            break;
        }
    }
}

void dc::handle_polygon()
{
    dc::cmodel->activeTool = utils::polygonTog;
    dc::cmodel->penWidth = dynamic_cast<Gtk::SpinButton *>(dc::cview->toolVector[utils::polygonTog].second->get_child()->get_first_child()->get_first_child())->get_value();
}

void dc::handle_pen()
{
    dc::cmodel->activeTool = utils::penTog;
    dc::cmodel->penWidth = dynamic_cast<Gtk::SpinButton *>(dc::cview->toolVector[utils::penTog].second->get_child()->get_first_child())->get_value();
}

void dc::handle_eraser()
{
    dc::cmodel->activeTool = utils::eraserTog;
    dc::cmodel->overrideSet = true;
    dc::cmodel->penWidth = dynamic_cast<Gtk::SpinButton *>(dc::cview->toolVector[utils::eraserTog].second->get_child()->get_first_child())->get_value();
}

void dc::handle_test()
{
    dc::cmodel->refresh_scr = true;
    dc::cview->drawWin->queue_draw();
}

void dc::on_draw_arrow(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
    // draw a double arrow

    // set color to black
    cr->set_source_rgb(0, 0, 0);
    cr->set_line_width(3);

    cr->move_to(27, height-5);
    cr->line_to(20, height-8);

    cr->move_to(27, height-5);
    cr->line_to(34, height-8);

    cr->move_to(27, height-5);

    cr->curve_to((width/2.f)-30, 0, (width/2.f)+30, 0, width-27, height-5);

    cr->line_to(width-34, height-8);
    cr->move_to(width-27, height-5);
    cr->line_to(width-20, height-8);

    cr->stroke();
}

void dc::on_draw_arrow_click(int n_pressed, double x, double y)
{
    std::string tempColor = dc::cmodel->colorNames[utils::colorPrimary];

    dc::cmodel->colorNames[utils::colorPrimary] =  dc::cmodel->colorNames[utils::colorSecondary];
     dc::cmodel->colorNames[utils::colorSecondary] = tempColor;


    dc::cview->colorVector[utils::colorPrimary]->queue_draw();
    dc::cview->colorVector[utils::colorSecondary]->queue_draw();
}

void dc::on_draw_drag_begin(double x, double y, char mb)
{
    switch(mb)
    {
        case 'l':
            cmodel->activeCol = utils::colorPrimary;
            break;
        case 'r':
            cmodel->activeCol = utils::colorSecondary;
            break;
        case 'm':
            dc::cmodel->xOffset = 0;
            dc::cmodel->yOffset = 0;
            return;
        default:
            // ignore
            break;
    }

    switch(dc::cmodel->activeTool)
    {
        case utils::polygonTog:
        {
            DrawModel::HistoryT history_point;

            history_point.toolUsed = dc::cmodel->polygonTypeSel;
            history_point.color = dc::cmodel->colorNames[dc::cmodel->activeCol];
            history_point.width = dc::cmodel->penWidth;
            history_point.pixBegin = {x+(dc::cmodel->xOffset*(-1)), y+(dc::cmodel->yOffset*(-1))};
            history_point.pixEnd = {x+(dc::cmodel->xOffset*(-1)), y+(dc::cmodel->yOffset*(-1))};

            dc::cmodel->drawHistory.push_back(history_point);
        }
    }

    dc::cmodel->currentPixel.x = x;
    dc::cmodel->currentPixel.y = y;

    dc::cmodel->dragPixelStart.x = x;
    dc::cmodel->dragPixelStart.y = y;

    cview->drawWin->queue_draw();
}

void dc::on_draw_drag_update(double x_offset, double y_offset, char mb)
{
    // gets the old pixel coords, before currentPixel get updated
    dc::cmodel->dragPixelPrev.x = dc::cmodel->currentPixel.x;
    dc::cmodel->dragPixelPrev.y = dc::cmodel->currentPixel.y;

    switch(mb)
    {
        case 'l':
            dc::cmodel->activeCol = utils::colorPrimary;
            break;
        case 'r':
            dc::cmodel->activeCol = utils::colorSecondary;
            break;
        case 'm':
            dc::cmodel->xOffset = x_offset + dc::cmodel->lastXOffset;
            dc::cmodel->yOffset = y_offset + dc::cmodel->lastYOffset;
            cview->drawWin->queue_draw();
            return;
        default:
            // ignore
            break;
    }

    dc::cmodel->currentPixel.x = dc::cmodel->dragPixelStart.x + x_offset;
    dc::cmodel->currentPixel.y = dc::cmodel->dragPixelStart.y + y_offset;
    
    switch(dc::cmodel->activeTool)
    {
        case utils::penTog:
        case utils::eraserTog:
        {
            DrawModel::HistoryT history_point;

            history_point.toolUsed = dc::cmodel->activeTool;
            history_point.width = dc::cmodel->penWidth;
            history_point.pixBegin = {dc::cmodel->dragPixelPrev.x+dc::cmodel->xOffset*(-1), dc::cmodel->dragPixelPrev.y+dc::cmodel->yOffset*(-1)};
            history_point.pixEnd = {dc::cmodel->currentPixel.x+dc::cmodel->xOffset*(-1), dc::cmodel->currentPixel.y+dc::cmodel->yOffset*(-1)};

            if(dc::cmodel->overrideSet)
            {
                history_point.color = dc::cmodel->colorNames[dc::cmodel->overrideCol];
            }
            else
            {
                history_point.color = dc::cmodel->colorNames[dc::cmodel->activeCol];
            }

            dc::cmodel->drawHistory.push_back(history_point);
            dc::cmodel->history_index++;

            break;
        }
        case utils::polygonTog:
            dc::cmodel->drawHistory[dc::cmodel->history_index].pixEnd = { dc::cmodel->currentPixel.x+dc::cmodel->xOffset*(-1), dc::cmodel->currentPixel.y+dc::cmodel->yOffset*(-1)};
            break;
    }

    cview->drawWin->queue_draw();
}

void dc::on_draw_drag_end(double x_offset, double y_offset, char mb)
{
    switch(mb)
    {
        case 'l':
            dc::cmodel->activeCol = utils::colorPrimary;
            break;
        case 'r':
            dc::cmodel->activeCol = utils::colorSecondary;
            break;
        case 'm':
            dc::cmodel->xOffset = x_offset + dc::cmodel->lastXOffset;
            dc::cmodel->yOffset = y_offset + dc::cmodel->lastYOffset;

            dc::cmodel->lastXOffset += x_offset;
            dc::cmodel->lastYOffset += y_offset;

            cview->drawWin->queue_draw();
            return;
        default:
            // ignore
            break;
    }

    switch(dc::cmodel->activeTool)
    {
        case utils::polygonTog:
            dc::cmodel->history_index++;
            break;
    }

    cview->drawWin->queue_draw();
}

void dc::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
    if(dc::cmodel->refresh_scr)
    {
        // since 'cr' is empty at the beginning this basically refreshes the screen
        dc::rgb_color color;
        color = dc::hex2rgb(cmodel->colorNames[cmodel->overrideCol]);

        cr->set_source_rgb(color.r, color.g, color.b);
        cr->paint();

        dc::cmodel->drawHistory.clear();
        dc::cmodel->history_index = 0;

        dc::cmodel->refresh_scr = false;
        return;
    }
    
    // run a specialized drawing function
    dc::draw_state(cr);
}

void dc::draw_state(const Cairo::RefPtr<Cairo::Context>& cr)
{
    for( auto line : dc::cmodel->drawHistory)
    {
        dc::rgb_color color;
        color = dc::hex2rgb(line.color);
        cr->set_source_rgb(color.r, color.g, color.b);

        switch(line.toolUsed)
        {
            // fallthorough cause these tools have the same effect
            case utils::eraserTog:
            case utils::penTog:
                cr->set_line_cap(Cairo::Context::LineCap::ROUND);
                cr->set_line_width(line.width);
                cr->move_to(line.pixBegin.x+dc::cmodel->xOffset, line.pixBegin.y+dc::cmodel->yOffset);
                cr->line_to(line.pixEnd.x+dc::cmodel->xOffset, line.pixEnd.y+dc::cmodel->yOffset);
                cr->stroke();
                break;
            case utils::rectangleType:
                cr->set_line_width(line.width);
                cr->move_to(line.pixBegin.x+dc::cmodel->xOffset, line.pixBegin.y+dc::cmodel->yOffset);
                cr->rectangle(line.pixBegin.x+dc::cmodel->xOffset, line.pixBegin.y+dc::cmodel->yOffset, line.pixEnd.x - line.pixBegin.x, line.pixEnd.y - line.pixBegin.y);
                cr->stroke();
                break;
            case utils::elipseType:
                cr->set_line_width(line.width);
                // cr->move_to(line.pixBegin.x+dc::cmodel->xOffset, line.pixBegin.y+dc::cmodel->yOffset);
                cr->arc(line.pixBegin.x+dc::cmodel->xOffset, line.pixBegin.y+dc::cmodel->yOffset, std::abs((line.pixEnd.x - line.pixBegin.x)), 0, M_PI*2);
                cr->stroke();
                break;
            case utils::lineType:
                cr->set_line_width(line.width);
                cr->move_to(line.pixBegin.x+dc::cmodel->xOffset, line.pixBegin.y+dc::cmodel->yOffset);
                cr->line_to(line.pixEnd.x+dc::cmodel->xOffset, line.pixEnd.y+dc::cmodel->yOffset);
                cr->stroke();
                break;
            default:
                std::cerr << "Not implemented (yet)" << std::endl;
                break;
        }
    }
}

std::string dc::rgb2hex(double r, double g, double b)
{
    std::stringstream colorStream;

    colorStream << std::hex << std::lround(r*255);
    colorStream << std::hex << std::lround(g*255);
    colorStream << std::hex << std::lround(b*255);
    
    colorStream << std::dec;

    return colorStream.str();
}

dc::rgb_color dc::hex2rgb(std::string hex_color)
{
    dc::rgb_color color;

    for(auto letter : hex_color)
    {
        if(letter >= '0' && letter <= '9')
        {
            // digit ok
            continue;
        }

        if(letter >= 'A' && letter <= 'Z')
        {
            // uppercase letter ok
            continue;
        }

        if(letter >= 'a' && letter <= 'z')
        {
            // lowercase letter ok
            continue;
        }
        
        std::cerr << "Error: unexpected char in color: '" << letter << "' exiting..." << std::endl;
        exit(99);
    }

    color.r = (std::stoi(hex_color.substr(0, 2), 0, 16) / 255.f);
    color.g = (std::stoi(hex_color.substr(2, 2), 0, 16) / 255.f);
    color.b = (std::stoi(hex_color.substr(4, 2), 0, 16) / 255.f);

    return color;
}
