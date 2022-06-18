/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrfanidisBiquadAudioProcessor::OrfanidisBiquadAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    apvts(*this, &undoManager, "Parameters", createParameterLayout()),
    spec(),
    parameters(*this, getAPVTS()),
    processorFloat(*this, getAPVTS(), getSpec()),
    processorDouble(*this, getAPVTS(), getSpec())
{
}

OrfanidisBiquadAudioProcessor::~OrfanidisBiquadAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorParameter* OrfanidisBiquadAudioProcessor::getBypassParameter() const
{
    return bypassState;
}

bool OrfanidisBiquadAudioProcessor::isBypassed() const noexcept
{
    return bypassState->get() == true;
}

void OrfanidisBiquadAudioProcessor::setBypassParameter(juce::AudioParameterBool* newBypass) noexcept
{
    if (bypassState != newBypass)
    {
        bypassState = newBypass;
        releaseResources();
        reset();
    }
}

bool OrfanidisBiquadAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return false;
}

juce::AudioProcessor::ProcessingPrecision OrfanidisBiquadAudioProcessor::getProcessingPrecision() const noexcept
{
    return processingPrecision;
}

bool OrfanidisBiquadAudioProcessor::isUsingDoublePrecision() const noexcept
{
    return processingPrecision == doublePrecision;
}

void OrfanidisBiquadAudioProcessor::setProcessingPrecision(ProcessingPrecision newPrecision) noexcept
{
    // If you hit this assertion then you're trying to use double precision
    // processing on a processor which does not support it!
    jassert(newPrecision != doublePrecision || supportsDoublePrecisionProcessing());

    if (processingPrecision != newPrecision)
    {
        processingPrecision = newPrecision;
        releaseResources();
        processorFloat.reset();
        processorDouble.reset();
    }
}

//==============================================================================
const juce::String OrfanidisBiquadAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OrfanidisBiquadAudioProcessor::acceptsMidi() const
{
    return false;
}

bool OrfanidisBiquadAudioProcessor::producesMidi() const
{
    return false;
}

bool OrfanidisBiquadAudioProcessor::isMidiEffect() const
{
    return false;
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

    processorFloat.prepare(getSpec());
    processorDouble.prepare(getSpec());
}

void OrfanidisBiquadAudioProcessor::releaseResources()
{
    processorFloat.reset();
    processorDouble.reset();
}

void OrfanidisBiquadAudioProcessor::numChannelsChanged()
{
    releaseResources();
    processorFloat.prepare(getSpec());
    processorDouble.prepare(getSpec());
}

void OrfanidisBiquadAudioProcessor::numBusesChanged()
{
    releaseResources();
    processorFloat.prepare(getSpec());
    processorDouble.prepare(getSpec());
}

void OrfanidisBiquadAudioProcessor::processorLayoutsChanged()
{
    releaseResources();
    processorFloat.prepare(getSpec());
    processorDouble.prepare(getSpec());
}

bool OrfanidisBiquadAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled()
    || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void OrfanidisBiquadAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassState->get() == true)
    {
        processBlockBypassed(buffer, midiMessages);
    }

    else
    {
        juce::ScopedNoDenormals noDenormals;

        processorFloat.process(buffer, midiMessages);
    }
}

void OrfanidisBiquadAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypassState->get() == true)
    {
        processBlockBypassed(buffer, midiMessages);
    }

    else
    {
        juce::ScopedNoDenormals noDenormals;

        processorDouble.process(buffer, midiMessages);
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
    return new OrfanidisBiquadAudioProcessorEditor(*this, getAPVTS(), undoManager);
}

juce::AudioProcessorValueTreeState::ParameterLayout OrfanidisBiquadAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout params;

    Parameters::setParameterLayout(params);

    return params;
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
