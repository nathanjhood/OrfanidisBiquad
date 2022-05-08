/*
  ==============================================================================

    PeakBand.cpp
    Created: 7 May 2022 5:10:34am
    Author:  StoneyDSP

  ==============================================================================
*/

#include "PeakFilter.h"

//==============================================================================
template <typename SampleType>
PeakFilter<SampleType>::PeakFilter()
{

}

//==============================================================================
template <typename SampleType>
void PeakFilter<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;
}

template <typename SampleType>
void PeakFilter<SampleType>::reset()
{

}

//==============================================================================
template class PeakFilter<float>;
template class PeakFilter<double>;
