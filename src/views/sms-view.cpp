/* sms-view.cpp
 *
 * Copyright 2020 Hannes Schulze <haschu0103@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sms-view.h"

using namespace App::Views;

namespace {

class ContactRow : public Gtk::Grid {
  public:
    ContactRow (const App::Models::SMSStorage::Contact& contact)
        : Gtk::Grid ()
        , m_avatar (granite_widgets_avatar_new_with_default_icon (32), g_object_unref)
    {
        // Sink reference
        g_object_ref_sink (m_avatar.get ());

        set_margin_top (6);
        set_margin_left (6);
        set_margin_right (6);
        set_margin_bottom (6);
        set_column_spacing (6);
        set_row_spacing (3);

        // Layout
        gtk_widget_set_size_request (GTK_WIDGET (m_avatar.get ()), 40, 40);
        attach (*Glib::wrap (GTK_WIDGET (m_avatar.get ())), 0, 0, 1, 2);

        if (contact.display_name == std::string ()) {
            auto* display_name = Gtk::make_managed<Gtk::Label> ();
            display_name->set_markup ("<b>" + contact.phone_numbers.front () + "</b>");
            display_name->set_halign (Gtk::ALIGN_START);
            display_name->set_valign (Gtk::ALIGN_CENTER);
            display_name->set_ellipsize (Pango::ELLIPSIZE_END);
            display_name->set_hexpand (true);
            attach (*display_name, 1, 0, 1, 2);
        } else {
            auto* display_name = Gtk::make_managed<Gtk::Label> ();
            display_name->set_markup ("<b>" + contact.display_name + "</b>");
            display_name->set_halign (Gtk::ALIGN_START);
            display_name->set_valign (Gtk::ALIGN_END);
            display_name->set_ellipsize (Pango::ELLIPSIZE_END);
            display_name->set_hexpand (true);
            attach (*display_name, 1, 0, 1, 1);
 
            auto* phone_number = Gtk::make_managed<Gtk::Label> (contact.phone_numbers.front ());
            phone_number->get_style_context ()->add_class ("dim-label");
            phone_number->set_halign (Gtk::ALIGN_START);
            phone_number->set_valign (Gtk::ALIGN_START);
            phone_number->set_ellipsize (Pango::ELLIPSIZE_END);
            phone_number->set_hexpand (true);
            attach (*phone_number, 1, 1, 1, 1);
        }
    }
    ~ContactRow () = default;

  private:
    std::shared_ptr<GraniteWidgetsAvatar> m_avatar;
};

} // namespace

SMSView::SMSView (const std::shared_ptr<Models::SMSStorage>& model)
    : Gtk::Bin ()
    , m_model (model)
    , m_notebook (granite_widgets_dynamic_notebook_new (), g_object_unref)
{
    // Sink reference
    g_object_ref_sink (m_notebook.get ());

    // Main notebook
    granite_widgets_dynamic_notebook_set_allow_restoring (m_notebook.get (), false);
    granite_widgets_dynamic_notebook_set_tab_bar_behavior (m_notebook.get (),
        GRANITE_WIDGETS_DYNAMIC_NOTEBOOK_TAB_BAR_BEHAVIOR_ALWAYS);
    g_signal_connect (m_notebook.get (), "new-tab-requested", G_CALLBACK (static_on_new_tab_requested), this);
    g_signal_connect (m_notebook.get (), "close-tab-requested", G_CALLBACK (static_on_close_tab_requested), this);

    // Placeholder page
    create_placeholder_tab ();

    add (*Glib::wrap (GTK_WIDGET (m_notebook.get ())));
    gtk_widget_show_all (GTK_WIDGET (m_notebook.get ()));
}

std::shared_ptr<SMSView>
SMSView::create (const std::shared_ptr<Models::SMSStorage>& model)
{
    return std::shared_ptr<SMSView> (new SMSView (model));
}

void
SMSView::create_placeholder_tab ()
{
    m_placeholder = std::make_shared <Gtk::Box> (Gtk::ORIENTATION_VERTICAL, 0);
    Gtk::Label* placeholder_title = Gtk::make_managed<Gtk::Label> ("No open conversations");
    Gtk::Label* placeholder_subtitle = Gtk::make_managed<Gtk::Label> ("Start a new conversation and send SMS messages "
                                                                      "over your phone");
    placeholder_title->get_style_context ()->add_class ("h1");
    placeholder_title->set_margin_bottom (12);
    placeholder_subtitle->get_style_context ()->add_class ("h2");
    placeholder_subtitle->get_style_context ()->add_class ("dim-label");
    placeholder_subtitle->set_line_wrap (true);
    placeholder_subtitle->set_justify (Gtk::JUSTIFY_CENTER);
    m_placeholder->pack_start (*placeholder_title, false, false, 0);
    m_placeholder->pack_start (*placeholder_subtitle, false, false, 0);
    m_placeholder->set_halign (Gtk::ALIGN_CENTER);
    m_placeholder->set_valign (Gtk::ALIGN_CENTER);
    m_placeholder->set_margin_top (12);
    m_placeholder->set_margin_left (12);
    m_placeholder->set_margin_right (12);
    m_placeholder->set_margin_bottom (12);
    m_placeholder_tab.reset (granite_widgets_tab_new ("Welcome", nullptr, GTK_WIDGET (m_placeholder->gobj ())),
                             g_object_unref);
    g_object_ref_sink (m_placeholder_tab.get ());
    granite_widgets_dynamic_notebook_insert_tab (m_notebook.get (), m_placeholder_tab.get (), 0);
}

void
SMSView::on_new_tab_requested ()
{
    auto* btn = Glib::wrap (GTK_NOTEBOOK (gtk_bin_get_child (GTK_BIN (m_notebook.get ()))))->get_action_widget ();
    auto popover = std::make_shared<Gtk::Popover> (*btn);
    popover->set_position (Gtk::POS_BOTTOM);

    auto* scrolled_window = Gtk::make_managed<Gtk::ScrolledWindow> ();
    auto* listbox = Gtk::make_managed<Gtk::ListBox> ();
    listbox->set_margin_top (12);
    listbox->set_margin_left (12);
    listbox->set_margin_right (12);
    listbox->set_margin_bottom (12);
    listbox->set_selection_mode (Gtk::SELECTION_BROWSE);
    for (const auto& contact : m_model->get_available_contacts ())
        listbox->add (*Gtk::manage (new ContactRow (contact)));
    scrolled_window->add (*listbox);
    scrolled_window->set_size_request (300, 390);
    popover->add (*scrolled_window);

    popover->show_all_children ();
    m_widget_owners.push_back (popover);
    popover->popup ();
}

bool
SMSView::on_close_tab_requested (GraniteWidgetsTab* tab)
{
    if (tab == m_placeholder_tab.get ()) return false;

    if (granite_widgets_dynamic_notebook_get_n_tabs (m_notebook.get ()) <= 1)
        Glib::signal_idle ().connect_once ([this] () { create_placeholder_tab (); });

    return true;
}

void
SMSView::static_on_new_tab_requested (GraniteWidgetsDynamicNotebook* sender, SMSView* self)
{
    self->on_new_tab_requested ();
}

gboolean
SMSView::static_on_close_tab_requested (GraniteWidgetsDynamicNotebook* sender, GraniteWidgetsTab* tab, SMSView* self)
{
    return self->on_close_tab_requested (tab);
}

void
SMSView::set_device (const std::shared_ptr<Conecto::Device>& device)
{
    m_device = device;

    auto conversations = m_model->get_conversation_contacts (*device);
    {
        int count = granite_widgets_dynamic_notebook_get_n_tabs (m_notebook.get ());
        for (int i = 0; i < count; i++)
            granite_widgets_dynamic_notebook_remove_tab (m_notebook.get (),
                granite_widgets_dynamic_notebook_get_tab_by_index (m_notebook.get (), 0));
    }

    if (conversations.empty ()) {
        create_placeholder_tab ();
    } else {
        for (const auto& contact : conversations) {
            std::string display = contact.display_name == std::string () ? contact.phone_numbers.front () : contact.display_name;
            auto lbl = std::make_shared<Gtk::Label> (display);
            GraniteWidgetsTab* tab = granite_widgets_tab_new (display.c_str (), nullptr, GTK_WIDGET (lbl->gobj ()));
            m_widget_owners.push_back (lbl);
            granite_widgets_dynamic_notebook_insert_tab (m_notebook.get (), tab,
                                                         granite_widgets_dynamic_notebook_get_n_tabs (m_notebook.get ()) - 1);
        }
    }
}