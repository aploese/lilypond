/*
  time_signature-engraver.hh -- declare  Time_signature_engraver

  source file of the GNU LilyPond music typesetter

  (c)  1997--1998 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef METERGRAV_HH
#define METERGRAV_HH
#include "engraver.hh"
#include "time-description.hh"
#include "rhythmic-grouping.hh"

/**
  generate time_signatures. 
  */
class Time_signature_engraver : public Engraver {
protected:
  virtual void do_process_requests();
  virtual void do_pre_move_processing();
public:
  VIRTUAL_COPY_CONS(Translator);
  Time_signature * time_signature_p_;

  Time_signature_engraver();
  
};
#endif // METERGRAV_HH
