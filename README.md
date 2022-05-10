# OrfanidisBiquad
Audio EQ Peak Band with no digital "cramping", and selectable transformations (Direct Form I & II, plus Direct Form I & II transposed).

(Shown below; a +30dB bell-shaped boost at 10kHz, performed on a harmonic-rich 20Hz sawtooth-wave in Reaper)

![Orfanidis Biquad](Res/OrfanidisBiquad.png)


(write-up and comparisons to follow)

Reference: Original paper (including MATLAB code) "Digital Parametric Equalizer Design With Prescribed
Nyquist-Frequency Gain", Sophocles J. Orfanidis, 1996; https://www.ece.rutgers.edu/~orfanidi/ece348/peq.pdf


# Transformations
Determining an output transfer function (Y(z)), given an input value (X(z)) and six multiplier coefficients within an audio feedback path (b0, b1, b2, a1, and a2 - all of which are pre-scaled by 1/a0) - please note that each feedback term requires a delay of one audio sample;

+ X(z) = input sample

+ Y(z) = output sample

+ b0, b1, etc.. = coefficient gain multiplier

+ (+) = summing point (addition)

+ z-1 = unit (single sample) delay

+ <-, ->.. etc = signal flow direction 

![DF I](https://github.com/StoneyDSP/OrfanidisBiquad/blob/0a9c1168752616b455d68b52a2b0b841102dfa16/Res/400px-Biquad_filter_DF-I.svg.png)

 Some forms have what may be considered an additional audio feedback path (notated as W(z) in this description, but may vary);

![DF II](https://github.com/StoneyDSP/OrfanidisBiquad/blob/0a9c1168752616b455d68b52a2b0b841102dfa16/Res/Biquad_filter_DF-IIx.svg.png)


# Creating the transformations - getting started;

First, we need to create an input (Xn), and output (Yn), and our 6 coefficients for gain multiplication, the results of which are summed together using linear addition;

    {
    
    Xn = input sample;
    
    b0 = 1;
    b1 = 0;
    b2 = 0;
    a0 = 1;
    a1 = 0;
    a2 = 0;
    
    a0_ = (1 / a0);
    
    -a1_ = (-1 * (a1 * a0_));
    -a2_ = (-1 * (a2 * a0_));
    
    b0_ = (b0 * a0_);
    b1_ = (b1 * a0_);
    b2_ = (b2 * a0_);
    
    Yn = ((Xn * b0_) + (Xn * b1_) + (Xn * b2_) + (Xn * -a1_) + (Xn * -a2_));
    
    return Yn;
    
    }

(please note that extra care is taken where necessary within the code to ensure thread safety between parameter (message thread) and playback (audio thread).)

I have also re-created all steps in my preferred visual-programming/workbench environment, Reaktor Core, which I have chosen to share for here the very same reason I re-created this code within said program; ease of readability;

![Workbench](https://github.com/StoneyDSP/OrfanidisBiquad/blob/0a9c1168752616b455d68b52a2b0b841102dfa16/Res/Workbench%20-%20Bypass%20(coded%20by%20StoneyDSP).png)

For any readers unfamiliar with Reaktor Core, please keep in mind that signal flows from left (input) to right (output). In addition to the math operators connecting inputs to outputs, we have a few macros that may raise queries - this symbol legend may help fill in a few blanks;

![legend](https://github.com/StoneyDSP/OrfanidisBiquad/blob/0a9c1168752616b455d68b52a2b0b841102dfa16/Res/Workbench%20-%20Legend%20(coded%20by%20StoneyDSP).png)

Now I shall use a combination of visuals and pseudo-code to re-create and further investigate our various transformations available within the test plugin.

# Creating the transformations - Direct Form I;

Direct Form I is characterized by having a total four unit delays present within it's architecture, with two coefficients (a1 and a2) arranged as to control negative feedback gain, and the remaining three (b0 to b2) controlling positive feedback gain, with all feedback terms summed (added) together in a simple linear fashion;  

![Direct Form I calc](https://github.com/StoneyDSP/OrfanidisBiquad/blob/79913795b69fea84185d890bc17b1998918f8e5e/Res/DFI.svg)

![Direct Form I](Res/496px-Digital_Biquad_Direct_Form_1_Untransformed.svg.png)

![Direct Form I core](https://github.com/StoneyDSP/OrfanidisBiquad/blob/3170a60ffa2696ae42f426c74b20188f12361c36/Res/Workbench%20-%20DFI%20(coded%20by%20Native%20Instruments).png)

    {
    
    Xn = inputValue;

    Yn = ((Xn * b0) + (Xn(z-1) * b1) + (Xn(z-2) * b2) + (Yn(z-1) * -a1) + (Yn(z-2) * -a2));
    
    Xn(z-2) = Xn(z-1);
    Xn(z-1) = Xn;
    
    Yn(z-2) = Yn(z-1);
    Yn(z-1) = Yn;

    return Yn;
    
    }

# Creating the transformations - Direct Form II;

Direct Form II (also known as the "canonical" form, at least of the two discussed thus far) uses the same arrangement of coefficents, but only two unit delays - it also has what may be viewed as a "second" feedback path, here denoted as W(n);

![Direct Form II calc W](https://github.com/StoneyDSP/OrfanidisBiquad/blob/79913795b69fea84185d890bc17b1998918f8e5e/Res/DFII%20w.svg)

![Direct Form II calc Y](https://github.com/StoneyDSP/OrfanidisBiquad/blob/79913795b69fea84185d890bc17b1998918f8e5e/Res/DFII%20y.svg)

![Direct Form II](Res/496px-Digital_Biquad_Direct_Form_2_Untransformed.svg.png)

![Direct Form II core](https://github.com/StoneyDSP/OrfanidisBiquad/blob/8cab54019b024ef532892ee12846403297755c02/Res/Workbench%20-%20DFII%20(coded%20by%20StoneyDSP).png)

    {
    
    Xn = input sample;
    
    Wn = (Xn + ((Wn(z-1) * -a1) + (Wn(z-2) * -a2)));
    Yn = ((Wn * b0) + (Wn(z-1) * b1) + (Wn(z-2) * b2));

    Wn(z-2) = Wn(z-1);
    Wn(z-1) = Wn;

    return Yn;
    
    }
    
# Creating the transformations - Direct Form I Transposed;

For the "transposed" forms, all terms are inverted (signal flow reversed, summing points become split points, and multpiliers swap positions), creating the same output transfer function for the same number of components but in a somewhat "mirrored" directional flow of our input signal, resulting in our coefficient multiplactions occuring before the unit delays;

![Direct Form IT](Res/496px-Digital_Biquad_Direct_Form_1_Transformed.svg.png)

![Direct Form I transposed core](https://github.com/StoneyDSP/OrfanidisBiquad/blob/5bd7d03001aa5e90b2c92a29a909a4f51e0d9367/Res/Workbench%20-%20DFI%20transposed%20(coded%20by%20StoneyDSP).png)

    {
    
    Xn = input sample;
    
    Wn = (Xn + Wn(z-2));
    Yn = ((Wn * b0) + Xn(z-2);
    
    Xn(z-2) = ((Wn * b1) + Xn(z-1));
    Xn(z-1) = (Wn * b2);
    
    Wn(z-2) = ((Wn * a1) + Wn(z-1));
    Wn(z-1) = (Wn * a2);
    
    return Yn;
    
    }
    
# Creating the transformations - Direct Form II Transposed;

Direct Form II transposed only requires the two unit delays (like it's non-transposed counterpart), as opposed to the four of the Direct Form I (both counterparts), and likewise features it's multiplcation coefficients happening before the unit delays occur;

![Direct Form IIT](Res/496px-Digital_Biquad_Direct_Form_2_Transformed.svg.png)
    
![Direct Form II transposed core](https://github.com/StoneyDSP/OrfanidisBiquad/blob/5bd7d03001aa5e90b2c92a29a909a4f51e0d9367/Res/Workbench%20-%20DFII%20transposed%20(coded%20by%20StoneyDSP).png)

    {
    
    Xn = input sample;
    
    Yn = ((Xn * b0) + (Xn(z-2));
    
    Xn(z-2) = ((Xn * b1) + (Xn(z-1)) + (Yn * a1));
    Xn(z-1) = ((Xn * b2) + (Yn * a2));
    
    return Yn;
    
    }
    
# Observations on the various Direct Form topologies in real-time audio use-cases;

As depicted in the above diagrams, the Direct Form I ("DFI") and II ("DFII") utlize a chain of single-sample unit delays in a feedback arrangement, with the coefficients a1 through to b0 controlling the gain at various points in the feedback network (in the case of DFII, actually two feedback networks).

+ DFI utlizes a total of four samples of delay ("z-1"), with DFII requiring only two samples. The higher number of unit delays present in the DFI structure make this arrangement relatively unstable when modulating the parameters while simultaneously passing audio, resulting in loud (and potentially damaging) clicks and pops in the resulting audio. In our test workbench (running as a VST3 effect in Reaper), even just moderate sweeps of the filter frequency control can incur signal overloads significant enough to trigger the in-built "channel auto-mute" safety feature, which avoids sending potentially damaging signals to the audio playback device and speakers.

+ DFII, using less unit delays in it's architecture, produces much less significant artefacts during parameter modulation; in all but the most extreme cases, the output remains relatively benign. However, this structure is far more prone to "round-off" errors due to a narrowing computational precision in certain parts of the feedback network; this can manifest as a kind of "quantization noise" - much like un-dithered fixed-point audio - creeping well into the audible range, and in some cases enveloping low-amplitudinal parts of the input signal. This can be particularly extenuated by very large boosts of a tight "bell" shape in the lowest bass frequencies, causing strong quantization-error noise to permeate the upper-mid and treble ranges of the signal (image to follow).

The two "transposed" forms provide us the same output characteristics for the same number of components, but arranged in inverse terms as compared with the non-transposed forms; summing points become split points, gain multpiliers swap positions within the network, and the entire signal flow is reversed (our images are also flipped around to keep the input and output directions visually aligned with the previous structures). The results are interesting; 

+ The transposed Direct Form I ("DFI(t)") utilizes the four unit-delays of it's predecessor, meaning instability while passing audio during parameter modulation, yet also incurs the exact same "round-off error" and quantization noise as the original DFII structure.

+ The Transposed Direct Form II, on the other hand, similarly uses only two unit-delays as per it's predecessor, making it much more amenable to audio-rate modulation; meanwhile, this form also successfully manages to avoid the higher "round-off" error and quantization noise of it's predecessor (and the DFI(t) structure).

(images to follow)
    
# With all four arrangements compared;

+ DFI = four delay units (unstable modulation and higher footprint), higher precision (less quantization noise)
+ DFII =  two delay units (stable modulation and lower footprint), lower precision (more quantization noise)
+ DFI(t) = four delay units (unstable modulation and higher footprint), lower precision (more quantization noise)
+ DFII(t) = two delay units (stable modulation and lower footprint), higher precision (less quantization noise)

As we can observe from the above comparison, our DFII(t) structure is the most favourable in both cases - it has only two delay units, meaning it is more stable under modulation, favourably comparable to the DFII structure; it also produces less quantization noise, comparable to the DFI structure in this regard. The lower unit delay count also produces a lower memory footprint in realtime use.

Our DFI(t) structure performs the most poorly in the two highlighted cases - the quantization round-off error is unfavourably comparable to the DFII structure, while the four delay units contribute to heavy and unpredictable click/pop "zipper" noise underparameter modulation and also a higher memory footprint.

A parameter smoother may be applied in a future version. In testing so far, it has been observed that a smoothed parameter, derived from a clock source that is likely asynchronous to the audio buffer clock, generates an unusual notch-like filter effect on the resulting audio spectrum; the deterministic frequency of this "notch-like" effect is seemingly related in periodicity to the parameter smoother's clock speed.

The quantization noise created by the feedback network's computational round-off errors can be seemingly entirely negated by increasing processing precision from Floats to Doubles; the resulting noise floor falls not only well below the audible threshold, but also below the reach of abilities of our testing software.

Furthermore, increasing precision to Doubles also seemingly eradicates the "notch-like periodicity" that our parameter smoother incurrs on the waveform to imperceptible levels.

However, out of sight and out of mind does not mean out the window; we are able to produce several very pronounced audible artefacts in three of the four structures when processing in Floats (commonly deemed to be a beyond acceptable processing precision for audio purposes, to be debated elsewhere). Indeed only the Transposed Direct Form II manages favourably in all cases, and thus appears to be the prime candidate transformation for Biquad-based Equalizers in all audio application contexts at the time of writing.

^ Credit: Native Instruments for the Direct Form I code (taken from Reaktor 5's Core "Static Filter" library - go figure!) as well as the Core library unit delay, audio thread, and math modulation macros used here.

^^ Reference: Transformations images taken from: https://en.wikipedia.org/wiki/Digital_biquad_filter
