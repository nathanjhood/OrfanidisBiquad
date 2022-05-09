/*
  ==============================================================================

    ProcessorWrapper.cpp
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#include "ProcessorWrapper.h"

template <typename SampleType>
ProcessWrapper<SampleType>::ProcessWrapper(juce::AudioProcessorValueTreeState& apvts)
{
    ioPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("ioID"));
    jassert(ioPtr != nullptr);

    gainPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("gainID"));
    jassert(gainPtr != nullptr);

    freqPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("freqID"));
    jassert(freqPtr != nullptr);

    bandPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("bandID"));
    jassert(bandPtr != nullptr);
}

template <typename SampleType>
void ProcessWrapper<SampleType>::createParameterLayout(std::vector<std::unique_ptr<RangedAudioParameter>>& params)
{
    auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.01f, 00.198894f);
    auto gainRange = juce::NormalisableRange<float>(-30.00f, 30.00f, 0.01f, 1.00f);
    
    params.push_back(std::make_unique<juce::AudioParameterBool>("ioID", "IO", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("freqID", "Freq", freqRange, 632.45f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bandID", "Bandwidth", 00.10f, 01.00f, 00.10f));
}

template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    transform.prepare(spec);
    reset();
};

template <typename SampleType>
void ProcessWrapper<SampleType>::reset() 
{
    transform.reset(SampleType(0.0));
};

template <typename SampleType>
void ProcessWrapper<SampleType>::transType(TransformationType newTransform)
{
    if (typePtr != newTransform)
    {
        typePtr = newTransform;
        reset();
    }
}

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    convert.calculate(gainPtr->get(), freqPtr->get(), bandPtr->get());

    coeffs.calculateCoefficients(SampleType(1.0), convert.getG(), convert.getGB(), convert.getw0(), convert.getDw());

    transform.coefficients(coeffs.b0(), coeffs.b1(), coeffs.b2(), coeffs.a0(), coeffs.a1(), coeffs.a2());

    transform.setTransformType(typePtr);
};

//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;