/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrfanidisBiquadAudioProcessorEditor::OrfanidisBiquadAudioProcessorEditor (OrfanidisBiquadAudioProcessor& p, APVTS& apvts, juce::UndoManager& um)
    :
    juce::AudioProcessorEditor(&p),
    audioProcessor(p),
    state(apvts),
    undoManager(um),
    subComponents(p, apvts),
    undoButton("Undo"),
    redoButton("Redo")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize(425, 250);
    addAndMakeVisible(subComponents);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };
    setResizable(true, true);

    //startTimerHz(24);
}


OrfanidisBiquadAudioProcessorEditor::~OrfanidisBiquadAudioProcessorEditor()
{
}

////==============================================================================
//void OrfanidisBiquadAudioProcessorEditor::timerCallback()
//{
//    undoManager.beginNewTransaction();
//}

void OrfanidisBiquadAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::midnightblue);

    //// draw an outline around the component
    /*g.setColour(juce::Colours::hotpink);
    g.drawRect(getLocalBounds(), 2);*/

    // Add project info text to background here
    g.setColour(juce::Colours::antiquewhite);
    g.setFont(15.0f);
    g.drawFittedText(ProjectInfo::companyName, getLocalBounds(), juce::Justification::topLeft, 1);
    g.drawFittedText(ProjectInfo::projectName, getLocalBounds(), juce::Justification::topRight, 1);
    g.drawFittedText(ProjectInfo::versionString, getLocalBounds(), juce::Justification::bottomLeft, 1);
}

void OrfanidisBiquadAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    subComponents.resized();
    subComponents.setBounds(0, 25, getWidth(), getHeight());
}
