/*
  ==============================================================================

    Transformations.cpp
    Created: 7 May 2022 4:36:01am
    Author:  StoneyDSP

  ==============================================================================
*/

#include "Transformations.h"

//==============================================================================
template <typename SampleType>
Transformations<SampleType>::Transformations()
{
    reset(static_cast<SampleType>(0.0));
    coefficients(b0_, b1_, b2_, a0_, a1_, a2_);
}

//==============================================================================
template <typename SampleType>
void Transformations<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert(spec.numChannels > 0);

    Xn_1.resize(spec.numChannels);
    Xn_2.resize(spec.numChannels);
    Yn_1.resize(spec.numChannels);
    Yn_2.resize(spec.numChannels);

    reset(static_cast<SampleType>(0.0));

    coefficients(b0_, b1_, b2_, a0_, a1_, a2_);
}

template <typename SampleType>
void Transformations<SampleType>::reset(SampleType initialValue)
{
    for (auto& xn_1 : Xn_1)
        xn_1 = initialValue;

    for (auto& xn_2 : Xn_2)
        xn_2 = initialValue;

    for (auto& yn_1 : Yn_1)
        yn_1 = initialValue;

    for (auto& yn_2 : Yn_2)
        yn_2 = initialValue;
}

template <typename SampleType>
void Transformations<SampleType>::coefficients(SampleType b0, SampleType b1, SampleType b2, SampleType a0, SampleType a1, SampleType a2)
{
    a0_ = (static_cast <SampleType>(1.0) / a0);
    a1_ = (static_cast <SampleType>(a1 * a0_));
    a2_ = (static_cast <SampleType>(a2 * a0_));
    b0_ = (static_cast <SampleType>(b0 * a0_));
    b1_ = (static_cast <SampleType>(b1 * a0_));
    b2_ = (static_cast <SampleType>(b2 * a0_));
}

template <typename SampleType>
void Transformations<SampleType>::setTransformType(dfType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset(static_cast<SampleType>(0.0));
    }
}

template <typename SampleType>
SampleType Transformations<SampleType>::processSample(int channel, SampleType inputValue)
{
    jassert(isPositiveAndBelow(channel, Xn_1.size()));
    jassert(isPositiveAndBelow(channel, Xn_2.size()));
    jassert(isPositiveAndBelow(channel, Yn_1.size()));
    jassert(isPositiveAndBelow(channel, Yn_1.size()));

    return directFormI(channel, inputValue);   
}

template <typename SampleType>
SampleType Transformations<SampleType>::directFormI(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0_) + (Xn * b1_) + (Xn * b2_) - (Xn * a1_) - (Xn * a2_));

    return Yn;
}

template <typename SampleType>
SampleType Transformations<SampleType>::directFormII(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0_) + (Xn * b1_) + (Xn * b2_) - (Xn * a1_) - (Xn * a2_));

    return Yn;
}

template <typename SampleType>
SampleType Transformations<SampleType>::directFormITransposed(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0_) + (Xn * b1_) + (Xn * b2_) - (Xn * a1_) - (Xn * a2_));

    return Yn;
}

template <typename SampleType>
SampleType Transformations<SampleType>::directFormIITransposed(int channel, SampleType inputValue)
{
    SampleType Xn = inputValue;

    SampleType Yn = ((Xn * b0_) + (Xn * b1_) + (Xn * b2_) - (Xn * a1_) - (Xn * a2_));

    return Yn;
}

template <typename SampleType>
void Transformations<SampleType>::snapToZero() noexcept
{
    for (auto& xn_1 : Xn_1)
        juce::dsp::util::snapToZero(xn_1);

    for (auto& xn_2 : Xn_2)
        juce::dsp::util::snapToZero(xn_2);

    for (auto& yn_1 : Yn_1)
        juce::dsp::util::snapToZero(yn_1);

    for (auto& yn_2 : Yn_2)
        juce::dsp::util::snapToZero(yn_2);
}

//==============================================================================
template class Transformations<float>;
template class Transformations<double>;