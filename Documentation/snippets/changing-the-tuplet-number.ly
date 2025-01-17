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
  lsrtags = "rhythms"

  texidoc = "
By default, only the numerator of the tuplet number is printed over the
tuplet bracket, i.e., the numerator of the argument to the
@code{\\tuplet} command.

Alternatively, @emph{num}:@emph{den} of the tuplet number may be
printed, or the tuplet number may be suppressed altogether.
"

  doctitle = "Changing the tuplet number"
} % begin verbatim


\relative c'' {
  \tuplet 3/2 { c8 c c }
  \tuplet 3/2 { c8 c c }
  \override TupletNumber.text = #tuplet-number::calc-fraction-text
  \tuplet 3/2 { c8 c c }
  \omit TupletNumber
  \tuplet 3/2 { c8 c c }
}
