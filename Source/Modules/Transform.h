/*
  ==============================================================================

    Transformations.h
    Created: 7 May 2022 4:36:01am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef TRANSFORM_H_INCLUDED
#define TRANSFORM_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "OrfanidisCalc.h"

enum class TransformationType
{
    directFormI,
    directFormII,
    directFormItransposed,
    directFormIItransposed
};

template <typename SampleType>
class Transformations
{
public:
    using transformationType = TransformationType;
    //==============================================================================
    /** Constructor. */
    Transformations();

    //==============================================================================
    /** Initialises the processor. */
    void prepare(juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the processor. */
    void reset(SampleType initialValue);

    /** Ensure that the state variables are rounded to zero if the state 
    variables are denormals. This is only needed if you are doing sample 
    by sample processing.*/
    void snapToZero() noexcept;

    //==============================================================================
    void coefficients(SampleType b0_, SampleType b1_, SampleType b2_, SampleType a0_, SampleType a1_, SampleType a2_);

    void setTransformType(TransformationType newTransformType);

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        if (context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample((int)channel, inputSamples[i]);
        }

#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
        snapToZero();
#endif
    }

    //==============================================================================
    /** Processes one sample at a time on a given channel. */
    SampleType processSample(int channel, SampleType inputValue);

    SampleType directFormI(int channel, SampleType inputValue);

    SampleType directFormII(int channel, SampleType inputValue);

    SampleType directFormITransposed(int channel, SampleType inputValue);

    SampleType directFormIITransposed(int channel, SampleType inputValue);

private:

    //==============================================================================
    std::vector<SampleType> Wn_1, Wn_2, Xn_1, Xn_2, Yn_1, Yn_2;

    //==============================================================================
    SampleType b0 = 1.0, b1 = 0.0, b2 = 0.0, a0 = 1.0, a1 = 0.0, a2 = 0.0;
    transformationType transformType = transformationType::directFormIItransposed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transformations)
};
#endif // TRANSFORM_H_INCLUDED