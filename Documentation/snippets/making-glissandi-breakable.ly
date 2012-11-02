% DO NOT EDIT this file manually; it is automatically
% generated from Documentation/snippets/new
% Make any changes in Documentation/snippets/new/
% and then run scripts/auxiliar/makelsr.py
%
% This file is in the public domain.
%% Note: this file works from version 2.17.6
\version "2.17.6"

\header {
  lsrtags = "staff-notation, tweaks-and-overrides"

  texidoc = "
Setting the @code{breakable} property to @code{#t} in combination with
@code{after-line-breaking} allows a glissando to break if it occurs
at a line break:
"

  doctitle = "Making glissandi breakable"
} % begin verbatim


glissandoSkipOn = {
  \override NoteColumn.glissando-skip = ##t
  \override NoteHead.transparent = ##t
  \override NoteHead.no-ledgers = ##t
}

\relative c'' {
  \override Glissando.breakable = ##t
  \override Glissando.after-line-breaking = ##t
  f1\glissando |
  \break
  a4 r2. |
  f1\glissando
  \once \glissandoSkipOn
  \break
  a2 a4 r4 |
}
