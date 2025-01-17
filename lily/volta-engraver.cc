/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2000--2023 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "engraver.hh"

#include "axis-group-interface.hh"
#include "context.hh"
#include "grob-array.hh"
#include "international.hh"
#include "lily-imports.hh"
#include "ly-scm-list.hh"
#include "ly-smob-list.hh"
#include "note-column.hh"
#include "item.hh"
#include "side-position-interface.hh"
#include "staff-symbol.hh"
#include "stream-event.hh"
#include "text-interface.hh"
#include "volta-bracket.hh"
#include "warn.hh"

#include "translator.icc"

#include <string>
#include <vector>

// State pertaining to volta spans at a specific depth of nested folded
// repeats.
class Volta_layer
{
public:
  Stream_event *start_ev_ = nullptr;
  Stream_event *stop_prev_ev_ = nullptr;
  Stream_event *stop_curr_ev_ = nullptr; // to handle an empty bracket
  Moment start_mom_;
  Spanner *bracket_ = nullptr;
  Spanner *end_bracket_ = nullptr;
  Spanner *spanner_ = nullptr;
  SCM text_ = SCM_EOL;
  bool start_bracket_this_timestep_ = false;

public:
  void gc_mark () const { scm_gc_mark (text_); }
};

struct Preinit_Volta_engraver
{
  // Entry [n] pertains to volta spans in the nth-deep folded repeat.  [0] is
  // used if \volta appears at the top level, which is not expected, but is
  // easily written.
  std::vector<Volta_layer> layers_;
};

/*
  Create Volta spanners, by reading repeatCommands  property, usually
  set by Volta_repeat_iterator.
*/
class Volta_engraver : Preinit_Volta_engraver, public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Volta_engraver);

protected:
  void acknowledge_bar_line (Grob_info_t<Item>);
  void listen_dal_segno (Stream_event *);
  void listen_fine (Stream_event *);
  void listen_volta_span (Stream_event *);
  static std::string format_numbers (SCM volta_numbers);

  void derived_mark () const override;
  void finalize () override;
  void start_translation_timestep ();
  void stop_translation_timestep ();
  void process_music ();

  bool should_close_end_ = false;
};

void
Volta_engraver::derived_mark () const
{
  for (const auto &layer : layers_)
    layer.gc_mark ();
}

void
Volta_engraver::finalize ()
{
  layers_.clear ();
}

Volta_engraver::Volta_engraver (Context *c)
  : Engraver (c)
{
  // We need at least one layer to support manual repeat commands.
  // Others may be created as needed.
  layers_.resize (1);
}

// TODO: The volta number formatter could be a Scheme procedure configured as a
// context property.  It might make life easier if the formatter received a
// list with ranges represented by pairs rather than making the formatter
// implement the range-discovery algorithm that is implemented here.
std::string
Volta_engraver::format_numbers (SCM volta_numbers)
{
  volta_numbers = scm_sort_list (volta_numbers, Guile_user::less);
  volta_numbers = Srfi_1::delete_duplicates (volta_numbers, Guile_user::equal);

  // Use a dash for runs of 3 or more.  Behind Bars has an example using "1.2."
  // (p.236) but otherwise doesn't say much about this.
  //
  // TODO: It seems that "1.2.3." might also be more readable than "1.-3."
  // Should there be a context property controlling how large a range should be
  // before using a dash?

  constexpr auto EN_DASH = "\u2013";
  constexpr auto HAIR_SPACE = "\u200a";

  std::string result;

  if (scm_is_null (volta_numbers))
    return result;

  size_t range_start = 0;
  size_t prev_num = 0;

  auto handle_num = [&] (size_t num) {
    if (range_start)
      {
        if (num != prev_num + 1) // end range
          {
            if (!result.empty ())
              result += HAIR_SPACE;
            result += std::to_string (range_start) + '.';
            result += (prev_num - range_start > 1) ? EN_DASH : HAIR_SPACE;
            result += std::to_string (prev_num) + '.';
            range_start = 0;
          }
      }
    else if (prev_num)
      {
        if (num != prev_num + 1)
          {
            if (!result.empty ())
              result += HAIR_SPACE;
            result += std::to_string (prev_num) + '.';
          }
        else
          {
            range_start = prev_num;
          }
      }

    prev_num = num;
  };

  for (auto num : as_ly_scm_list_t<size_t> (volta_numbers))
    {
      handle_num (num);
    }
  handle_num (0);

  return result;
}

void
Volta_engraver::listen_dal_segno (Stream_event *)
{
  should_close_end_ = true;
}

void
Volta_engraver::listen_fine (Stream_event *)
{
  should_close_end_ = true;
}

void
Volta_engraver::listen_volta_span (Stream_event *ev)
{
  const auto layer_no = from_scm<size_t> (get_property (ev, "volta-depth"), 0);
  if (layer_no >= layers_.size ())
    layers_.resize (layer_no + 1);
  auto &layer = layers_[layer_no];

  // It is common to have the same repeat structure in multiple voices, so we
  // ignore simultaneous events; but it might not be a bad thing to add some
  // consistency checks here if they could catch some kinds of user error.

  auto dir = from_scm<Direction> (get_property (ev, "span-direction"));
  if (dir == START)
    {
      if (!layer.start_ev_)
        layer.start_ev_ = ev;
    }
  else if (dir == STOP)
    {
      if (!layer.start_ev_)
        {
          if (!layer.stop_prev_ev_)
            layer.stop_prev_ev_ = ev;
        }
      else
        {
          // When an alternative is empty, we can in one timestep receive a stop
          // event for the previous alternative and the current alternative.
          // (This code will not handle consecutive empty alternatives, but it
          // covers the most important case: an empty final alternative.)
          SCM volta_numbers_sym = ly_symbol2scm ("volta-numbers");
          SCM start_nums = get_property (layer.start_ev_, volta_numbers_sym);
          SCM these_nums = get_property (ev, volta_numbers_sym);
          if (!ly_is_equal (these_nums, start_nums))
            {
              if (!layer.stop_prev_ev_)
                layer.stop_prev_ev_ = ev;
            }
          else
            {
              if (!layer.stop_curr_ev_)
                layer.stop_curr_ev_ = ev;
            }
        }
    }
  else
    {
      ev->programming_error ("invalid direction of volta-span-event");
    }
}

void
Volta_engraver::process_music ()
{
  for (size_t layer_no = 0; layer_no < layers_.size (); ++layer_no)
    {
      auto &layer = layers_[layer_no];

      bool manual_start = false;
      bool manual_end = false;

      if (layer_no == 0) // manual repeat commands
        {
          SCM repeat_commands = get_property (this, "repeatCommands");
          for (SCM c : as_ly_scm_list (repeat_commands))
            {
              if (scm_is_pair (c)
                  && scm_is_eq (scm_car (c), ly_symbol2scm ("volta"))
                  && scm_is_pair (scm_cdr (c)))
                {
                  SCM label = scm_cadr (c);
                  if (scm_is_false (label))
                    manual_end = true;
                  else
                    {
                      manual_start = true;
                      layer.text_ = label;
                    }
                }
            }
        }

      bool end = manual_end || layer.stop_prev_ev_;
      if (!end && layer.bracket_)
        {
          auto voltaSpannerDuration = from_scm (
            get_property (this, "voltaSpannerDuration"), Moment::infinity ());
          end = (voltaSpannerDuration <= now_mom () - layer.start_mom_);
        }

      bool start = manual_start || layer.start_ev_;
      if (start && layer.bracket_ && !end)
        {
          if (manual_start)
            {
              layer.bracket_->warning (_ ("already have a VoltaBracket;"
                                          "ending it prematurely"));
            }
          end = true;
        }

      if (end)
        {
          if (layer.bracket_)
            {
              layer.end_bracket_ = layer.bracket_;
              layer.bracket_ = nullptr;
            }
          else if (manual_end)
            {
              // FIXME: Be more verbose?
              warning (_ ("no VoltaBracket to end"));
            }
        }

      if (layer.stop_curr_ev_)
        start = false;

      if (start)
        {
          layer.start_bracket_this_timestep_ = true;
          layer.start_mom_ = now_mom ();
          layer.bracket_ = make_spanner ("VoltaBracket", SCM_EOL);

          if (!layer.spanner_)
            {
              layer.spanner_ = make_spanner ("VoltaBracketSpanner", SCM_EOL);

              // Set the vertical order of the layers by adjusting
              // outside-staff-priority.
              if (layer_no)
                {
                  SCM sym = ly_symbol2scm ("outside-staff-priority");
                  SCM pri = get_property (layer.spanner_, sym);
                  set_property (layer.spanner_, sym,
                                scm_difference (pri, to_scm (layer_no)));
                }
            }

          Axis_group_interface::add_element (layer.spanner_, layer.bracket_);
        }
    }
}

void
Volta_engraver::acknowledge_bar_line (Grob_info_t<Item> info)
{
  auto *const item = info.grob ();
  for (auto &layer : layers_)
    {
      if (layer.bracket_)
        Volta_bracket_interface::add_bar (layer.bracket_, item);
      if (layer.end_bracket_)
        Volta_bracket_interface::add_bar (layer.end_bracket_, item);

      if (layer.spanner_)
        Side_position_interface::add_support (layer.spanner_, item);
    }

  // Certain bar lines cause volta brackets to hook down at the end.
  // See the function allow-volta-hook in bar-line.scm.
  if (!should_close_end_)
    {
      SCM glyph = get_property (item, "glyph-left");
      should_close_end_
        = !from_scm<bool> (Lily::volta_bracket_calc_hook_visibility (glyph));
    }
}

void
Volta_engraver::start_translation_timestep ()
{
  should_close_end_ = false;
}

void
Volta_engraver::stop_translation_timestep ()
{
  auto *const ci = unsmob<Item> (get_property (this, "currentCommandColumn"));

  for (auto &layer : layers_)
    {
      if (layer.start_bracket_this_timestep_)
        {
          // check before setting text to respect user overrides
          SCM text_sym = ly_symbol2scm ("text");
          if (scm_is_null (get_property (layer.bracket_, text_sym)))
            {
              // If there is no manual label, format an automatic one.
              if (scm_is_null (layer.text_) && layer.start_ev_)
                {
                  SCM nums = get_property (layer.start_ev_, "volta-numbers");
                  const auto &s = format_numbers (nums);
                  if (!s.empty ())
                    layer.text_ = ly_string2scm (s);
                }

              if (Text_interface::is_markup (layer.text_))
                set_property (layer.bracket_, text_sym, layer.text_);
            }

          layer.start_bracket_this_timestep_ = false;
        }

      if (layer.end_bracket_ && !layer.end_bracket_->get_bound (RIGHT))
        layer.end_bracket_->set_bound (RIGHT, ci);

      if (layer.spanner_ && layer.end_bracket_)
        layer.spanner_->set_bound (RIGHT,
                                   layer.end_bracket_->get_bound (RIGHT));

      if (layer.end_bracket_ && !layer.bracket_)
        {
          SCM staves_found = get_property (this, "stavesFound");
          for (auto *g : as_ly_smob_list<Grob> (staves_found))
            Side_position_interface::add_support (layer.spanner_, g);

          layer.spanner_ = 0;
        }

      if (layer.end_bracket_)
        {
          // TODO: Now that we attempt to handle nested repeats, consider
          // whether there is a case in which one layer should have an end hook
          // and the other should not, and how important it is to get it right.
          if (!should_close_end_)
            {
              SCM eh = get_property (layer.end_bracket_, "edge-height");
              eh = scm_cons (scm_car (eh), to_scm (0));
              set_property (layer.end_bracket_, "edge-height", eh);
            }

          announce_end_grob (layer.end_bracket_, SCM_EOL);
          layer.end_bracket_ = 0;
        }

      if (layer.bracket_ && !layer.bracket_->get_bound (LEFT))
        layer.bracket_->set_bound (LEFT, ci);

      if (layer.spanner_ && layer.bracket_ && !layer.spanner_->get_bound (LEFT))
        layer.spanner_->set_bound (LEFT, layer.bracket_->get_bound (LEFT));

      layer.start_ev_ = nullptr;
      layer.stop_prev_ev_ = nullptr;
      layer.stop_curr_ev_ = nullptr;
      layer.text_ = SCM_EOL;
    }
}

/*
  TODO: should attach volta to paper-column if no bar is found.
*/
void
Volta_engraver::boot ()
{
  ADD_ACKNOWLEDGER (bar_line);
  ADD_LISTENER (dal_segno);
  ADD_LISTENER (fine);
  ADD_LISTENER (volta_span);
}

ADD_TRANSLATOR (Volta_engraver,
                /* doc */
                R"(
Make volta brackets.
                )",

                /* create */
                R"(
VoltaBracket
VoltaBracketSpanner
                )",

                /* read */
                R"(
currentCommandColumn
repeatCommands
stavesFound
voltaSpannerDuration
                )",

                /* write */
                "");
