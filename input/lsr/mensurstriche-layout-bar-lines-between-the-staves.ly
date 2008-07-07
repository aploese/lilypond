%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.51"

\header {
  lsrtags = "staff-notation, ancient-notation, contexts-and-engravers, tweaks-and-overrides"

  texidoc = "
The mensurstriche-layout where the bar lines do not show on the staves
but between staves can be achieved with a @code{StaffGroup} instead of
a @code{ChoirStaff}.  The bar line on staves is blanked out by setting
the @code{transparent} property.

"
  doctitle = "Mensurstriche layout (bar lines between the staves)"
} % begin verbatim
global = {
  \override Staff.BarLine #'transparent = ##t
  s1 s
  % the final bar line is not interrupted
  \revert Staff.BarLine #'transparent
  \bar "|."
}
\new StaffGroup \relative c'' {
  <<
    \new Staff { << \global { c1 c } >> }
    \new Staff { << \global { c c } >> }
  >>
}
