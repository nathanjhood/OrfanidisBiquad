/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ParameterSlider.h"

//==============================================================================
/**
*/
class OrfanidisBiquadAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OrfanidisBiquadAudioProcessorEditor (OrfanidisBiquadAudioProcessor&);
    ~OrfanidisBiquadAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    juce::Label gainLabel_;
    juce::Label frequencyLabel_;
    juce::Label bandwidthLabel_;

    juce::Slider gainSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachmentPtr;

    juce::Slider frequencySlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqSliderAttachmentPtr;

    juce::Slider bandwidthSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bandwidthSliderAttachmentPtr;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OrfanidisBiquadAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrfanidisBiquadAudioProcessorEditor)
};
