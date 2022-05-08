/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OrfanidisCalc.h"
#include "Convert.h"
#include "PeakFilter.h"
#include "Transform.h"

//==============================================================================
/**
*/
class OrfanidisBiquadAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    OrfanidisBiquadAudioProcessor();
    ~OrfanidisBiquadAudioProcessor() override;

    //==============================================================================
    juce::AudioProcessorParameter* getBypassParameter() const;
    bool supportsDoublePrecisionProcessing() const;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //==============================================================================
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages);

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:

    //==============================================================================
    /** Updates the internal state variables of the processor. */
    void update();

    //==============================================================================
    juce::AudioParameterFloat* gain;
    juce::AudioParameterFloat* frequency;
    juce::AudioParameterFloat* bandwidth;
    juce::AudioParameterChoice* type;

    juce::dsp::ProcessSpec spec;

    OrfanidisCalc<float> coeffs;
    Conversion<float> convert;
    Transformations<float> transform;

    //==============================================================================
    /** Parameter pointers. */
    juce::AudioParameterBool* bitsPtr{ nullptr };
    juce::AudioParameterBool* bypPtr{ nullptr };
    juce::AudioParameterFloat* gainPtr{ nullptr };
    juce::AudioParameterFloat* freqPtr{ nullptr };
    juce::AudioParameterFloat* bandPtr{ nullptr };
    juce::AudioParameterChoice* transPtr{ nullptr };

    //==============================================================================
    /** Init variables. */

    double ramp = 0.0001;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrfanidisBiquadAudioProcessor)
};
