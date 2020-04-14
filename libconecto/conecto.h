/* conecto.h
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

#include "constants.h"
#include "exceptions.h"
#include "backend.h"
#include "discovery.h"
#include "device.h"
#include "network-packet.h"
#include "communication-channel.h"
#include "crypt.h"
#include "config-file.h"
#include "abstract-packet-handler.h"

// Plugins
#include "ping.h"
#include "battery.h"
#include "notifications.h"
#include "mouse.h"