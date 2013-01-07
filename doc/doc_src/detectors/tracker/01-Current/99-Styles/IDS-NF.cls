\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{IDS-NF}[2012/04/27]
\let\ids@startsection\@startsection
\LoadClass[a4paper,fleqn,preprintnumbers,nofootinbib,PRD,11pt]{revtex4-1}
%-- YU: [prl,12pt,tightenlines]
\renewcommand\thesection{\arabic{section}}
\renewcommand\p@section{}
\renewcommand\thesubsection{\thesection.\arabic{subsection}}
\renewcommand\p@subsection{}
\renewcommand\thesubsubsection{\thesubsection.\arabic{subsubsection}}
\renewcommand\p@subsubsection{}
\renewcommand\theparagraph{\thesubsubsection.\arabic{paragraph}}
\renewcommand\p@paragraph{}
\renewcommand\thesubparagraph{\theparagraph.\arabic{subparagraph}}
\renewcommand\p@subparagraph{}
\newcommand\ids@section[2]{\ids@startsection{#1}{#2}{\z@}{-0.8cm \@plus-0.2cm \@minus-0.1cm}{0.5cm}{\normalfont\large\bfseries}}
\renewcommand\section{\ids@section{section}{1}}
\renewcommand\subsection{\ids@section{subsection}{2}}
\renewcommand\subsubsection{\ids@section{subsubsection}{3}}
\renewcommand\paragraph{\ids@section{paragraph}{4}}
\renewcommand\subparagraph{\ids@section{subparagraph}{5}}
\renewcommand\l@subsubsection[2]{}
\setcounter{tocdepth}{2}
\setcounter{secnumdepth}{5}
\newlength\ids@border
\setlength\ids@border{20mm}
\setlength\paperheight{297mm}
\setlength\paperwidth{210mm}
\special{papersize=210mm,297mm}
\setlength\hoffset{\ids@border}
\addtolength\hoffset{-1in}
\setlength\oddsidemargin{0mm}
\setlength\evensidemargin{\oddsidemargin}
\setlength\textwidth{\paperwidth}
\addtolength\textwidth{-2\ids@border}
\setlength\voffset{\ids@border}
\addtolength\voffset{-1in}
\setlength\topmargin{0mm}
\setlength\headheight{0mm}
\setlength\headsep{0mm}
\setlength\footskip{13mm}
\setlength\textheight{\paperheight}
\addtolength\textheight{-2\ids@border}
\addtolength\textheight{-\footskip}
\newlength\ids@capwidth
\long\def\@makecaption#1#2{%
  \par
  \vskip\abovecaptionskip
  \begingroup
   \small\rmfamily
   \setlength\ids@capwidth{\textwidth}
   \addtolength\ids@capwidth{-4em}
   \sbox\@tempboxa{%
    \let\\\heading@cr
    \@make@capt@title{#1}{#2}%
   }%
   \@ifdim{\wd\@tempboxa >\ids@capwidth}{%
    \begingroup
     \samepage
     \begin{center}
       \begin{minipage}{\ids@capwidth}
         \flushing
         \let\footnote\@footnotemark@gobble
         \@make@capt@title{#1}{#2}\par
       \end{minipage}
     \end{center}
    \endgroup
   }{%
     \global \@minipagefalse
     \hb@xt@\hsize{\hfil\unhbox\@tempboxa\hfil}%
   }%
  \endgroup
  \vskip\belowcaptionskip
}%
\leftmargini   28pt
\leftmarginii  18pt
\leftmarginiii 18pt
\leftmarginiv  18pt
\leftmarginv   18pt
\leftmarginvi  18pt
\renewcommand\baselinestretch{1.1}
\renewcommand\@listI{\leftmargin\leftmargini\parsep0pt plus 1pt\itemsep\parsep\topsep\parsep}
\renewcommand\@listii{\leftmargin\leftmarginii\parsep0pt plus 1pt\itemsep\parsep\topsep\parsep}
\renewcommand\@listiii{\leftmargin\leftmarginiii\parsep0pt plus 1pt\itemsep\parsep\topsep\parsep}
\renewcommand\@listiv{\leftmargin\leftmarginiv\parsep0pt plus 1pt\itemsep\parsep\topsep\parsep}
\renewcommand\@listv{\leftmargin\leftmarginv\parsep0pt plus 1pt\itemsep\parsep\topsep\parsep}
\renewcommand\@listvi{\leftmargin\leftmarginvi\parsep0pt plus 1pt\itemsep\parsep\topsep\parsep}
