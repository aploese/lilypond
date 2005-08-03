/*
  part-combine-engraver.cc -- implement PC-engraver

  source file of the GNU LilyPond music typesetter

  (c) 2000--2005 Jan Nieuwenhuizen <janneke@gnu.org>

  Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"
#include "text-interface.hh"
#include "note-head.hh"
#include "stem.hh"
#include "side-position-interface.hh"
#include "multi-measure-rest.hh"

class Part_combine_engraver : public Engraver
{
  TRANSLATOR_DECLARATIONS (Part_combine_engraver);

protected:
  DECLARE_ACKNOWLEDGER (note_head);
  DECLARE_ACKNOWLEDGER (stem);
  
  void process_music ();
  void stop_translation_timestep ();
  virtual bool try_music (Music *);
private:
  Item *text_;
  Music *event_;
};

bool
Part_combine_engraver::try_music (Music *m)
{
  event_ = m;
  return true;
}

Part_combine_engraver::Part_combine_engraver ()
{
  text_ = 0;
  event_ = 0;
}

void
Part_combine_engraver::process_music ()
{
  if (event_
      && to_boolean (get_property ("printPartCombineTexts")))
    {
      SCM what = event_->get_property ("part-combine-status");
      SCM text = SCM_EOL;
      if (what == ly_symbol2scm ("solo1"))
	text = get_property ("soloText");
      else if (what == ly_symbol2scm ("solo2"))
	text = get_property ("soloIIText");
      else if (what == ly_symbol2scm ("unisono"))
	text = get_property ("aDueText");

      if (Text_interface::is_markup (text))
	{
	  text_ = make_item ("CombineTextScript", event_->self_scm ());
	  text_->set_property ("text", text);
	}
    }
}

void
Part_combine_engraver::acknowledge_note_head (Grob_info i)
{
  if (text_)
    {
      Grob *t = text_;
      Side_position_interface::add_support (t, i.grob ());
      if (Side_position_interface::get_axis (t) == X_AXIS
	  && !t->get_parent (Y_AXIS))
	t->set_parent (i.grob (), Y_AXIS);
    }
}

void
Part_combine_engraver::acknowledge_stem (Grob_info i)
{
  if (text_)
      Side_position_interface::add_support (text_, i.grob ());
}

void
Part_combine_engraver::stop_translation_timestep ()
{
  text_ = 0;
  event_ = 0;
}

#include "translator.icc"
ADD_ACKNOWLEDGER (Part_combine_engraver, note_head);
ADD_ACKNOWLEDGER (Part_combine_engraver, stem);
ADD_TRANSLATOR (Part_combine_engraver,
		/* descr */ "Part combine engraver for orchestral scores:		"
		"Print markings a2, Solo, Solo II, and unisono ",
		/* creats*/ "CombineTextScript",
		/* accepts */ "part-combine-event",
		/* reads */ "printPartCombineTexts soloText soloIIText "
		"aDueText",
		/* write */ "");
