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
    :
    G0(one), G(zero), GB(zero), w0(zero), Dw(zero),
    frq(minFreq), res(one), gain(zero),
    loop(zero), outputSample(zero), omega(zero), 
    minFreq((SampleType)20.0), maxFreq((SampleType)20000.0),
    transformType(TransformationType::directFormIItransposed),
    F(zero), D(zero), C(zero), B(zero), A(zero), G1(zero), W2(zero), DW(zero),
    F00(zero), F01(zero), F11(zero), G00(zero), G01(zero), G11(zero), 
    G0G1(zero),
    G0W2(zero),
    omegaPiTwo(zero), onePlusW2A(zero), num(zero), den(zero),
    Gsq(zero), GsqX(zero), GsqD(zero), Fsq(zero), FsqX(zero), FsqD(zero)
{
    reset();
}

//==============================================================================
template <typename SampleType>
void OrfanidisPeak<SampleType>::setFrequency(SampleType newFreq)
{
    jassert(minFreq <= newFreq && newFreq <= maxFreq);

    if (frq != juce::jlimit(minFreq, maxFreq, newFreq))
    {
        frq = newFreq;
        
        coefficients();
    }
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::setResonance(SampleType newRes)
{
    if (res != juce::jlimit(SampleType(0.1), SampleType(1.0), newRes))
    {
        res = one / newRes;

        coefficients();
    }
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::setGain(SampleType newGain)
{
    if (gain != newGain)
    {
        gain = newGain;

        coefficients();
    }
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::setTransformType(transformationType newTransformType)
{
    if (transformType != newTransformType)
    {
        transformType = newTransformType;
        reset();
        coefficients();
    }
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

    omega = (two * pi) / static_cast <SampleType> (sampleRate);

    minFreq = static_cast <SampleType> (sampleRate / 24576.0);
    maxFreq = static_cast <SampleType> (sampleRate / 2.125);

    jassert(static_cast <SampleType> (20.0) >= minFreq && minFreq <= static_cast <SampleType> (20000.0));
    jassert(static_cast <SampleType> (20.0) <= maxFreq && maxFreq >= static_cast <SampleType> (20000.0));

    reset();

    coefficients();
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::reset(SampleType initialValue)
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        std::fill(v->begin(), v->end(), initialValue);
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
SampleType OrfanidisPeak<SampleType>::directFormI(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];
    auto& Yn1 = Yn_1[(size_t)channel];
    auto& Yn2 = Yn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn * b[0]) + (Xn1 * b[1]) + (Xn2 * b[2]) + (Yn1 * a[1]) + (Yn2 * a[2]));

    Xn2 = Xn1; 
    Yn2 = Yn1;
    Xn1 = Xn; 
    Yn1 = Yn;

    return Yn;
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::directFormII(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = (Xn + ((Wn1 * a[1]) + (Wn2 * a[2])));
    Yn = ((Wn * b[0]) + (Wn1 * b[1]) + (Wn2 * b[2]));

    Wn2 = Wn1;
    Wn1 = Wn;

    return Yn;
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::directFormITransposed(int channel, SampleType inputSample)
{
    auto& Wn1 = Wn_1[(size_t)channel];
    auto& Wn2 = Wn_2[(size_t)channel];
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Wn = loop;
    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Wn = (Xn + Wn2);
    Yn = ((Wn * b[0]) + Xn2);

    Xn2 = ((Wn * b[1]) + Xn1); 
    Wn2 = ((Wn * a[1]) + Wn1);
    Xn1 = (Wn * b[2]); 
    Wn1 = (Wn * a[2]);

    return Yn;
}

template <typename SampleType>
SampleType OrfanidisPeak<SampleType>::directFormIITransposed(int channel, SampleType inputSample)
{
    auto& Xn1 = Xn_1[(size_t)channel];
    auto& Xn2 = Xn_2[(size_t)channel];

    auto& Xn = inputSample;
    auto& Yn = outputSample;

    Yn = ((Xn * b[0]) + (Xn2));

    Xn2 = ((Xn * b[1]) + (Xn1) + (Yn * a[1]));
    Xn1 = ((Xn * b[2]) + (Yn * a[2]));

    return Yn;
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::coefficients()
{
    G0 = one;
    G = juce::Decibels::decibelsToGain(gain); // Linear gain
    GB = juce::Decibels::decibelsToGain(gain / root2); //Bandwidth gain
    w0 = frq * omega; // RadSampFreq
    Dw = (frq * omega) / (SampleType(1.588308819) * res); //RadSampBW

    if (G == GB)    // if no boost or cut, pass audio
    {
        a_[0] = one;
        a_[1] = zero;
        a_[2] = zero;
        b_[0] = one;
        b_[1] = zero;
        b_[2] = zero;
    }

    else            // else calculate coefficients
    {
        calcs();

        a_[0] = one;
        a_[1] = minusTwo * ((one - G0W2) / onePlusW2A);
        a_[2] = (one + G0W2 - A) / onePlusW2A;
        b_[0] = (G1 + G0W2 + B) / onePlusW2A;
        b_[1] = minusTwo * ((G1 - G0W2) / onePlusW2A);
        b_[2] = (G1 + G0W2 - B) / onePlusW2A;
    }

    a[0] = (one / a_[0]);
    a[1] = ((-a_[1]) * a[0]);
    a[2] = ((-a_[2]) * a[0]);
    b[0] = (b_[0] * a[0]);
    b[1] = (b_[1] * a[0]);
    b[2] = (b_[2] * a[0]);
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::calcs()
{
    auto powTwo = [&] (SampleType x) { return (x * x); };

    const auto& Gpow2 = powTwo(G);
    const auto& GBpow2 = powTwo(GB);
    const auto& G0pow2 = powTwo(G0);
    const auto& piPow2 = powTwo(pi);
    const auto& w0pow2 = powTwo(w0);
    const auto& DwPow2 = powTwo(Dw);

    F = std::abs(Gpow2 - GBpow2);
    G00 = std::abs(Gpow2 - G0pow2);
    F00 = std::abs(GBpow2 - G0pow2);

    omegaPiTwo = std::pow((w0pow2 - piPow2), two);

    num = G0pow2 * omegaPiTwo + Gpow2 * F00 * piPow2 * DwPow2 / F;
    den = omegaPiTwo + F00 * piPow2 * DwPow2 / F;

    G1 = std::sqrt(num / den);

    const auto& G1pow2 = powTwo(G1);

    G0G1 = G0 * G1;

    G01 = std::abs(Gpow2 - G0G1);
    G11 = std::abs(Gpow2 - G1pow2);
    F01 = std::abs(GBpow2 - G0G1);
    F11 = std::abs(GBpow2 - G1pow2);

    GsqD = std::sqrt(G11 / G00);
    GsqX = std::sqrt(G00 * G11);
    Gsq = G01 - GsqX;

    FsqD = std::sqrt(F00 / F11);
    FsqX = std::sqrt(F00 * F11);
    Fsq = F01 - FsqX;

    W2 = GsqD * std::pow(std::tan(w0 / two), two);
    DW = (one + FsqD * W2) * std::tan(Dw / two);

    const auto& DWpow2 = powTwo(DW);

    G0W2 = G0 * W2;

    C = F11 * DWpow2 - two * W2 * Fsq;
    D = two * W2 * Gsq;

    A = std::sqrt((C + D) / F);
    B = std::sqrt(((Gpow2 * C) + (GBpow2 * D)) / F);

    onePlusW2A = one + W2 + A;
}

template <typename SampleType>
void OrfanidisPeak<SampleType>::snapToZero() noexcept
{
    for (auto v : { &Wn_1, &Wn_2, &Xn_1, &Xn_2, &Yn_1, &Yn_2 })
        for (auto& element : *v)
            juce::dsp::util::snapToZero(element);
}

//template <typename SampleType>
//void OrfanidisPeak<SampleType>::coefficients()
//{
//    const SampleType F = std::abs((G * G) - (GB * GB));
//    const SampleType G00 = std::abs((G * G) - (G0 * G0));
//    const SampleType F00 = std::abs((GB * GB) - (G0 * G0));
//
//    const SampleType num = G0 * G0 * std::pow(w0 * w0 - pi * pi, two) + G * G * F00 * pi * pi * Dw * Dw / F;
//    const SampleType den = std::pow(w0 * w0 - pi * pi, two) + F00 * pi * pi * Dw * Dw / F;
//
//    const SampleType G1 = std::sqrt(num / den);
//
//    const SampleType G01 = std::abs((G * G) - (G0 * G1));
//    const SampleType G11 = std::abs((G * G) - (G1 * G1));
//    const SampleType F01 = std::abs((GB * GB) - (G0 * G1));
//    const SampleType F11 = std::abs((GB * GB) - (G1 * G1));
//
//    const SampleType W2 = std::sqrt(G11 / G00) * std::pow(std::tan(w0 / two), two);
//    const SampleType DW = (one + std::sqrt(F00 / F11) * W2) * std::tan(Dw / two);
//
//    const SampleType C = F11 * DW * DW - two * W2 * (F01 - std::sqrt(F00 * F11));
//    const SampleType D = two * W2 * (G01 - std::sqrt(G00 * G11));
//
//    const SampleType A = std::sqrt((C + D) / F);
//    const SampleType B = std::sqrt((G * G * C + GB * GB * D) / F);
//
//    b_0 = (G1 + G0 * W2 + B) / (one + W2 + A);
//    b_1 = minusTwo * ((G1 - G0 * W2) / (one + W2 + A));
//    b_2 = (G1 - B + G0 * W2) / (one + W2 + A);
//    a_0 = one;
//    a_1 = minusTwo * ((one - W2) / (one + W2 + A));
//    a_2 = (one + W2 - A) / (one + W2 + A);
//}

//==============================================================================
template class OrfanidisPeak<float>;
template class OrfanidisPeak<double>;
