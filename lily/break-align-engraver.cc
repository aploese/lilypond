/*   
  break-align-engraver.cc --  implement Break_align_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */
#include "engraver.hh"
#include "protected-scm.hh"
#include "break-align-interface.hh"
#include "item.hh"
#include "align-interface.hh"
#include "axis-group-interface.hh"


class Break_align_engraver : public Engraver
{
  Item *align_;
  Protected_scm column_alist_;
  Item *left_edge_;

  void add_to_group (SCM,Item*);
protected:
  virtual void finalize ();
  virtual void acknowledge_grob (Grob_info i);
  virtual void stop_translation_timestep ();
  void add_column (SCM);
  
public:
  TRANSLATOR_DECLARATIONS (Break_align_engraver);
};

void
Break_align_engraver::add_column (SCM smob)
{
  Grob * e = unsmob_grob (smob);
  Break_align_interface::add_element (align_,e);
  typeset_grob (e);
}

void
Break_align_engraver::finalize ()
{
  column_alist_ = SCM_EOL;
}

void
Break_align_engraver::stop_translation_timestep ()
{
  SCM order = get_property ("breakAlignOrder");
  for (; gh_pair_p (order); order = ly_cdr (order))
    {
      SCM p = scm_assoc (ly_car (order), column_alist_);
      if (gh_pair_p (p))
	{
	  add_column (ly_cdr (p));
	  column_alist_ = scm_assoc_remove_x (column_alist_, ly_car (order));
	}
    }

  for (SCM p = column_alist_; gh_pair_p (p); p = ly_cdr (p))
    {
      SCM pair = ly_car (p);
      add_column (ly_cdr (pair));
    }

  
  column_alist_ = SCM_EOL;

  if (align_)
    {
      typeset_grob (align_);
      align_ = 0;
    }
  if (left_edge_)
    {
      typeset_grob (left_edge_);
      left_edge_ = 0;
    }
}


Break_align_engraver::Break_align_engraver ()
{
  column_alist_ = SCM_EOL;
  left_edge_ = 0;
  align_ = 0;
}

void
Break_align_engraver::acknowledge_grob (Grob_info inf)
{
  if (Item * item = dynamic_cast <Item *> (inf.grob_))
    {
      /*
	Removed check for item->empty (X_AXIS). --hwn 20/1/04
       */
      if (item->get_parent (X_AXIS))
	return;

      SCM bp=item->get_property ("breakable");
      bool breakable = (to_boolean (bp));
      if (!breakable)
	return ;

      SCM align_name = item->get_property ("break-align-symbol");
      if (!gh_symbol_p (align_name))
	return ;

      if (!align_)
	{
	  align_ = make_item ("BreakAlignment");

	  announce_grob (align_, SCM_EOL);

	  left_edge_ = make_item ("LeftEdge");
	  add_to_group (left_edge_->get_property ("break-align-symbol"), left_edge_);
	  announce_grob (left_edge_, SCM_EOL);
	}
      
      add_to_group (align_name, item);
    }
}

void
Break_align_engraver::add_to_group (SCM align_name, Item*item)
{
  SCM s = scm_assoc (align_name, column_alist_);
  Item * group = 0;

  if (s != SCM_BOOL_F)
    {
      Grob *e =  unsmob_grob (ly_cdr (s));
      group = dynamic_cast<Item*> (e);
    }
  else
    {
      group = make_item ("BreakAlignGroup");

      group->set_property ("break-align-symbol", align_name);
      group->set_parent (align_, Y_AXIS);
      announce_grob (group, item->self_scm ());
	  
      column_alist_ = scm_assoc_set_x (column_alist_, align_name, group->self_scm ());

    }
  Axis_group_interface::add_element (group, item);
}

ENTER_DESCRIPTION (Break_align_engraver,
/* descr */       "Align grobs with corresponding break-align-symbols into groups, and order the groups according to breakAlignOrder",
/* creats*/       "BreakAlignment BreakAlignGroup LeftEdge",
/* accepts */     "",
/* acks  */       "break-aligned-interface"
,/* reads */       "breakAlignOrder",
/* write */       "");
