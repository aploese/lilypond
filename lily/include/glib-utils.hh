/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2022-2023 Jean Abou Samra <jean@abou-samra.fr>

  LilyPond is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LilyPond is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <glib.h>

#include <memory>

// unique_glib_ptr is like unique_stdlib_ptr from flower, but uses g_free
// instead of free.  Use for anything GLib returns that is owned by the caller
// (but not for objects with shared ownership, whose reference count should be
// decremented instead, using g_xxxx_unref).

struct Glib_freer
{
  void operator() (void *p) { g_free (p); }
};
template <typename T>
using unique_glib_ptr = std::unique_ptr<T, Glib_freer>;
