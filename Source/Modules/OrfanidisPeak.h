/*
  ==============================================================================

    OrfanidisPeak.h
    Created: 10 Jun 2022 1:13:19am
    Author:  Nathan J. Hood - adapted from:

    peq.m - Parametric EQ with matching gain at Nyquist frequency
    Sophocles J. Orfanidis, J. Audio Eng. Soc., vol.45, p.444, June 1997.
    https://www.ece.rutgers.edu/~orfanidi/ece348/peq.pdf

    Released under GNU General Public License v3.0

  ==============================================================================
*/

#pragma once

#ifndef ORFANIDISPEAK_H_INCLUDED
#define ORFANIDISPEAK_H_INCLUDED

#include <JuceHeader.h>
#include "Coefficient.h"

enum class TransformationType
{
    directFormI = 0,
    directFormII = 1,
    directFormItransposed = 2,
    directFormIItransposed = 3
};

template <typename SampleType>
class OrfanidisPeak
{
public:
    using transformationType = TransformationType;
    //==========================================================================
    /** Constructor. */
    OrfanidisPeak();

    //==========================================================================
    /** Sets the centre Frequency of the filter. Range = 20..20000 */
    void setFrequency(SampleType newFreq);

    /** Sets the resonance of the filter. Range = 0..1 */
    void setResonance(SampleType newRes);

    /** Sets the centre Frequency gain of the filter. Peak and shelf modes only. */
    void setGain(SampleType newGain);

    /** Sets the BiLinear Transform for the filter to use. See enum for available types. */
    void setTransformType(transformationType newTransformType);

    //==========================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset(SampleType initialValue = {0.0});

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

    //==========================================================================
    /** Processes one sample at a time on a given channel. */
    SampleType processSample(int channel, SampleType inputValue);

private:
    //==========================================================================
    void coefficients();
    void calcs();

    //==========================================================================
    SampleType directFormI(int channel, SampleType inputValue);
    SampleType directFormII(int channel, SampleType inputValue);
    SampleType directFormITransposed(int channel, SampleType inputValue);
    SampleType directFormIITransposed(int channel, SampleType inputValue);

    //==========================================================================
    /** Coefficient current value. Safe to pass i.e. to the display thread */
    SampleType geta0() { return a[0]; }
    SampleType getb0() { return b[0]; }
    SampleType geta1() { return a[1]; }
    SampleType getb1() { return b[1]; }
    SampleType geta2() { return a[2]; }
    SampleType getb2() { return b[2]; }

    //==============================================================================
    /** Unit-delay objects. */
    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    //==========================================================================
    /** Coefficient gain */
    Coefficient<SampleType> a[3], b[3];

    /** Coefficient calculation */
    SampleType a_[3];
    SampleType b_[3];

    Coefficient<SampleType> G0, G, GB, w0, Dw;

    //==========================================================================
    /** Initialised parameter */
    SampleType frq, res, gain;
    SampleType loop, outputSample, omega, minFreq, maxFreq;
    transformationType transformType;

    SampleType D, C, B, A, G1;
    SampleType G0W2, onePlusW2A;
    SampleType num, den;

    //==========================================================================
    /** Initialised constant */
    const SampleType zero = 0.0, one = 1.0, two = 2.0, minusOne = -1.0, minusTwo = -2.0;
    const SampleType pi = juce::MathConstants<SampleType>::pi;
    const SampleType root2 = juce::MathConstants<SampleType>::sqrt2;
    double sampleRate = 44100.0;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrfanidisPeak)
};

#endif //ORFANIDISPEAK_H_INCLUDED
