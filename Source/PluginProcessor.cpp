/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrfanidisBiquadAudioProcessor::OrfanidisBiquadAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    )
#endif

{
    bitsPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("bitsID"));
    jassert(bitsPtr != nullptr);

    bypPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("bypassID"));
    jassert(bypPtr != nullptr);
}

OrfanidisBiquadAudioProcessor::~OrfanidisBiquadAudioProcessor()
{
}

juce::AudioProcessorValueTreeState& OrfanidisBiquadAudioProcessor::getAPVTS()
{
    return apvts;
}

juce::AudioProcessorValueTreeState::ParameterLayout OrfanidisBiquadAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    ProcessWrapper<float>::createParameterLayout(params);

    params.push_back(std::make_unique<juce::AudioParameterBool>("bitsID", "Doubles", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("bypassID", "Bypass", false));
    
    return { params.begin(), params.end() };
}

//==============================================================================
juce::AudioProcessorParameter* OrfanidisBiquadAudioProcessor::getBypassParameter() const
{
    return bypPtr;
}

bool OrfanidisBiquadAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return false;
}

//==============================================================================
const juce::String OrfanidisBiquadAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OrfanidisBiquadAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OrfanidisBiquadAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OrfanidisBiquadAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OrfanidisBiquadAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OrfanidisBiquadAudioProcessor::getNumPrograms()
{
    return 1;
}

int OrfanidisBiquadAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OrfanidisBiquadAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String OrfanidisBiquadAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void OrfanidisBiquadAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index);
    juce::ignoreUnused(newName);
}

//==============================================================================
void OrfanidisBiquadAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    getProcessingPrecision();

    processor.prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
    processor.reset();
}

void OrfanidisBiquadAudioProcessor::releaseResources()
{
    processor.reset();
}

void OrfanidisBiquadAudioProcessor::update()
{
    bitsPtr->get();
    bypPtr->get();
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OrfanidisBiquadAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled()
    || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
  #endif
}
#endif

void OrfanidisBiquadAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypPtr->get() == false)

    {
        juce::ScopedNoDenormals noDenormals;

        update();

        processor.process(buffer, midiMessages);
    }

    else
    {
        processBlockBypassed(buffer, midiMessages);
    }
}

void OrfanidisBiquadAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypPtr->get() == false)

    {
        juce::ignoreUnused(buffer);
        juce::ignoreUnused(midiMessages);

        juce::ScopedNoDenormals noDenormals;

        update();
    }

    else
    {
        processBlockBypassed(buffer, midiMessages);
    }
}

void OrfanidisBiquadAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer);
    juce::ignoreUnused(midiMessages);
}

void OrfanidisBiquadAudioProcessor::processBlockBypassed(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer);
    juce::ignoreUnused(midiMessages);
}

//==============================================================================
bool OrfanidisBiquadAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* OrfanidisBiquadAudioProcessor::createEditor()
{
    //return new OrfanidisBiquadAudioProcessorEditor (*this);
    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void OrfanidisBiquadAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OrfanidisBiquadAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrfanidisBiquadAudioProcessor();
}
