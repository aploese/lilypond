/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2006--2020 Han-Wen Nienhuys <hanwen@xs4all.nl>
           Erik Sandberg <mandolaerik@gmail.com>

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

#ifndef EVENT_ITERATOR_HH
#define EVENT_ITERATOR_HH

#include "simple-music-iterator.hh"

class Event_iterator final : public Simple_music_iterator
{
  OVERRIDE_CLASS_NAME (Event_iterator);

public:
  DECLARE_SCHEME_CALLBACK (constructor, ());
  Event_iterator ();
  Event_iterator (Event_iterator const &);

protected:
  void create_contexts () override;
  void process (Moment) override;
};

#endif // EVENT_ITERATOR_HH
