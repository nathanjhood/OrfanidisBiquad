/*
  ==============================================================================

    Transformations.h
    Created: 7 May 2022 4:36:01am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef TRANSFORMATIONS_H_INCLUDED
#define TRANSFORMATIONS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

template <typename SampleType>
class Transformations
{
public:

    //==============================================================================
    /** Constructor. */
    Transformations();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

private:

    double sampleRate = 44100.0;

    std::vector<SampleType> xn_1;
    std::vector<SampleType> xn_2;
    std::vector<SampleType> yn_1;
    std::vector<SampleType> yn_2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transformations)
};

#endif // TRANSFORMATIONS_H_INCLUDED
