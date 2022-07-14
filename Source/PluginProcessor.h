/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginParameters.h"
#include "PluginWrapper.h"

//==============================================================================
/**
*/
class OrfanidisBiquadAudioProcessor : public juce::AudioProcessor
{
public:
    using precisionType = ProcessingPrecision;
    //==============================================================================
    OrfanidisBiquadAudioProcessor();
    ~OrfanidisBiquadAudioProcessor() override;

    //==========================================================================
    juce::AudioProcessorParameter* getBypassParameter() const;
    bool isBypassed() const noexcept;
    void setBypassParameter(juce::AudioParameterBool* newBypass) noexcept;

    //==========================================================================
    bool supportsDoublePrecisionProcessing() const override;
    ProcessingPrecision getProcessingPrecision() const noexcept;
    bool isUsingDoublePrecision() const noexcept;
    void setProcessingPrecision(ProcessingPrecision newPrecision) noexcept;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    //==============================================================================
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==========================================================================
    /** Undo Manager. */
    juce::UndoManager undoManager;
    juce::UndoManager& getUndoManager() { return undoManager; };

    //==========================================================================
    /** Audio processor value tree. */
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; };
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==========================================================================
    /** Audio processor specs. */
    juce::dsp::ProcessSpec spec;
    juce::dsp::ProcessSpec& getSpec() { return spec; };

    float getRMSLevel(const int channel) const;

private:
    juce::LinearSmoothedValue<float> rmsLeft, rmsRight;

    //==========================================================================
    /** Audio processor members. */
    Parameters parameters;
    ProcessWrapper<float> processorFloat;
    ProcessWrapper<double> processorDouble;

    //==========================================================================
    /** Parameter pointers. */
    juce::AudioParameterInt* precisionPtr{ nullptr };
    juce::AudioParameterBool* bypassState{ nullptr };

    //==========================================================================
    /** Init variables. */
    double rampDurationSeconds = 0.05;
    ProcessingPrecision processingPrecision;

    //==========================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrfanidisBiquadAudioProcessor)
};
