/*
  ==============================================================================

    ProcessorWrapper.cpp
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#include "PluginWrapper.h"
#include "PluginProcessor.h"

template <typename SampleType>
ProcessWrapper<SampleType>::ProcessWrapper(OrfanidisBiquadAudioProcessor& p) 
    :
    audioProcessor(p),
    state(p.getAPVTS()),
    setup(p.getSpec()),
    frequencyPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("frequencyID"))),
    resonancePtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("bandwidthID"))),
    gainPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("gainID"))),
    transformPtr(dynamic_cast <juce::AudioParameterChoice*> (p.getAPVTS().getParameter("transformID"))),
    outputPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("outputID"))),
    mixPtr(dynamic_cast <juce::AudioParameterFloat*> (p.getAPVTS().getParameter("mixID")))
{
    jassert(frequencyPtr != nullptr);
    jassert(resonancePtr != nullptr);
    jassert(gainPtr != nullptr);
    jassert(transformPtr != nullptr);
    jassert(outputPtr != nullptr);
    jassert(mixPtr != nullptr);

}

template <typename SampleType>
void ProcessWrapper<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    mixer.prepare(spec);
    filter.prepare(spec);
    output.prepare(spec);

    reset();
    update();
};

template <typename SampleType>
void ProcessWrapper<SampleType>::reset() 
{
    mixer.reset();
    filter.reset();
    output.reset();
};

template <typename SampleType>
void ProcessWrapper<SampleType>::process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
{
    midiMessages.clear();

    update();

    juce::dsp::AudioBlock<SampleType> block(buffer);

    mixer.pushDrySamples(block);

    juce::dsp::ProcessContextReplacing<SampleType> context(block);

    filter.process(context);
    output.process(context);
    mixer.mixWetSamples(block);
};

template <typename SampleType>
void ProcessWrapper<SampleType>::update()
{
    mixer.setWetMixProportion(mixPtr->get() * 0.01f);
    
    filter.setFrequency(frequencyPtr->get());
    filter.setResonance(resonancePtr->get());
    filter.setGain(gainPtr->get());

    filter.setTransformType(static_cast<TransformationType>(transformPtr->getIndex()));
    output.setGainDecibels(outputPtr->get());
};

//==============================================================================
template class ProcessWrapper<float>;
template class ProcessWrapper<double>;