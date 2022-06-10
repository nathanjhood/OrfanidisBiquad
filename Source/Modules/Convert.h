/*
  ==============================================================================

    Convert.h
    Created: 8 May 2022 6:21:33am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef CONVERT_H_INCLUDED
#define CONVERT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

template <typename SampleType>
class Conversion
{
public:
    //==============================================================================
    /** Constructor. */
    Conversion();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    SampleType freqToHz(SampleType newFreq);

    SampleType hzToRadPerSamp(SampleType newHz);

    //==============================================================================
    void setG(SampleType newG) { G = newG; };

    void setGB(SampleType newGB) { GB = newGB; };

    void setw0(SampleType neww0) { w0 = neww0; };

    void setDw(SampleType newDw) { Dw = newDw; };

    //==============================================================================
    SampleType getG() const noexcept { return G; };

    SampleType getGB() const noexcept { return GB; };

    SampleType getw0() const noexcept { return w0; };

    SampleType getDw() const noexcept { return Dw; };

    //==============================================================================
    void calculate(SampleType gain, SampleType frequency, SampleType q)
    {
        const SampleType boostCut = juce::Decibels::decibelsToGain(gain);
        const SampleType bwGain = juce::Decibels::decibelsToGain(gain / (std::sqrt(SampleType(2.0))));
        const SampleType hzFrequency = freqToHz(frequency);
        const SampleType freqRads = hzToRadPerSamp(hzFrequency);
        const SampleType bwRads = freqRads / (SampleType(1.588308819) * q);

        setG(boostCut);
        setGB(bwGain);
        setw0(freqRads);
        setDw(bwRads);
    }

private:
    //==============================================================================
    double sampleRate = 44100.0;
    SampleType G = 0, GB = 0, w0 = 0, Dw = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Conversion)
};

#endif // CONVERT_H_INCLUDED