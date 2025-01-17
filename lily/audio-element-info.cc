/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1997--2023 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "audio-element-info.hh"

#include "translator-group.hh"
#include "context.hh"

using std::vector;

vector<Context *>
Audio_element_info::origin_contexts (Translator *end) const
{
  Context *t = origin_trans_->context ();
  vector<Context *> r;
  do
    {
      r.push_back (t);
      t = t->get_parent ();
    }
  while (t && t != end->context ());

  return r;
}
