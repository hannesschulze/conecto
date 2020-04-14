/* devices-list.h
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

#pragma once

#include <gtkmm.h>
#include "../../models/connected-devices.h"
#include "../../models/unavailable-devices.h"
#include "../../models/available-devices.h"

namespace App {
namespace Views {
namespace Main {

/**
 * @brief A list shown on the left-hand side of the main window
 *
 * A list shown in the main window, listing all online, offline and available devices
 *
 * Connected to the following models: @p App::Models::ConnectedDevices, @p App::Models::UnavailableDevices,
 * @p App::Models::AvailableDevices
 */
class DevicesList : public Gtk::ScrolledWindow {
  public:
    /**
     * @brief Create a list of devices, visualizing the specified models
     *
     * @param connected_devices The connected-devices model
     * @param unavailable_devices The unavailable-devices model
     * @param available_devices The available-devices model
     */
    DevicesList (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                 const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                 const Glib::RefPtr<Models::AvailableDevices>&   available_devices);
    ~DevicesList () {}

    DevicesList (const DevicesList&) = delete;
    DevicesList& operator= (const DevicesList&) = delete;

  private:
    Glib::RefPtr<Models::ConnectedDevices>   m_connected_devices;
    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;
    Glib::RefPtr<Models::AvailableDevices>   m_available_devices;

    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>>        m_column_icon;
    Gtk::TreeModelColumn<Glib::ustring>                    m_column_text;
    Gtk::TreeModelColumn<std::shared_ptr<Conecto::Device>> m_column_device;
    Gtk::TreeModelColumn<bool>                             m_column_starred;
    Gtk::TreeModelColumn<bool>                             m_column_can_star;
    Gtk::TreeViewColumn                                    m_item_column;

    Gtk::TreeModelColumnRecord   m_columns;
    Glib::RefPtr<Gtk::TreeStore> m_tree_store;
    Gtk::TreeView                m_tree_view;

    Gtk::TreeRow m_row_connected;
    Gtk::TreeRow m_row_unavailable;
    Gtk::TreeRow m_row_available;

    std::unique_ptr<Gtk::CellRenderer>       m_cell_expander;
    Gtk::CellRendererText                    m_cell_text;
    std::unique_ptr<Gtk::CellRendererPixbuf> m_cell_icon;
    std::unique_ptr<Gtk::CellRenderer>       m_cell_starred;

    void cell_data_func_expander (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it);
    void cell_data_func_name (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it);
    void cell_data_func_icon (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it);
    void cell_data_func_starred (Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& it);
    bool on_select (const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreePath& path, bool);
    bool on_button_release (GdkEventButton* event);
    void on_toggle_starred (const Gtk::TreePath& path);

    void on_insert_row (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it, Gtk::TreeRow parent);
    void on_delete_row (const Gtk::TreeModel::Path& path, Gtk::TreeRow parent);
    void on_rows_reordered (const Gtk::TreeModel::Path& path, const Gtk::TreeIter& it, int* new_order,
                            Gtk::TreeRow parent);
    void on_update_row_connected (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it);
    void on_update_row_unavailable (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it);
    void on_update_row_available (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it);
    void on_activated (const Gtk::TreePath& path, Gtk::TreeViewColumn* column);

    static Glib::RefPtr<Gdk::Pixbuf> get_color_pixbuf (Gdk::RGBA color);
};

} // namespace Main
} // namespace Views
} // namespace App