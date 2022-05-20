/*
  ==============================================================================

    ProcessorWrapper.h
    Created: 8 May 2022 9:38:17pm
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef PROCESSORWRAPPER_H_INCLUDED
#define PROCESSORWRAPPER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
//#include "OrfanidisCalc.h"
#include "Convert.h"
#include "Transform.h"
#include "OrfCoefficients.h"
//#include "Biquads.h"

template <typename SampleType>
class ProcessWrapper
{
public:
    //==============================================================================
    /** Constructor. */
    ProcessWrapper(juce::AudioProcessorValueTreeState& apvts);

    //==============================================================================
    /** Create Parameter Layout. */
    static void createParameterLayout(std::vector<std::unique_ptr<RangedAudioParameter>>& params);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages);

private:
    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

    //==============================================================================
    /** Instantiate objects. */
    juce::dsp::ProcessSpec spec;
    Conversion<SampleType> convert;
    Transformations<SampleType> transform;
    OrfCoefficients<SampleType> coeffs;

    //==============================================================================
    /** Parameter objects. */
    juce::AudioParameterBool* io;
    juce::AudioParameterFloat* gain;
    juce::AudioParameterFloat* frequency;
    juce::AudioParameterFloat* bandwidth;

    //==============================================================================
    /** Parameter pointers. */
    juce::AudioParameterBool*     ioPtr{ nullptr };
    juce::AudioParameterFloat*    frequencyPtr{ nullptr };
    juce::AudioParameterFloat*    resonancePtr{ nullptr };
    juce::AudioParameterFloat*    gainPtr{ nullptr };
    juce::AudioParameterChoice*    transformPtr{ nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};

#endif //  PROCESSORWRAPPER_H_INCLUDED