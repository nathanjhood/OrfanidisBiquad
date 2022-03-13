/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrfanidisBiquadAudioProcessor::OrfanidisBiquadAudioProcessor()
    : gain    { nullptr },
    frequency { nullptr },
    bandwidth { nullptr }
{
    gain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("gain"));
    jassert(gain != nullptr);

    frequency = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("freq"));
    jassert(frequency != nullptr);

    bandwidth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("bandwidth"));
    jassert(bandwidth != nullptr);

    xn_1.resize(8, 0); // state for up to 8 chans
    xn_2.resize(8, 0);
    yn_1.resize(8, 0);
    yn_2.resize(8, 0);
}

OrfanidisBiquadAudioProcessor::~OrfanidisBiquadAudioProcessor()
{
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OrfanidisBiquadAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OrfanidisBiquadAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OrfanidisBiquadAudioProcessor::getProgramName (int index)
{
    return {};
}

void OrfanidisBiquadAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OrfanidisBiquadAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec{};

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    xn_1.assign(8, 0);
    xn_2.assign(8, 0);
    yn_1.assign(8, 0);
    yn_2.assign(8, 0);

    coeffs.setRate(sampleRate);
}

void OrfanidisBiquadAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OrfanidisBiquadAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
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
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    
    coeffs.calculate(*gain, *frequency, *bandwidth);

    const double b0 = coeffs.b0();
    const double b1 = coeffs.b1();
    const double b2 = coeffs.b2();
    const double a1 = coeffs.a1();
    const double a2 = coeffs.a2();

    float** yn = buffer.getArrayOfWritePointers();

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        for (int c = 0; c < totalNumInputChannels; ++c)
        {
            yn[c][s] += denormal;

            const double xn = static_cast<double> (yn[c][s]); // perform calcs at double

                                          // direct form 1
            yn[c][s] = static_cast<float> (b0 * xn + b1 * xn_1[c] + b2 * xn_2[c]
                - a1 * yn_1[c] - a2 * yn_2[c]);

            xn_2[c] = xn_1[c];  // advance delay networks
            xn_1[c] = xn;
            yn_2[c] = yn_1[c];
            yn_1[c] = yn[c][s];
        }
    }

    denormal *= -1; // avoid removal by DC filter
}

//==============================================================================
bool OrfanidisBiquadAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OrfanidisBiquadAudioProcessor::createEditor()
{
    return new OrfanidisBiquadAudioProcessorEditor (*this);
    //return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void OrfanidisBiquadAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OrfanidisBiquadAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout OrfanidisBiquadAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    using namespace juce;

    auto gainRange = NormalisableRange<float>(-36.00f, 36.00f, 00.01f, 1.0f);
    layout.add(std::make_unique<AudioParameterFloat>("gain", "Gain", gainRange, 00.00f));

    auto freqRange = NormalisableRange<float>(20.000f, 20000.000f, 00.001f, 0.198894f);
    layout.add(std::make_unique<AudioParameterFloat>("freq", "Freq", freqRange, 632.455f));

    auto bwRange = NormalisableRange<float>(0.03f, 6.00f, 0.01f, 1.f);
    layout.add(std::make_unique<AudioParameterFloat>("bandwidth", "Bandwidth", bwRange, 0.707f));

    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrfanidisBiquadAudioProcessor();
}
