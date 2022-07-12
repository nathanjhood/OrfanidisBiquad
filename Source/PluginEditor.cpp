/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrfanidisBiquadAudioProcessorEditor::OrfanidisBiquadAudioProcessorEditor (OrfanidisBiquadAudioProcessor& p)
    :
    juce::AudioProcessorEditor(&p),
    audioProcessor(p),
    state(p.getAPVTS()),
    undoManager(p.getUndoManager()),
    subComponents(p, p.getAPVTS()),
    meterLeft(([&]() { return audioProcessor.getRMSLevel(0); })),
    meterRight(([&]() { return audioProcessor.getRMSLevel(1); }))
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize(425, 300);
    addAndMakeVisible(subComponents);
    addAndMakeVisible(meterLeft);
    addAndMakeVisible(meterRight);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };
    setResizable(true, true);

    startTimerHz(60);
}


OrfanidisBiquadAudioProcessorEditor::~OrfanidisBiquadAudioProcessorEditor()
{
}

//==============================================================================
void OrfanidisBiquadAudioProcessorEditor::timerCallback()
{
}

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

    subComponents.setBounds(0, 0, getWidth(), getHeight());
    //meterLeft.setBounds(130, 20, 200, 15);
    //meterRight.setBounds(130, 40, 200, 15);
    meterLeft.setBounds(420, 50, 15, 200);
    meterRight.setBounds(440, 50, 15, 200);
    undoButton.setBounds((getWidth() / 2) - 10, getHeight() - 20, 20, 20);
    redoButton.setBounds((getWidth() / 2) + 10, getHeight() - 20, 20, 20);

    subComponents.resized();
    meterLeft.resized();
    meterRight.resized();
    undoButton.resized();
    redoButton.resized();
}
