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
    bypPtr = dynamic_cast       <juce::AudioParameterBool*>    (apvts.getParameter("bypassID"));
    jassert(bypPtr != nullptr);

    gainPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("gainID"));
    jassert(gainPtr != nullptr);

    freqPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("freqID"));
    jassert(freqPtr != nullptr);

    bandPtr = dynamic_cast      <juce::AudioParameterFloat*>    (apvts.getParameter("bandID"));
    jassert(bandPtr != nullptr);

    typePtr = dynamic_cast      <juce::AudioParameterChoice*>   (apvts.getParameter("typeID"));
    jassert(typePtr != nullptr);
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

    auto freqRange = juce::NormalisableRange<float>(20.00f, 20000.00f, 0.01f, 00.198894f);
    auto gainRange = juce::NormalisableRange<float>(-30.00f, 30.00f, 0.01f, 1.00f);

    params.push_back(std::make_unique<juce::AudioParameterBool>("bypassID", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("freqID", "Freq", freqRange, 632.45f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gainID", "Gain", gainRange, 00.00f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bandID", "Bandwidth", 00.10f, 01.00f, 00.10f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("typeID", "Type", juce::StringArray({ "DFI", "DFII"}), 1));
    
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

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    coeffs.setRate(sampleRate);
    peakFilter.prepare(spec);
    transform.prepare(spec);
}

void OrfanidisBiquadAudioProcessor::releaseResources()
{
    transform.reset(0);
}

void OrfanidisBiquadAudioProcessor::update()
{
    bypPtr->get();

    coeffs.calculate(*gainPtr, *freqPtr, *bandPtr);

    transform.coefficients(coeffs.b0(), coeffs.b1(), coeffs.b2(), coeffs.a0(), coeffs.a1(), coeffs.a2());

    if (typePtr->getIndex() == 0)
        transform.setTransformType(TransformationType::dfI);
    else
        transform.setTransformType(TransformationType::dfII);
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

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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
        juce::ignoreUnused(midiMessages);

        juce::ScopedNoDenormals noDenormals;

        update();

        auto block = juce::dsp::AudioBlock<float>(buffer);

        auto context = juce::dsp::ProcessContextReplacing(block);

        transform.process(context);
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
