/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Components/AutoComponent.h"

//==============================================================================
/**
*/
class OrfanidisBiquadAudioProcessorEditor  : public juce::AudioProcessorEditor/*, public juce::Timer*/
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    //==========================================================================
    OrfanidisBiquadAudioProcessorEditor (OrfanidisBiquadAudioProcessor& p, APVTS& apvts, juce::UndoManager& um);
    ~OrfanidisBiquadAudioProcessorEditor() override;

    //==============================================================================
    //void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OrfanidisBiquadAudioProcessor& audioProcessor;
    APVTS& state;
    juce::UndoManager& undoManager;

    AutoComponent subComponents;
    juce::TextButton undoButton, redoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrfanidisBiquadAudioProcessorEditor)
};
