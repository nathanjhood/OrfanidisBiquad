/*
  ==============================================================================

    Convert.cpp
    Created: 8 May 2022 6:21:33am
    Author:  StoneyDSP

  ==============================================================================
*/

#include "Convert.h"

//==============================================================================
template <typename SampleType>
Conversion<SampleType>::Conversion()
{

}

template <typename SampleType>
void Conversion<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);

    sampleRate = spec.sampleRate;
}

template <typename SampleType>
void Conversion<SampleType>::reset()
{

}

template <typename SampleType>
SampleType Conversion<SampleType>::freqToHz(SampleType newFreq)
{
    jassert (SampleType(20.0) <= newFreq && newFreq <= SampleType(20000.0));

    const SampleType minFreq = static_cast <SampleType>(sampleRate) / SampleType(24576.0);
    const SampleType maxFreq = static_cast <SampleType>(sampleRate) / SampleType(2.125);

    return jlimit(minFreq, maxFreq, newFreq);
}

template <typename SampleType>
SampleType Conversion<SampleType>::hzToRadPerSamp(SampleType newHz)
{
    jassert(0 <= newHz && newHz <= ((sampleRate) / SampleType(2.0)));
    return newHz * ((SampleType(2.0) * (juce::MathConstants<SampleType>::pi)) / static_cast <SampleType>(sampleRate));
}

template class Conversion<float>;
template class Conversion<double>;