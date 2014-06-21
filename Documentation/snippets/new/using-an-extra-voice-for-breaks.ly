\version "2.18.0"

\header {
  lsrtags = "specific-notation, workaround"

  texidoc = "

Often it is easier to manage line and page-breaking information by
keeping it separate from the music by introducing an extra voice
containing only skips along with the @code{\break},
@code{pageBreak} and other layout information.

This pattern becomes especially helpful when overriding
@code{line-break-system-details} and the other useful but long
properties of @code{NonMusicalPaperColumnGrob}.
"
  doctitle = "Using an extra voice for breaks"
}

music = \relative c'' { c4 c c c }

\header { tagline = ##f }
\paper { left-margin = 0\mm }
\book {
  \score {
    \new Staff <<
      \new Voice {
        s1 * 2 \break
        s1 * 3 \break
        s1 * 6 \break
        s1 * 5 \break
      }
      \new Voice {
        \repeat unfold 2 { \music }
        \repeat unfold 3 { \music }
        \repeat unfold 6 { \music }
        \repeat unfold 5 { \music }
      }
    >>
  }
}
