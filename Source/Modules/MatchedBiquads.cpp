/*
  ==============================================================================

    MatchedBiquads.cpp
    Created: 14 Jul 2022 8:13:32pm
    Author:  natha

  ==============================================================================
*/

#include "MatchedBiquads.h"

template <typename SampleType>
MatchedBiquad<SampleType>::MatchedBiquad()
{
    reset();
}

template <typename SampleType>
void MatchedBiquad<SampleType>::setFrequency(SampleType newFreq)
{
    if (f != newFreq)
    {
        f = newFreq;
        coeffs();
    }
}

template <typename SampleType>
void MatchedBiquad<SampleType>::setGain(SampleType newGain)
{
    if (g != newGain)
    {
        g = newGain;
        coeffs();
    }
}

template <typename SampleType>
void MatchedBiquad<SampleType>::setResonance(SampleType newRes)
{

    if (q != juce::jlimit(0.1, 100, newRes))
    {
        q = newRes;
        coeffs();
    };
}

template <typename SampleType>
void MatchedBiquad<SampleType>::setFilterType(FilterType newType)
{
    if (type != newType)
    {
        type = newType;
        reset();
        coeffs();
    }
}

template <typename SampleType>
void MatchedBiquad<SampleType>::setTransformType(TransformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset();
        coeffs();
    }
}

template <typename SampleType>
void MatchedBiquad<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
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
}

template <typename SampleType>
void MatchedBiquad<SampleType>::reset(SampleType initValue)
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initValue);
}

template <typename SampleType>
SampleType MatchedBiquad<SampleType>::processSample(int channel, SampleType inputValue)
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
SampleType MatchedBiquad<SampleType>::directFormI(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];
    auto& Yn1 = Yn_1[(size_t)channel];
    auto& Yn2 = Yn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn * b0) + (Xn1 * b1) + (Xn2 * b2) + (Yn1 * a1) + (Yn2 * a2));

    Xn2 = Xn1;
    Yn2 = Yn1;
    Xn1 = Xn;
    Yn1 = Yn;

    return Yn;
}

template <typename SampleType>
SampleType MatchedBiquad<SampleType>::directFormII(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = (Xn + ((Wn1 * a1) + (Wn2 * a2)));
    Yn = ((Wn * b0) + (Wn1 * b1) + (Wn2 * b2));

    Wn2 = Wn1;
    Wn1 = Wn;

    return Yn;
}

template <typename SampleType>
SampleType MatchedBiquad<SampleType>::directFormITransposed(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = (Xn + Wn2);
    Yn = ((Wn * b0) + Xn2);

    Xn2 = ((Wn * b1) + Xn1);
    Wn2 = ((Wn * a1) + Wn1);
    Xn1 = (Wn * b2);
    Wn1 = (Wn * a2);

    return Yn;
}

template <typename SampleType>
SampleType MatchedBiquad<SampleType>::directFormIITransposed(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn * b0) + (Xn2));

    Xn2 = ((Xn * b1) + (Xn1) + (Yn * a1));
    Xn1 = ((Xn * b2) + (Yn * a2));

    return Yn;
}

template <typename SampleType>
void MatchedBiquad<SampleType>::coeffs()
{
    f0 = f / (static_cast<SampleType>sampleRate / two);
    AA = std::pow(10.0, g / 20.0);

    switch (type)
    {
    case FilterType::PeakEQ:

        alfa = std::sin(f0 * pi) / (two * std::sqrt(AA) * q);

        // Poles
        
        a0 = one;
        a1 = minusTwo * std::cos(f0 * pi) / (one + alfa);
        a2 = (one - alfa) / (one + alfa);

        // Zeros
        b0 = (one + AA * alfa) / (one + alfa);
        b1 = a1;
        b2 = (one - AA * alfa) / (one + alfa);

        break;

    case FilterType::HighPass:

        alfa = std::sin(f0 * pi) / (two * q);

        // Poles
        a0 = one;
        a1 = minusTwo * std::cos(f0 * pi) / (one + alfa);
        a2 = (one - alfa) / (one + alfa);

        // Zeros
        b0 = (one - a1 + a2) / (two * two);
        b1 = minusTwo * b0;
        b2 = b0;

        break;

    case FilterType::BandPass:

        alfa = std::sin(f0 * pi) / (two * q);

        // Poles
        a0 = one;
        a1 = minusTwo * std::cos(f0 * pi) / (one + alfa);
        a2 = (one - alfa) / (one + alfa);

        // Zeros
        b0 = (one - a2) / two;
        b1 = zero;
        b2 = -b0;

        break;
    }
}

template <typename SampleType>
void MatchedBiquad<SampleType>::snapToZero() noexcept
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        for (auto& element : *v)
            juce::dsp::util::snapToZero(element);
}

//==============================================================================
template class MatchedBiquad<float>;
template class MatchedBiquad<double>;
