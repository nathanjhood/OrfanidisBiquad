/*
  ==============================================================================

    Transformations.cpp
    Created: 7 May 2022 4:36:01am
    Author:  StoneyDSP

  ==============================================================================
*/

#include "Transformations.h"

//==============================================================================
template <typename SampleType>
Transformations<SampleType>::Transformations()
{

}

//==============================================================================
template <typename SampleType>
void Transformations<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    xn_1.resize(spec.numChannels);
    xn_2.resize(spec.numChannels);
    yn_1.resize(spec.numChannels);
    xn_2.resize(spec.numChannels);
}

template <typename SampleType>
void Transformations<SampleType>::reset()
{

}

//==============================================================================
template class Transformations<float>;
template class Transformations<double>;