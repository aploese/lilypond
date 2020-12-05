%% DO NOT EDIT this file manually; it is automatically
%% generated from Documentation/snippets/new
%% Make any changes in Documentation/snippets/new/
%% and then run scripts/auxiliar/makelsr.py
%%
%% This file is in the public domain.
%% Note: this file works from version 2.23.1
\version "2.23.1"

\header {
  lsrtags = "expressive-marks"
  texidoc = "
The ends of hairpins may be aligned to the @code{LEFT}, @code{CENTER}
or @code{RIGHT} of @code{NoteColumn} grobs by overriding the property
@code{endpoint-alignments}, which is a pair of numbers representing
the left and right ends of the hairpin.  @code{endpoint-alignments}
are expected to be directions (either -1, 0 or@tie{}1).  Other values
will be transformed with a warning.  The right end of a hairpin
terminating at a rest is not affected, always ending at the left edge
of the rest.
"
  doctitle = "Aligning the ends of hairpins to NoteColumn directions"
} % begin verbatim


{
  c'2\< <c' d'>\! |
  \override Hairpin.endpoint-alignments = #'(1 . -1)
  c'2\< <c' d'>\! |
  \override Hairpin.endpoint-alignments = #`(,LEFT . ,CENTER)
  c'2\< <c' d'>\! |
}
