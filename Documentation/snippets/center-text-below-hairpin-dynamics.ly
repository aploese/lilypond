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
  lsrtags = "expressive-marks, really-cool, scheme-language, text"

  texidoc = "
This example provides a function to typeset a hairpin (de)crescendo
with some additional text below it, such as @qq{molto} or @qq{poco}. The
added text will change the direction according to the direction of the
hairpin. The Hairpin is aligned to DynamicText.

The example also illustrates how to modify the way an object is
normally printed, using some Scheme code.
"

  doctitle = "Center text below hairpin dynamics"
} % begin verbatim


\paper { tagline = ##f }

hairpinWithCenteredText =
#(define-music-function (text) (markup?)
  #{
    \once \override Voice.Hairpin.after-line-breaking =
      #(lambda (grob)
        (let* ((stencil (ly:hairpin::print grob))
               (par-y (ly:grob-parent grob Y))
               (dir (ly:grob-property par-y 'direction))
               (staff-line-thickness
                 (ly:output-def-lookup (ly:grob-layout grob) 'line-thickness))
               (new-stencil (ly:stencil-aligned-to
                 (ly:stencil-combine-at-edge
                   (ly:stencil-aligned-to stencil X CENTER)
                   Y dir
                   (ly:stencil-aligned-to
                     (grob-interpret-markup
                       grob
                       (make-fontsize-markup
                         (magnification->font-size
                           (+ (ly:staff-symbol-staff-space grob)
                              (/ staff-line-thickness 2)))
                           text)) X CENTER))
                 X LEFT))
               (staff-space (ly:output-def-lookup
                 (ly:grob-layout grob) 'staff-space))
               (par-x (ly:grob-parent grob X))
               (dyn-text (grob::has-interface par-x 'dynamic-text-interface))
               (dyn-text-stencil-x-length
                 (if dyn-text
                   (interval-length
                     (ly:stencil-extent (ly:grob-property par-x 'stencil) X))
                   0))
               (x-shift
                 (if dyn-text
                   (-
                     (+ staff-space dyn-text-stencil-x-length)
                     (* 0.5 staff-line-thickness)) 0)))

        (ly:grob-set-property! grob 'Y-offset 0)
        (ly:grob-set-property! grob 'stencil
           (ly:stencil-translate-axis
            new-stencil
            x-shift X))))
  #})

hairpinMolto =
\hairpinWithCenteredText \markup { \italic molto }

hairpinMore =
\hairpinWithCenteredText \markup { \larger moltissimo }

\layout { ragged-right = ##f }

\relative c' {
  \hairpinMolto
  c2\< c\f
  \hairpinMore
  c2\ppppp\< c\f
  \break
  \hairpinMolto
  c2^\< c\f
  \hairpinMore
  c2\ppppp\< c\f
}
