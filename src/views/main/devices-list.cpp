/* devices-list.cpp
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

#include "devices-list.h"
#include "../../utils/icons.h"
#include "../../controllers/active-device-manager.h"

using namespace App::Views::Main;
using namespace App::Models;

namespace {

class CellRendererExpander : public Gtk::CellRenderer {
  public:
    CellRendererExpander () {}
    ~CellRendererExpander () {}

  protected:
    Gtk::SizeRequestMode get_request_mode_vfunc () const override { return Gtk::SIZE_REQUEST_HEIGHT_FOR_WIDTH; }
    void get_preferred_width_vfunc (Gtk::Widget& widget, int& minimum_size, int& natural_size) const override
    {
        apply_style_changes (widget);
        minimum_size = natural_size = get_arrow_size (widget) + 2 * (int) property_xpad ().get_value ();
        revert_style_changes (widget);
    }
    void get_preferred_height_for_width_vfunc (Gtk::Widget& widget, int width, int& minimum_size,
                                               int& natural_size) const override
    {
        apply_style_changes (widget);
        minimum_size = natural_size = get_arrow_size (widget) + 2 * (int) property_ypad ().get_value ();
        revert_style_changes (widget);
    }
    void render_vfunc (const Cairo::RefPtr<Cairo::Context>& cr, Gtk::Widget& widget, const Gdk::Rectangle& bg_area,
                       const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags) override
    {
        if (!property_is_expander ().get_value ()) return;
        auto ctx = apply_style_changes (widget);

        Gdk::Rectangle aligned_area;
        get_aligned_area (widget, flags, cell_area, aligned_area);
        int arrow_size = std::min (get_arrow_size (widget), aligned_area.get_width ());

        int offset = arrow_size / 2;
        int x = aligned_area.get_x () + aligned_area.get_width () / 2 - offset;
        int y = aligned_area.get_y () + aligned_area.get_height () / 2 - offset;

        auto state = ctx->get_state ();
        ctx->set_state (property_is_expanded ().get_value () ? state | Gtk::STATE_FLAG_CHECKED
                                                             : state & ~Gtk::STATE_FLAG_CHECKED);
        ctx->render_expander (cr, x, y, arrow_size, arrow_size);
        revert_style_changes (widget);
    }
    Glib::RefPtr<Gtk::StyleContext> apply_style_changes (Gtk::Widget& widget) const
    {
        auto ctx = widget.get_style_context ();
        gtk_style_context_save (ctx->gobj ());
        ctx->add_class ("category-expander");
        return ctx;
    }
    void revert_style_changes (Gtk::Widget& widget) const
    {
        gtk_style_context_restore (widget.get_style_context ()->gobj ());
    }
    int get_arrow_size (Gtk::Widget& widget) const
    {
        int arrow_size;
        gtk_widget_style_get (widget.gobj (), "expander-size", &arrow_size, nullptr);
        return arrow_size;
    }
};

class CellRendererIcon : public Gtk::CellRendererPixbuf {
  public:
    CellRendererIcon ()
    {
        property_mode ().set_value (Gtk::CELL_RENDERER_MODE_ACTIVATABLE);
        property_xpad ().set_value (2);
        property_stock_size ().set_value (Gtk::ICON_SIZE_MENU);
    }
    ~CellRendererIcon () {}
};

class CellRendererStarred : public Gtk::CellRenderer {
  public:
    CellRendererStarred ()
    {
        m_starred_icon = Gtk::IconTheme::get_default ()->load_icon ("starred", 16);
        m_non_starred_icon = Gtk::IconTheme::get_default ()->load_icon ("non-starred", 16);
        property_mode ().set_value (Gtk::CELL_RENDERER_MODE_ACTIVATABLE);
    }
    ~CellRendererStarred () {}

    void set_is_starred (bool is_starred) { m_is_starred = is_starred; }

    sigc::signal<void, const Gtk::TreePath&> signal_toggle_starred;

  protected:
    Gtk::SizeRequestMode get_request_mode_vfunc () const override { return Gtk::SIZE_REQUEST_HEIGHT_FOR_WIDTH; }
    void get_preferred_width_vfunc (Gtk::Widget& widget, int& minimum_size, int& natural_size) const override
    {
        minimum_size = natural_size = 16 + 2 * (int) property_xpad ().get_value ();
    }
    void get_preferred_height_for_width_vfunc (Gtk::Widget& widget, int width, int& minimum_size,
                                               int& natural_size) const override
    {
        minimum_size = natural_size = 16 + 2 * (int) property_ypad ().get_value ();
    }
    void render_vfunc (const Cairo::RefPtr<Cairo::Context>& cr, Gtk::Widget& widget, const Gdk::Rectangle& bg_area,
                       const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags) override
    {
        auto ctx = widget.get_style_context ();
        bool is_hovering = (ctx->get_state () & Gtk::STATE_FLAG_PRELIGHT) == Gtk::STATE_FLAG_PRELIGHT;

        Gdk::Rectangle aligned_area;
        get_aligned_area (widget, flags, cell_area, aligned_area);

        if (m_is_starred) {
            Gdk::Cairo::set_source_pixbuf (cr, m_starred_icon, aligned_area.get_x (), aligned_area.get_y ());
            cr->paint_with_alpha (is_hovering ? 1.0 : 0.75);
        } else if (is_hovering) {
            Gdk::Cairo::set_source_pixbuf (cr, m_non_starred_icon, aligned_area.get_x (), aligned_area.get_y ());
            cr->paint_with_alpha (0.4);
        }
    }
    bool activate_vfunc (GdkEvent* event, Gtk::Widget& widget, const Glib::ustring& path, const Gdk::Rectangle& bg_area,
                         const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags) override
    {
        signal_toggle_starred.emit (Gtk::TreePath (path));
        return true;
    }

    Glib::RefPtr<Gdk::Pixbuf> m_starred_icon;
    Glib::RefPtr<Gdk::Pixbuf> m_non_starred_icon;

    bool m_is_starred;
};

} // namespace

DevicesList::DevicesList (const Glib::RefPtr<ConnectedDevices>&   connected_devices,
                          const Glib::RefPtr<UnavailableDevices>& unavailable_devices,
                          const Glib::RefPtr<AvailableDevices>&   available_devices)
    : Gtk::ScrolledWindow ()
    , m_connected_devices (connected_devices)
    , m_unavailable_devices (unavailable_devices)
    , m_available_devices (available_devices)
{
    m_columns.add (m_column_icon);
    m_columns.add (m_column_text);
    m_columns.add (m_column_device);
    m_columns.add (m_column_starred);
    m_columns.add (m_column_can_star);
    m_tree_store = Gtk::TreeStore::create (m_columns);
    m_tree_view.set_model (m_tree_store);
    m_tree_view.get_style_context ()->add_class ("source-list");
    m_tree_view.get_style_context ()->add_class ("view");
    add (m_tree_view);

    m_row_connected = *m_tree_store->append ();
    m_row_connected.set_value (m_column_icon, get_color_pixbuf (Gdk::RGBA ("#26a269")));
    m_row_connected.set_value (m_column_text, Glib::ustring ("Connected"));
    m_row_connected.set_value (m_column_starred, false);
    m_row_connected.set_value (m_column_can_star, false);
    m_row_unavailable = *m_tree_store->append ();
    m_row_unavailable.set_value (m_column_icon, get_color_pixbuf (Gdk::RGBA ("#e5a50a")));
    m_row_unavailable.set_value (m_column_text, Glib::ustring ("Paired"));
    m_row_unavailable.set_value (m_column_starred, false);
    m_row_unavailable.set_value (m_column_can_star, false);
    m_row_available = *m_tree_store->append ();
    m_row_available.set_value (m_column_icon, get_color_pixbuf (Gdk::RGBA ("#9a9996")));
    m_row_available.set_value (m_column_text, Glib::ustring ("Available"));
    m_row_available.set_value (m_column_starred, false);
    m_row_available.set_value (m_column_can_star, false);

    // Update connected devices
    m_connected_devices->signal_row_inserted ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_insert_row), m_row_connected));
    m_connected_devices->signal_row_changed ().connect (sigc::mem_fun (*this, &DevicesList::on_update_row_connected));
    m_connected_devices->signal_row_deleted ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_delete_row), m_row_connected));
    m_connected_devices->signal_rows_reordered ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_rows_reordered), m_row_connected));

    // Update unavailable devices
    m_unavailable_devices->signal_row_inserted ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_insert_row), m_row_unavailable));
    m_unavailable_devices->signal_row_changed ().connect (
            sigc::mem_fun (*this, &DevicesList::on_update_row_unavailable));
    m_unavailable_devices->signal_row_deleted ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_delete_row), m_row_unavailable));
    m_unavailable_devices->signal_rows_reordered ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_rows_reordered), m_row_unavailable));

    // Update available devices
    m_available_devices->signal_row_inserted ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_insert_row), m_row_available));
    m_available_devices->signal_row_changed ().connect (sigc::mem_fun (*this, &DevicesList::on_update_row_available));
    m_available_devices->signal_row_deleted ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_delete_row), m_row_available));
    m_available_devices->signal_rows_reordered ().connect (
            sigc::bind (sigc::mem_fun (*this, &DevicesList::on_rows_reordered), m_row_available));

    // Add already existing devices
    for (auto& child : m_connected_devices->children ()) {
        on_insert_row (m_connected_devices->get_path (child), child, m_row_connected);
        on_update_row_connected (m_connected_devices->get_path (child), child);
    }
    for (auto& child : m_unavailable_devices->children ()) {
        on_insert_row (m_unavailable_devices->get_path (child), child, m_row_unavailable);
        on_update_row_unavailable (m_unavailable_devices->get_path (child), child);
    }
    for (auto& child : m_available_devices->children ()) {
        on_insert_row (m_available_devices->get_path (child), child, m_row_available);
        on_update_row_available (m_available_devices->get_path (child), child);
    }

    // Activated event
    m_tree_view.signal_row_activated ().connect (sigc::mem_fun (*this, &DevicesList::on_activated));
    m_tree_view.set_activate_on_single_click (true);

    m_tree_view.set_halign (Gtk::ALIGN_FILL);
    m_tree_view.set_valign (Gtk::ALIGN_FILL);
    m_tree_view.set_enable_search (false);
    m_tree_view.set_headers_visible (false);
    m_tree_view.set_enable_tree_lines (false);
    m_tree_view.set_show_expanders (false);
    m_item_column.set_expand (true);
    m_tree_view.append_column (m_item_column);
    m_tree_view.signal_button_release_event ().connect (sigc::mem_fun (*this, &DevicesList::on_button_release));
    m_tree_view.set_level_indentation (24);

    // Selection
    m_tree_view.get_selection ()->set_select_function (sigc::mem_fun (*this, &DevicesList::on_select));

    m_cell_expander = std::unique_ptr<Gtk::CellRenderer> (new CellRendererExpander);
    m_cell_expander->property_xpad ().set_value (0);
    m_item_column.pack_end (*m_cell_expander, false);
    m_item_column.set_cell_data_func (*m_cell_expander, sigc::mem_fun (*this, &DevicesList::cell_data_func_expander));

    m_cell_starred = std::unique_ptr<Gtk::CellRenderer> (new CellRendererStarred);
    static_cast<CellRendererStarred&> (*m_cell_starred)
            .signal_toggle_starred.connect (sigc::mem_fun (*this, &DevicesList::on_toggle_starred));
    m_item_column.pack_end (*m_cell_starred, false);
    m_item_column.set_cell_data_func (*m_cell_starred, sigc::mem_fun (*this, &DevicesList::cell_data_func_starred));

    m_cell_text.property_ellipsize ().set_value (Pango::ELLIPSIZE_END);
    m_cell_text.property_xalign ().set_value (0);
    m_item_column.pack_end (m_cell_text, true);
    m_item_column.set_cell_data_func (m_cell_text, sigc::mem_fun (*this, &DevicesList::cell_data_func_name));

    m_cell_icon = std::unique_ptr<Gtk::CellRendererPixbuf> (new CellRendererIcon);
    m_cell_icon->property_xpad ().set_value (8);
    m_item_column.pack_end (*m_cell_icon, false);
    m_item_column.set_cell_data_func (*m_cell_icon, sigc::mem_fun (*this, &DevicesList::cell_data_func_icon));
}

void
DevicesList::cell_data_func_expander (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it)
{
    renderer->property_is_expander ().set_value (it->children ().size () > 0);
}

void
DevicesList::cell_data_func_name (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it)
{
    Gtk::CellRendererText& text_renderer = dynamic_cast<Gtk::CellRendererText&> (*renderer);

    std::string   text = it->get_value (m_column_text);
    Pango::Weight weight = Pango::WEIGHT_NORMAL;
    if (!it->parent ()) weight = Pango::WEIGHT_BOLD;

    text_renderer.property_weight ().set_value (weight);
    text_renderer.property_text ().set_value (text);
}

void
DevicesList::cell_data_func_icon (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it)
{
    Gtk::CellRendererPixbuf& icon_renderer = dynamic_cast<Gtk::CellRendererPixbuf&> (*renderer);

    auto pixbuf = it->get_value (m_column_icon);
    icon_renderer.set_visible (pixbuf.operator bool ());
    if (pixbuf) icon_renderer.property_pixbuf ().set_value (pixbuf);
}

void
DevicesList::cell_data_func_starred (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it)
{
    renderer->set_visible (it->get_value (m_column_can_star));

    CellRendererStarred& icon_renderer = dynamic_cast<CellRendererStarred&> (*renderer);
    icon_renderer.set_is_starred (it->get_value (m_column_starred));
}

bool
DevicesList::on_select (const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreePath& path, bool test)
{
    return m_tree_store->get_iter (path)->parent ();
}

void
DevicesList::on_activated (const Gtk::TreePath& path, Gtk::TreeViewColumn* column)
{
    bool selectable = m_tree_store->get_iter (path)->parent ();
    if (selectable) ACTIVE_DEVICE.activate_device (m_tree_store->get_iter (path)->get_value (m_column_device));
}

bool
DevicesList::on_button_release (GdkEventButton* event)
{
    Gtk::TreePath path;
    int           x = (int) event->x;
    int           y = (int) event->y;
    if (m_tree_view.get_path_at_pos (x, y, path)) {
        if (m_tree_view.row_expanded (path))
            m_tree_view.collapse_row (path);
        else
            m_tree_view.expand_row (path, false);
    }
    return false;
}

Glib::RefPtr<Gdk::Pixbuf>
DevicesList::get_color_pixbuf (Gdk::RGBA color)
{
    auto image = Cairo::ImageSurface::create (Cairo::FORMAT_ARGB32, 16, 16);
    {
        auto cr = Cairo::Context::create (image);
        cr->save ();
        color.set_alpha (0.5);
        Gdk::Cairo::set_source_rgba (cr, color);
        cr->translate (image->get_width () / 2, image->get_height () / 2);
        cr->arc (0, 0, (image->get_width () / 2) - 2, 0, 2 * M_PI);
        cr->fill_preserve ();
        cr->set_line_width (1.3);
        color.set_alpha (1.0);
        Gdk::Cairo::set_source_rgba (cr, color);
        cr->stroke ();
        cr->restore ();
    }

    // Copy to pixbuf
    image->flush ();
    auto pixbuf = Gdk::Pixbuf::create (Gdk::COLORSPACE_RGB, true, 8, image->get_width (), image->get_height ());
    pixbuf->fill (0x00000000);
    uint8_t* data = image->get_data ();
    uint8_t* pixels = pixbuf->get_pixels ();
    int      length = image->get_width () * image->get_height ();
    for (int i = 0; i < length; i++) {
        if (data[3] > 0) {
            pixels[0] = (uint8_t) (data[2] * 255 / data[3]);
            pixels[1] = (uint8_t) (data[1] * 255 / data[3]);
            pixels[2] = (uint8_t) (data[0] * 255 / data[3]);
            pixels[3] = data[3];
        }
        pixels += 4;
        data += 4;
    }
    return pixbuf;
}

void
DevicesList::on_insert_row (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it, Gtk::TreeRow parent)
{
    int pos = path[0];
    m_tree_store->insert (parent.children ()[pos]);
    m_tree_view.expand_row (m_tree_store->get_path (parent), true);
}

void
DevicesList::on_delete_row (const Gtk::TreeModel::Path& path, Gtk::TreeRow parent)
{
    int  pos = path[0];
    auto item = parent.children ()[pos];
    if (item) m_tree_store->erase (item);
}

void
DevicesList::on_update_row_connected (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it)
{
    auto item = m_row_connected.children ()[path[0]];
    if (!item) return;
    item->set_value (m_column_icon,
                     Utils::Icons::get_icon_for_device_type (it->get_value (m_connected_devices->column_type), 16));
    item->set_value (m_column_text, it->get_value (m_connected_devices->column_name));
    item->set_value (m_column_device, m_connected_devices->get_device (it));
    item->set_value (m_column_starred, it->get_value (m_connected_devices->column_starred));
    item->set_value (m_column_can_star, true);
}

void
DevicesList::on_update_row_unavailable (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it)
{
    auto item = m_row_unavailable.children ()[path[0]];
    if (!item) return;
    item->set_value (m_column_icon,
                     Utils::Icons::get_icon_for_device_type (it->get_value (m_unavailable_devices->column_type), 16));
    item->set_value (m_column_text, it->get_value (m_unavailable_devices->column_name));
    item->set_value (m_column_device, m_unavailable_devices->get_device (it));
    item->set_value (m_column_starred, it->get_value (m_unavailable_devices->column_starred));
    item->set_value (m_column_can_star, true);
}

void
DevicesList::on_update_row_available (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it)
{
    auto item = m_row_available.children ()[path[0]];
    if (!item) return;
    item->set_value (m_column_icon,
                     Utils::Icons::get_icon_for_device_type (it->get_value (m_available_devices->column_type), 16));
    item->set_value (m_column_text, it->get_value (m_available_devices->column_name));
    item->set_value (m_column_device, m_available_devices->get_device (it));
    item->set_value (m_column_starred, false);
    item->set_value (m_column_can_star, false);
}

void
DevicesList::on_rows_reordered (const Gtk::TreeModel::Path& path, const Gtk::TreeIter& it, int* new_order,
                                Gtk::TreeRow parent)
{
    std::vector<int> order (new_order, new_order + parent.children ().size ());
    m_tree_store->reorder (parent.children (), order);
}

void
DevicesList::on_toggle_starred (const Gtk::TreePath& path)
{
    Gtk::TreeIter iter = m_tree_store->get_iter (path);
    if (iter && iter->get_value (m_column_device) && iter->get_value (m_column_can_star)) {
        Conecto::Backend::get_instance ().get_config ().set_device_starred (iter->get_value (m_column_device),
                                                                            !iter->get_value (m_column_starred));
    }
}