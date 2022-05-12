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
#include "OrfanidisCalc.h"
#include "Convert.h"
#include "Transform.h"
#include "OrfCoefficients.h"

template <typename SampleType>
class ProcessWrapper
{
public:
    //==============================================================================
    /** Constructor. */
    ProcessWrapper(juce::AudioProcessorValueTreeState& apvts);

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset();

    //==============================================================================
    void process(juce::AudioBuffer<SampleType>& buffer, juce::MidiBuffer& midiMessages)
    {
        juce::ignoreUnused(midiMessages);

        update();

        juce::dsp::AudioBlock<SampleType> block (buffer);
        juce::dsp::ProcessContextReplacing<SampleType> context (block);

        if (ioPtr->get() == true)
            context.isBypassed = true;
        else
            context.isBypassed = false;

        transform.process(context);
    };

    static void createParameterLayout(std::vector<std::unique_ptr<RangedAudioParameter>>& params);

    void transType(TransformationType newTransform);

private:
    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

    //==============================================================================
    /** Instantiate objects. */
    //OrfanidisCalc<SampleType> coeffs;
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
    juce::AudioParameterBool* ioPtr{ nullptr };
    juce::AudioParameterFloat* gainPtr{ nullptr };
    juce::AudioParameterFloat* freqPtr{ nullptr };
    juce::AudioParameterFloat* bandPtr{ nullptr };

    TransformationType typePtr = TransformationType::dfIt;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessWrapper)
};

#endif //  PROCESSORWRAPPER_H_INCLUDED