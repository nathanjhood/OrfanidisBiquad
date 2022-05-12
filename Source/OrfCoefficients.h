/*
  ==============================================================================

    OrfCoefficients.h
    Created: 12 May 2022 8:35:45pm
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#ifndef ORFCOEFFICIENTS_H_INCLUDED
#define ORFCOEFFICIENTS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

template <typename SampleType>
class OrfCoefficients
{
public:
    //==============================================================================
    /** Constructor. */
    OrfCoefficients();

    //==============================================================================
    SampleType b0() { return static_cast<SampleType>(b0_); };
    SampleType b1() { return static_cast<SampleType>(b1_); };
    SampleType b2() { return static_cast<SampleType>(b2_); };
    SampleType a0() { return static_cast<SampleType>(a0_); };
    SampleType a1() { return static_cast<SampleType>(a1_); };
    SampleType a2() { return static_cast<SampleType>(a2_); };

    //==============================================================================
    SampleType pow(SampleType x)
    {
        return ((x*x));
    }

    SampleType sqrt(SampleType x)
    {
        return (std::exp2(std::log2(x) * static_cast<SampleType>(0.5)));
    }

    SampleType sqrtAmulB(SampleType a, SampleType b) 
    {
        return sqrt((a*b));
    }

    SampleType sqrtAdivB(SampleType a, SampleType b)
    {
        return sqrt((a/b));
    }

    SampleType powAminB(SampleType a, SampleType b)
    {
        return pow((a-b));
    }

    SampleType absAminB(SampleType a, SampleType b)
    {
        return std::abs((a-b));
    }

    SampleType tan(SampleType x)
    {
        return (std::sin(x) / (std::cos(x)));
    }

    SampleType tanAdivB(SampleType a, SampleType b)
    {
        return tan((a/b));
    }

    //==============================================================================
    void calculateCoefficients(SampleType G0, SampleType G, SampleType GB, SampleType w0, SampleType Dw);

private:
    //==============================================================================
    SampleType b0_ = 1.0;
    SampleType b1_ = 0.0;
    SampleType b2_ = 0.0;
    SampleType a0_ = 1.0;
    SampleType a1_ = 0.0;
    SampleType a2_ = 0.0;

    //==============================================================================
    const SampleType pi = static_cast<SampleType>(juce::MathConstants<SampleType>::pi);
    const SampleType root2 = sqrt(static_cast<SampleType>(2.0));

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrfCoefficients)
};

#endif //ORFCOEFFICIENTS_H_INCLUDED