\header {
    
    texidoc = "newlyrics, multiple stanzas, multiple lyric voices."
    
}

%%\new PianoStaff <<
 <<
    \new Staff \relative {
	d'2 d c4 bes a2 \break
	c2 c d4 f g2
    }
    \newlyrics {
	My first Li -- ly song,
	Not much can go wrong!
    }
    \newlyrics {
	My next Li -- ly verse
	Not much can go wrong!
    }
    \new Staff \relative {
	\clef bass
	d2 d c4 bes a2 \break
	c2 c d4 f g2
    }
    \newlyrics {
	MY FIRST LI -- LY SONG,
	NOT MUCH CAN GO WRONG!
    }
    \newlyrics {
	My next Li -- ly verse
	Not much can go wrong!
    }
>>

\version "2.3.0"
