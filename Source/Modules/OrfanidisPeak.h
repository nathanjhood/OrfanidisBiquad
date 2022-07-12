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
    //==============================================================================
    /** Constructor. */
    OrfanidisPeak();

    //==============================================================================
    /** Sets the centre Frequency of the filter. Range = 20..20000 */
    void setFrequency(SampleType newFreq);

    /** Sets the resonance of the filter. Range = 0..1 */
    void setResonance(SampleType newRes);

    /** Sets the centre Frequency gain of the filter. Peak and shelf modes only. */
    void setGain(SampleType newGain);

    /** Sets the BiLinear Transform for the filter to use. See enum for available types. */
    void setTransformType(transformationType newTransformType);

    //==============================================================================
    /** Sets the length of the ramp used for smoothing parameter changes. */
    void setRampDurationSeconds(double newDurationSeconds) noexcept;

    /** Returns the ramp duration in seconds. */
    double getRampDurationSeconds() const noexcept;

    /** Returns true if the current value is currently being interpolated. */
    bool isSmoothing() const noexcept;

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset(SampleType initialValue);

    /** Ensure that the state variables are rounded to zero if the state
    variables are denormals. This is only needed if you are doing sample
    by sample processing.*/
    void snapToZero() noexcept;

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        const auto len = inputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        if (context.isBypassed)
        {
            frq.skip(static_cast<int> (len));
            res.skip(static_cast<int> (len));
            lev.skip(static_cast<int> (len));

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

    //==============================================================================
    /** Processes one sample at a time on a given channel. */
    SampleType processSample(int channel, SampleType inputValue);

private:
    //==============================================================================
    void coefficients();

    SampleType directFormI(int channel, SampleType inputValue);

    SampleType directFormII(int channel, SampleType inputValue);

    SampleType directFormITransposed(int channel, SampleType inputValue);

    SampleType directFormIITransposed(int channel, SampleType inputValue);

    //==========================================================================
    /** Coefficient current value. Safe to pass i.e. to the display thread */
    SampleType geta0() { return a0.get(); }
    SampleType getb0() { return b0.get(); }
    SampleType geta1() { return a1.get(); }
    SampleType getb1() { return b1.get(); }
    SampleType geta2() { return a2.get(); }
    SampleType getb2() { return b2.get(); }

    //==============================================================================
    /** Unit-delay objects. */
    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    //==========================================================================
    /** Coefficient gain */
    Coefficient<SampleType> b0, b1, b2, a0, a1, a2;

    /** Coefficient calculation */
    Coefficient<SampleType> b_0, b_1, b_2, a_0, a_1, a_2;

    //==============================================================================
    /** Parameter Smoothers. */
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Multiplicative> frq;
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear> res;
    juce::SmoothedValue<SampleType, juce::ValueSmoothingTypes::Linear> lev;

    //==============================================================================
    /** Initialise the parameters. */
    double sampleRate = 44100.0, rampDurationSeconds = 0.00005;
    SampleType minFreq = 20.0, maxFreq = 20000.0, hz = 1000.0, q = 0.5, g = 0.0;
    transformationType transformType = transformationType::directFormIItransposed;

    //==========================================================================
    /** Initialised parameter */
    SampleType loop = 0.0, outputSample = 0.0;
    SampleType minFreq = 20.0, maxFreq = 20000.0, hz = 1000.0, q = 0.5, g = 0.0;
    transformationType transformType = transformationType::directFormIItransposed;

    SampleType omega, cos, sin, tan, alpha, a, sqrtA{ 0.0 };

    //==========================================================================
    /** Initialised constant */
    const SampleType zero = 0.0, one = 1.0, two = 2.0, minusOne = -1.0, minusTwo = -2.0;
    const SampleType pi = juce::MathConstants<SampleType>::pi;
    double sampleRate = 48000.0;
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrfanidisPeak)
};

#endif //ORFANIDISPEAK_H_INCLUDED
