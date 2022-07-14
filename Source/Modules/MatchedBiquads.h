/*
  ==============================================================================

    MatchedBiquads.h
    Author:  Martin Vicanek 2016
    C++ by StoneyDSP
    Created: 14 Jul 2022 8:13:32pm

  ==============================================================================
*/

// Match BiQuad Filter
// https://www.vicanek.de/articles/BiquadFits.pdf
// by Martin Vicanek 2016
// JSFX by TBProAudio 2019 

#pragma once

#ifndef MATCHEDBIQUADS_H_INCLUDED
#define MATCHEDBIQUADS_H_INCLUDED

#include <JuceHeader.h>
#include "Coefficient.h"

enum struct FilterType
{
    PeakEQ, 
    HighPass, 
    /*LowPass, */
    BandPass
    /*MPeakEQ, 
    MHighPass, 
    MLowPass, 
    MBandPass*/
};

enum struct TransformationType
{
    directFormI = 0,
    directFormII = 1,
    directFormItransposed = 2,
    directFormIItransposed = 3
};

template <typename SampleType>
class MatchedBiquad
{
public:
    MatchedBiquad();

    //==========================================================================
    /** Sets the centre Frequency of the filter. Range = 20..20000 */
    void setFrequency(SampleType newFreq);

    /** Sets the resonance of the filter. Range = 0..1 */
    void setResonance(SampleType newRes);

    /** Sets the centre Frequency gain of the filter. Peak and shelf modes only. */
    void setGain(SampleType newGain);

    /** Sets the BiLinear Transform for the filter to use. See enum for available types. */
    void setFilterType(FilterType newType);

    /** Sets the BiLinear Transform for the filter to use. See enum for available types. */
    void setTransformType(TransformationType newTransformType);

    //==========================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset(SampleType initialValue = { 0.0 });

    /** Ensure that the state variables are rounded to zero if the state
    variables are denormals. This is only needed if you are doing sample
    by sample processing.*/
    void snapToZero() noexcept;

    //==========================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        if (context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample((int)channel, inputSamples[i]);
        }

#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
        snapToZero();
#endif
    }

private:

    void coeffs();

    //==========================================================================
    /** Processes one sample at a time on a given channel. */
    SampleType processSample(int channel, SampleType inputSample);

    SampleType directFormI(int channel, SampleType inputValue);
    SampleType directFormII(int channel, SampleType inputValue);
    SampleType directFormITransposed(int channel, SampleType inputValue);
    SampleType directFormIITransposed(int channel, SampleType inputValue);

    Coefficient<SampleType> a0, a1, a2, b0, b1, b2;

    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    SampleType f, g, q, loop, outputSample, AA, f0, alfa;
    FilterType type = FilterType::LowPass;
    TransformationType transformType = TransformationType::directFormIItransposed;

    //==========================================================================
    /** Initialised constant */
    const SampleType zero = 0.0, one = 1.0, two = 2.0, minusOne = -1.0, minusTwo = -2.0;
    const SampleType pi = juce::MathConstants<SampleType>::pi;
    const SampleType root2 = juce::MathConstants<SampleType>::sqrt2;
    double sampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatchedBiquad)
};

#endif //MATCHEDBIQUADS_H_INCLUDED

/*

slider1:0 < 0, 7, 1{Peak EQ, High Pass, Low Pass, Band Pass, MPeak EQ, MHigh Pass, MLow Pass, MBand Pass} > Type
slider2 : 1000 < 1, 22049, 0.1 > Freq(Hz)
slider3 : 0 < -120, 120, 0.1 > Gain(dB)
slider4 : 0.7 < 0.01, 100, 0.01 > Q

@init
function cosh(z)
(
    (exp(z) + exp(-z)) * 0.5;
);


double x0nm1 = 0.0;
double x0nm2 = 0.0;
double x0n = 0.0;
double y0nm1 = 0.0;
double y0nm2 = 0.0;
double y0n = 0.0;

double x1nm1 = 0.0;
double x1nm2 = 0.0;
double x1n = 0.0;
double y1nm1 = 0.0;
double y1nm2 = 0.0;
double y1n = 0.0;

// EO@init


@slider
ftype = slider1;
f = slider2;
g = slider3;
q = slider4;

a0 = a1 = a2 = b0 = b1 = b2 = 0.0;

// Peaking EQ
(ftype == 0) ?
(
    AA = pow(10.0, g / 20.0);

f0 = f / (srate / 2.0);

// Poles
a0 = 1;
alfa = sin(f0 * $pi) / (2 * sqrt(AA) * q);
a1 = -2 * cos(f0 * $pi) / (1 + alfa);
a2 = (1 - alfa) / (1 + alfa);

// Zeros
b0 = (1 + AA * alfa) / (1 + alfa);
b1 = a1;
b2 = (1 - AA * alfa) / (1 + alfa);
) :
    // High Pass
    (ftype == 1) ?
    (
        f0 = f / (srate / 2.0);

// Poles
a0 = 1;
alfa = sin(f0 * $pi) / (2 * q);
a1 = -2 * cos(f0 * $pi) / (1 + alfa);
a2 = (1 - alfa) / (1 + alfa);

// Zeros
b0 = (1 - a1 + a2) / 4;
b1 = -2 * b0;
b2 = b0;
) :
    // Low Pass 
    (ftype == 2) ?
    (
        f0 = f / (srate / 2.0);

// Poles
a0 = 1;
alfa = sin(f0 * $pi) / (2 * q);
a1 = -2 * cos(f0 * $pi) / (1 + alfa);
a2 = (1 - alfa) / (1 + alfa);

// # Zeros
b0 = (1 + a1 + a2) / 4;
b1 = 2 * b0;
b2 = b0;

) :
    // Band Pass 
    (ftype == 3) ?
    (
        f0 = f / (srate / 2.0);

// Poles
a0 = 1.0;
alfa = sin(f0 * $pi) / (2 * q);
a1 = -2 * cos(f0 * $pi) / (1 + alfa);
a2 = (1 - alfa) / (1 + alfa);

// Zeros
b0 = (1 - a2) / 2;
b1 = 0;
b2 = -b0;
) :
    (ftype == 4) ? // MPeak EQ
    (
        AA = pow(10.0, g / 20.0);

f0 = f / (srate / 2.0);
// Poles
a0 = 1.0;
a2 = exp(-0.5 * $pi * f0 / (sqrt(AA) * q));
_test = 4 * AA * q * q;

(4 * AA * q * q > 1) ?   // complex conjugate poles
(
    a1 = -2 * a2 * cos(sqrt(1 - 1 / (4 * AA * q * q)) * $pi * f0);
) :                 // real poles
    (
        a1 = -2 * a2 * cosh(sqrt(1 / (4 * AA * q * q) - 1) * $pi * f0);
);
a2 = a2 ^ 2;

// Zeros
AA0 = (1.0 + a1 + a2) ^ 2;
AA1 = (1.0 - a1 + a2) ^ 2;
AA2 = -4 * a2;

phi1 = sin(0.5 * $pi * f0) ^ 2;
phi0 = 1.0 - phi1;
phi2 = 4 * phi0 * phi1;

r1 = (phi0 * AA0 + phi1 * AA1 + phi2 * AA2) * AA ^ 2;
r2 = (AA1 - AA0 + 4 * (phi0 - phi1) * AA2) * AA ^ 2;

BB0 = AA0;
BB2 = (r1 - phi1 * r2 - BB0) / (4 * phi1 ^ 2);
BB1 = r2 + BB0 + 4 * (phi1 - phi0) * BB2;

b1 = 0.5 * (1 + a1 + a2 - sqrt(BB1));
w = 1.0 + a1 + a2 - b1;
b0 = 0.5 * (w + sqrt(w ^ 2 + BB2));
b2 = -BB2 / (4 * b0);
):
(ftype == 5) ? // MHigh Pass
(
    f0 = f / (srate / 2.0);

// Poles
a0 = 1.0;
a2 = exp(-0.5 * $pi * f0 / q);
(2 * q > 1) ?   // complex conjugate poles
(
    a1 = -2 * a2 * cos(sqrt(1 - 1 / (4 * q * q)) * $pi * f0);
) :            // real poles
    (
        a1 = -2 * a2 * cosh(sqrt(1 / (4 * q * q) - 1) * $pi * f0);
);
a2 = a2 ^ 2;

//Zeros
AA0 = (1 + a1 + a2) ^ 2;
AA1 = (1 - a1 + a2) ^ 2;
AA2 = -4 * a2;

phi1 = sin(0.5 * $pi * f0) ^ 2;
phi0 = 1 - phi1;
phi2 = 4 * phi0 * phi1;

b0 = q * sqrt(phi0 * AA0 + phi1 * AA1 + phi2 * AA2) / (4 * phi1);
b1 = -2 * b0;
b2 = b0;
):
(ftype == 6) ?  // MLowPass
(
    f0 = f / (srate / 2.0);

// Poles
a0 = 1.0;
a2 = exp(-0.5 * $pi * f0 / q);
(2 * q > 1) ?   // complex conjugate poles
(
    a1 = -2 * a2 * cos(sqrt(1 - 1 / (4 * q * q)) * $pi * f0);
) :            // real poles
    (
        a1 = -2 * a2 * cosh(sqrt(1 / (4 * q * q) - 1) * $pi * f0)
        );
a2 = a2 ^ 2;

// Zeros
AA0 = (1 + a1 + a2) ^ 2;
AA1 = (1 - a1 + a2) ^ 2;
AA2 = -4 * a2;

phi1 = sin(0.5 * $pi * f0) ^ 2;
phi0 = 1 - phi1;
phi2 = 4 * phi0 * phi1;

r1 = (AA0 * phi0 + AA1 * phi1 + AA2 * phi2) * q ^ 2;

BB1 = (r1 - AA0 * phi0) / phi1;

b0 = 0.5 * (sqrt(BB1) + 1 + a1 + a2);
b1 = 1 + a1 + a2 - b0;
b2 = 0;
):
(ftype == 7) ? // MBandPass
(
    f0 = f / (srate / 2.0);

// Poles
a0 = 1.0;
a2 = exp(-0.5 * $pi * f0 / q);
(2 * q > 1) ? // complex conjugate poles
(
    a1 = -2 * a2 * cos(sqrt(1 - 1 / (4 * q * q)) * $pi * f0);
) :          // real poles
    (
        a1 = -2 * a2 * cosh(sqrt(1 / (4 * q * q) - 1) * $pi * f0);
);
a2 = a2 ^ 2;

// Zeros
AA0 = (1 + a1 + a2) ^ 2;
AA1 = (1 - a1 + a2) ^ 2;
AA2 = -4 * a2;

phi1 = sin(0.5 * $pi * f0) ^ 2;
phi0 = 1 - phi1;
phi2 = 4 * phi0 * phi1;

r1 = phi0 * AA0 + phi1 * AA1 + phi2 * AA2;
r2 = AA1 - AA0 + 4 * (phi0 - phi1) * AA2;

BB2 = (r1 - phi1 * r2) / (4 * phi1 * phi1);
BB1 = r2 + 4 * (phi1 - phi0) * BB2;

b1 = -0.5 * sqrt(BB1);
b0 = 0.5 * (sqrt(b1 * b1 + BB2) - b1);
b2 = -b0 - b1;
);

////////////////////////////

// EO@slider

@sample

// spl0
x0n = spl0;

y0n = (b0 * x0n + b1 * x0nm1 + b2 * x0nm2 - a1 * y0nm1 - a2 * y0nm2) / a0;

x0nm2 = x0nm1;
x0nm1 = x0n;

y0nm2 = y0nm1;
y0nm1 = y0n;

spl0 = y0n;

// spl1
x1n = spl1;

y1n = (b0 * x1n + b1 * x1nm1 + b2 * x1nm2 - a1 * y1nm1 - a2 * y1nm2) / a0;

x1nm2 = x1nm1;
x1nm1 = x1n;

y1nm2 = y1nm1;
y1nm1 = y1n;

spl1 = y1n;

// EO@sample

*/
