/*
  ==============================================================================

    OrfanidisPeak.cpp
    Created: 10 Jun 2022 1:13:19am
    Author:  Nathan J. Hood - adapted from:

    peq.m - Parametric EQ with matching gain at Nyquist frequency
    Sophocles J. Orfanidis, J. Audio Eng. Soc., vol.45, p.444, June 1997.
    https://www.ece.rutgers.edu/~orfanidi/ece348/peq.pdf

    Released under GNU General Public License v3.0

  ==============================================================================
*/

#include "OrfanidisPeak.h"

//==============================================================================
template <typename SampleType>
OrfanidisPeak<SampleType>::OrfanidisPeak()
{
    reset(static_cast<SampleType>(0.0));
}

//==============================================================================
template <typename SampleType>
void OrfanidisPeak<SampleType>::setFrequency(SampleType newFreq)
{
    jassert(static_cast<SampleType>(20.0) <= newFreq && newFreq <= static_cast<SampleType>(20000.0));

    hz = static_cast<SampleType>(juce::jlimit(minFreq, maxFreq, newFreq));
    frq.setTargetValue(hz);
    coefficients();
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::setResonance(SampleType newRes)
{
    jassert(static_cast<SampleType>(0.1) <= newRes && newRes <= static_cast<SampleType>(100.0));

    q = static_cast<SampleType>(juce::jlimit(SampleType(0.1), SampleType(100.0), newRes));
    res.setTargetValue(q);
    coefficients();
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::setGain(SampleType newGain)
{
    g = static_cast<SampleType>(newGain);
    lev.setTargetValue(g);
    coefficients();
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::setTransformType(transformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset(static_cast<SampleType>(0.0));
        coefficients();
    }
}

//==============================================================================
template <typename SampleType>
void OrfanidisPeak<SampleType>::setRampDurationSeconds(double newDurationSeconds) noexcept
{
    if (rampDurationSeconds != newDurationSeconds)
    {
        rampDurationSeconds = newDurationSeconds;
        reset(static_cast<SampleType>(0.0));
    }
}

template <typename SampleType>
double OrfanidisPeak<SampleType>::getRampDurationSeconds() const noexcept
{
    return rampDurationSeconds;
}

template <typename SampleType>
bool OrfanidisPeak<SampleType>::isSmoothing() const noexcept
{
    bool isSmoothing = frq.isSmoothing() || res.isSmoothing() || lev.isSmoothing();

    return isSmoothing;
}

//==============================================================================
template <typename SampleType>
void OrfanidisPeak<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    Wn_1.resize(spec.numChannels);
    Wn_2.resize(spec.numChannels);
    Xn_1.resize(spec.numChannels);
    Xn_2.resize(spec.numChannels);
    Yn_1.resize(spec.numChannels);
    Yn_2.resize(spec.numChannels);

    reset(static_cast<SampleType>(0.0));

    minFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(24576.0);
    maxFreq = static_cast <SampleType>(sampleRate) / static_cast <SampleType>(2.125);

    jassert(static_cast <SampleType>(20.0) >= minFreq && minFreq <= static_cast <SampleType>(20000.0));
    jassert(static_cast <SampleType>(20.0) <= maxFreq && maxFreq >= static_cast <SampleType>(20000.0));

    setFrequency(hz);
    setResonance(q);
    setGain(g);

    coefficients();
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::reset(SampleType initialValue)
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initialValue);

    frq.reset(sampleRate, rampDurationSeconds);
    res.reset(sampleRate, rampDurationSeconds);
    lev.reset(sampleRate, rampDurationSeconds);

    coefficients();
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::processSample(int channel, SampleType inputValue)
{
    jassert(juce::isPositiveAndBelow(channel, Wn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Wn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Xn_2.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));
    jassert(juce::isPositiveAndBelow(channel, Yn_1.size()));


    switch (transformType)
    {
    case TransformationType::directFormI:
        inputValue = directFormI(channel, inputValue);
        break;
    case TransformationType::directFormII:
        inputValue = directFormII(channel, inputValue);
        break;
    case TransformationType::directFormItransposed:
        inputValue = directFormITransposed(channel, inputValue);
        break;
    case TransformationType::directFormIItransposed:
        inputValue = directFormIITransposed(channel, inputValue);
        break;
    default:
        inputValue = directFormIITransposed(channel, inputValue);
    }

    return inputValue;
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::directFormI(int channel, SampleType inputValue)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];
    auto& Yn1 = Yn_1[(size_t)channel];
    auto& Yn2 = Yn_2[(size_t)channel];

    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0) + (Xn1 * b1) + (Xn2 * b2) + (Yn1 * a1) + (Yn2 * a2));

    Xn2 = Xn1, Yn2 = Yn1;
    Xn1 = Xn, Yn1 = Yn;

    return Yn;
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::directFormII(int channel, SampleType inputValue)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];

    SampleType Xn = inputValue;

    SampleType Wn = (Xn + ((Wn1 * a1) + (Wn2 * a2)));
    SampleType Yn = ((Wn * b0) + (Wn1 * b1) + (Wn2 * b2));

    Wn2 = Wn1;
    Wn1 = Wn;

    return Yn;
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::directFormITransposed(int channel, SampleType inputValue)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    SampleType Xn = inputValue;

    SampleType Wn = (Xn + Wn2);
    SampleType Yn = ((Wn * b0) + Xn2);

    Xn2 = ((Wn * b1) + Xn1), Wn2 = ((Wn * a1) + Wn1);
    Xn1 = (Wn * b2), Wn1 = (Wn * a2);

    return Yn;
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::directFormIITransposed(int channel, SampleType inputValue)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0) + (Xn2));

    Xn2 = ((Xn * b1) + (Xn1)+(Yn * a1));
    Xn1 = ((Xn * b2) + (Yn * a2));

    return Yn;
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::coefficients()
{
    const SampleType gainLin = static_cast <SampleType>(juce::Decibels::decibelsToGain(static_cast<SampleType>(lev.getNextValue())));
    const SampleType bandwidthGain = static_cast <SampleType> (juce::Decibels::decibelsToGain(lev.getNextValue() / root2));
    const SampleType hzFrequency = static_cast <SampleType>(frq.getNextValue());
    const SampleType radSampFrequency = static_cast <SampleType>(hzFrequency * ((two * pi) / sampleRate));
    const SampleType radSampBandwidth = static_cast <SampleType>(radSampFrequency / (SampleType(1.588308819) * q));

    SampleType G0 = one;
    SampleType G = gainLin;
    SampleType GB = bandwidthGain;
    SampleType w0 = radSampFrequency;
    SampleType Dw = radSampBandwidth;

    SampleType b_0 = one;
    SampleType b_1 = zero;
    SampleType b_2 = zero;
    SampleType a_0 = one;
    SampleType a_1 = zero;
    SampleType a_2 = zero;

    if (G == GB)    // if no boost or cut, pass audio
    {
        b_0 = one;
        b_1 = zero;
        b_2 = zero;
        a_0 = one;
        a_1 = zero;
        a_2 = zero;
    }

    else            // else calculate coefficients
    {
        const SampleType F = std::abs(G * G - GB * GB);
        const SampleType G00 = std::abs(G * G - G0 * G0);
        const SampleType F00 = std::abs(GB * GB - G0 * G0);

        const SampleType num = G0 * G0 * std::pow(w0 * w0 - pi * pi, two) + G * G * F00 * pi * pi * Dw * Dw / F;
        const SampleType den = std::pow(w0 * w0 - pi * pi, two) + F00 * pi * pi * Dw * Dw / F;

        const SampleType G1 = std::sqrt(num / den);

        const SampleType G01 = std::abs(G * G - G0 * G1);
        const SampleType G11 = std::abs(G * G - G1 * G1);
        const SampleType F01 = std::abs(GB * GB - G0 * G1);
        const SampleType F11 = std::abs(GB * GB - G1 * G1);

        const SampleType W2 = std::sqrt(G11 / G00) * std::pow(std::tan(w0 / two), two);
        const SampleType DW = (one + std::sqrt(F00 / F11) * W2) * std::tan(Dw / two);

        const SampleType C = F11 * DW * DW - two * W2 * (F01 - std::sqrt(F00 * F11));
        const SampleType D = two * W2 * (G01 - std::sqrt(G00 * G11));

        const SampleType A = std::sqrt((C + D) / F);
        const SampleType B = std::sqrt((G * G * C + GB * GB * D) / F);

        b_0 = (G1 + G0 * W2 + B) / (one + W2 + A);
        b_1 = minusTwo * ((G1 - G0 * W2) / (one + W2 + A));
        b_2 = (G1 - B + G0 * W2) / (one + W2 + A);
        a_0 = one;
        a_1 = minusTwo * ((one - W2) / (one + W2 + A)),
        a_2 = (one + W2 - A) / (one + W2 + A);
    }

    

    a0 = static_cast <SampleType>(one / a_0);
    a1 = static_cast <SampleType>((a_1 * a0) * minusOne);
    a2 = static_cast <SampleType>((a_2 * a0) * minusOne);
    b0 = static_cast <SampleType>(b_0 * a0);
    b1 = static_cast <SampleType>(b_1 * a0);
    b2 = static_cast <SampleType>(b_2 * a0);
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::snapToZero() noexcept
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        for (auto& element : *v)
            juce::dsp::util::snapToZero(element);
}

//==============================================================================
template class OrfanidisPeak<float>;
template class OrfanidisPeak<double>;