/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OrfanidisCalc.h"

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
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();
    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    //==============================================================================

    juce::AudioParameterFloat* gain;      // managedParameters owns
    juce::AudioParameterFloat* frequency;
    juce::AudioParameterFloat* bandwidth;

    OrfanidisCalc coeffs;

    std::vector<double> xn_1; // state for up to 8 chans
    std::vector<double> xn_2;
    std::vector<double> yn_1;
    std::vector<double> yn_2;

    double denormal{ 1.0e-16 };  // use to keep mantissa from dropping below 1.xxx
                               // see http://www.earlevel.com/main/2012/12/03/a-note-about-de-normalization/
                               // also see Falco's DspFilters MathSupplement.h re: 1e-8?



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrfanidisBiquadAudioProcessor)
};
