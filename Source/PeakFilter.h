/*
  ==============================================================================

    PeakBand.h
    Created: 7 May 2022 5:10:34am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef PEAKFILTER_H_INCLUDED
#define PEAKFILTER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

template <typename SampleType>
class PeakFilter
{
public:

    //==============================================================================
    /** Constructor. */
    PeakFilter();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

private:

    double sampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PeakFilter)
};

#endif //PEAKFILTER_H_INCLUDED
