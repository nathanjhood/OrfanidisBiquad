# OrfanidisBiquad
Audio EQ Peak Band with no digital "cramping", and selectable transformations (Direct Form I & II, plus Direct Form I & II transposed).

(Shown below; a +30dB bell-shaped boost at 10kHz, performed on a harmonic-rich 20Hz sawtooth-wave in Reaper)

![Orfanidis Biquad](Res/OrfanidisBiquad.png)


Reference: Original paper (including MATLAB code) "Digital Parametric Equalizer Design With Prescribed
Nyquist-Frequency Gain", Sophocles J. Orfanidis, 1996; https://www.ece.rutgers.edu/~orfanidi/ece348/peq.pdf


# Transformations
Determing an output transfer function (Y), given an input value (X) and six coefficients (b0, b1, b2, a1, and a2, all of which are pre-scaled by 1/a0);

Direct Form I;

![Direct Form I](Res/496px-Digital_Biquad_Direct_Form_1_Untransformed.svg.png)

Direct Form II;

![Direct Form II](Res/496px-Digital_Biquad_Direct_Form_2_Untransformed.svg.png)

Direct Form I Transposed;

![Direct Form IT](Res/496px-Digital_Biquad_Direct_Form_1_Transformed.svg.png)

Direct Form II Transposed;

![Direct Form IIT](Res/496px-Digital_Biquad_Direct_Form_2_Transformed.svg.png)


Reference: Transformations taken from: https://en.wikipedia.org/wiki/Digital_biquad_filter
