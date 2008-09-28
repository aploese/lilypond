%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.61"

\header {
  lsrtags = "winds"

  texidoc = "
Breathing signs are available in different tastes: commas (default),
ticks, vees and \"railroad tracks\" (caesura).

"
  doctitle = "Breathing signs"
} % begin verbatim
\new Staff \relative c'' {
  \key es \major
  \time 3/4
  % this bar contains no \breathe
  << { g4 as g } \\ { es4 bes es } >> |
  % Modern notation:
  % by default, \breathe uses the rcomma, just as if saying:
  % \override BreathingSign  #'text = #(make-musicglyph-markup "scripts.rcomma")
  << { g4 as g } \\ { es4 \breathe bes es } >> |
  
  % rvarcomma and lvarcomma are variations of the default rcomma and lcomma
  % N.B.: must use Staff context here, since we start a Voice below
  \override Staff.BreathingSign  #'text = #(make-musicglyph-markup "scripts.rvarcomma")
  << { g4 as g } \\ { es4 \breathe bes es } >> |
  
  % vee
  \override BreathingSign  #'text = #(make-musicglyph-markup "scripts.upbow")
  es8[ d es f g] \breathe f |
  
  % caesura
  \override BreathingSign  #'text = #(make-musicglyph-markup "scripts.caesura.curved")
  es8[ d] \breathe  es[ f g f] |
  es2 r4 \bar "||"
}
