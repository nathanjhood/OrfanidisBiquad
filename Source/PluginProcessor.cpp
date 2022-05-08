/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
//#include "PluginEditor.h"

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

    bandPtr = dynamic_cast  <juce::AudioParameterFloat*>        (apvts.getParameter("bandID"));
    jassert(bandPtr != nullptr);

    xn_1.resize(8, 0); // state for up to 8 chans
    xn_2.resize(8, 0);
    yn_1.resize(8, 0);
    yn_2.resize(8, 0);
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

    xn_1.assign(8, 0);
    xn_2.assign(8, 0);
    yn_1.assign(8, 0);
    yn_2.assign(8, 0);

    coeffs.setRate(sampleRate);
    peakFilter.prepare(spec);
    transform.prepare(spec);
}

void OrfanidisBiquadAudioProcessor::releaseResources()
{
}

void OrfanidisBiquadAudioProcessor::update()
{
    bypPtr->get();
    //coeffs.calculate(*gainPtr, *freqPtr, *bandPtr);
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
        auto totalNumInputChannels = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        // This is here to avoid people getting screaming feedback
        // when they first compile a plugin, but obviously you don't need to keep
        // this code if your algorithm always overwrites all the output channels.
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());

        // This is the place where you'd normally do the guts of your plugin's
        // audio processing...

        update();
        coeffs.calculate(*gainPtr, *freqPtr, *bandPtr);

        //coeffs.calculate(*gain, *frequency, *bandwidth);

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

    else
    {
        processBlockBypassed(buffer, midiMessages);
    }
}

void OrfanidisBiquadAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    if (bypPtr->get() == false)

    {
        juce::ignoreUnused(midiMessages);

        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels = getTotalNumInputChannels();


        // This is the place where you'd normally do the guts of your plugin's
        // audio processing...

        update();

        coeffs.calculate(*gain, *frequency, *bandwidth);

        const double b0 = coeffs.b0();
        const double b1 = coeffs.b1();
        const double b2 = coeffs.b2();
        const double a1 = coeffs.a1();
        const double a2 = coeffs.a2();

        double** yn = buffer.getArrayOfWritePointers();

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
