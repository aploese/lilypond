%% DO NOT EDIT this file manually; it was automatically
%% generated from the LilyPond Snippet Repository
%% (http://lsr.di.unimi.it).
%%
%% Make any changes in the LSR itself, or in
%% `Documentation/snippets/new/`, then run
%% `scripts/auxiliar/makelsr.pl`.
%%
%% This file is in the public domain.

\version "2.25.1"

\header {
  lsrtags = "contexts-and-engravers, pitches"

  texidoc = "
LilyPond can alter the stem direction of the middle note on a staff so
that it follows the melody, by adding the @code{Melody_engraver} to the
@code{Voice} context.

The context property @code{suspendMelodyDecisions} may be used to turn
off this behavior locally.
"

  doctitle = "Automatically changing the stem direction of the middle note based on the melody"
} % begin verbatim


\relative c'' {
  \time 3/4
  a8 b g f b g |
  \set suspendMelodyDecisions = ##t
  a  b g f b g |
  \unset suspendMelodyDecisions
  c  b d c b c |
}

\layout {
  \context {
    \Voice
    \consists "Melody_engraver"
    \autoBeamOff
  }
}
