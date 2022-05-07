/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrfanidisBiquadAudioProcessorEditor::OrfanidisBiquadAudioProcessorEditor (OrfanidisBiquadAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p)
{
    addAndMakeVisible(gainSlider_);
    gainSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider_.setTextBoxStyle(juce::Slider::TextEntryBoxPosition(juce::Slider::TextBoxRight), false, 80, 40);
    gainSlider_.setPopupDisplayEnabled(true, false, nullptr, 20000);
    gainSliderAttachmentPtr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "gain", gainSlider_));

    addAndMakeVisible(frequencySlider_);
    frequencySlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider_.setTextBoxStyle(juce::Slider::TextEntryBoxPosition (juce::Slider::TextBoxRight), false, 80, 40);
    frequencySlider_.setPopupDisplayEnabled(true, false, nullptr, 20000);
    freqSliderAttachmentPtr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "freq", frequencySlider_));

    addAndMakeVisible(bandwidthSlider_);
    bandwidthSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    bandwidthSlider_.setTextBoxStyle(juce::Slider::TextEntryBoxPosition (juce::Slider::TextBoxRight), false, 80, 40);
    bandwidthSlider_.setPopupDisplayEnabled(true, false, nullptr, 20000);
    bandwidthSliderAttachmentPtr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "bandwidth", bandwidthSlider_));

    addAndMakeVisible(gainLabel_);
    addAndMakeVisible(frequencyLabel_);
    addAndMakeVisible(bandwidthLabel_);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    const int numRows = p.getNumParameters();
    const int height = numRows * 48 + 24;               // obviously named constants
    setSize(512, height);                              // would be better here!
}


OrfanidisBiquadAudioProcessorEditor::~OrfanidisBiquadAudioProcessorEditor()
{
}

//==============================================================================
void OrfanidisBiquadAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::darkslategrey);
}

void OrfanidisBiquadAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const int unit = 8;
    const int margin = 2 * unit;
    const int offsetAdd = 4 * unit;
    const int offsetMult = 6 * unit;
    const int sliderWidth = 60 * unit;
    const int sliderHeight = 3 * unit;

    gainSlider_.setBounds(margin, offsetAdd + 0 * offsetMult, sliderWidth, sliderHeight);
    frequencySlider_.setBounds(margin, offsetAdd + 1 * offsetMult, sliderWidth, sliderHeight);
    bandwidthSlider_.setBounds(margin, offsetAdd + 2 * offsetMult, sliderWidth, sliderHeight);

    gainLabel_.setBounds(margin, 12 + 0 * offsetMult, sliderWidth, sliderHeight);
    frequencyLabel_.setBounds(margin, 12 + 1 * offsetMult, sliderWidth, sliderHeight);
    bandwidthLabel_.setBounds(margin, 12 + 2 * offsetMult, sliderWidth, sliderHeight);
}
