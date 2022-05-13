/*
  ==============================================================================

    OrfCoefficients.cpp
    Created: 12 May 2022 8:35:45pm
    Author:  StoneyDSP

  ==============================================================================
*/

#include "OrfCoefficients.h"

//==============================================================================
template <typename SampleType>
OrfCoefficients<SampleType>::OrfCoefficients()
{

}

template <typename SampleType>
void OrfCoefficients<SampleType>::efg(SampleType G, SampleType GB, SampleType w0, SampleType Dw)
{
    const SampleType G0 = static_cast<SampleType>(1.0);
    const SampleType G0pow2 = pow(G0);
    const SampleType Gpow2 = pow(G);
    const SampleType GBpow2 = pow(GB);
    const SampleType w0pow2 = pow(w0);
    const SampleType Dwpow2 = pow(Dw);
    const SampleType pipow2 = pow(pi);

    auto radPiTwo = powAminB(w0pow2, pipow2);

    auto F = absAminB(Gpow2, GBpow2);
    auto G00 = absAminB(Gpow2, G0pow2);
    auto F00 = absAminB(GBpow2, G0pow2);



}

template <typename SampleType>
void OrfCoefficients<SampleType>::calculateCoefficients(SampleType G0, SampleType G, SampleType GB, SampleType w0, SampleType Dw)
{
    if (G == GB)
    {
        b0_ = SampleType (1.0);
        b1_ = SampleType (0.0);
        b2_ = SampleType (0.0);
        a0_ = SampleType (1.0);
        a1_ = SampleType (0.0);
        a2_ = SampleType (0.0);
    }
    else
    {
        const SampleType G0pow2 = pow(G0);
        const SampleType Gpow2 = pow(G);
        const SampleType GBpow2 = pow(GB);
        const SampleType w0pow2 = pow(w0);
        const SampleType Dwpow2 = pow(Dw);
        const SampleType pipow2 = pow(pi);

        const SampleType F = absAminB(Gpow2, GBpow2);
        const SampleType G00 = absAminB(Gpow2, G0pow2);
        const SampleType F00 = absAminB(GBpow2, G0pow2);

        const SampleType radPiPow = w0pow2 - pipow2;

        const SampleType num = G0pow2 * pow(radPiPow) + Gpow2 * F00 * pipow2 * Dwpow2 / F;
        const SampleType den = pow(radPiPow) + F00 * pipow2 * Dwpow2 / F;

        const SampleType G1 = sqrtAdivB(num, den);
        const SampleType G1pow2 = pow(G1);

        const SampleType G01 = absAminB(Gpow2, (G0*G1));
        const SampleType G11 = absAminB(Gpow2, G1pow2);
        const SampleType F01 = absAminB(GBpow2, (G0 * G1));
        const SampleType F11 = absAminB(GBpow2, G1pow2);

        const SampleType W2 = sqrtAdivB(G11, G00) * pow(tanAdivB(w0, SampleType(2.0)));
        const SampleType DW = (SampleType(1.0) + sqrtAdivB(F00, F11) * W2) * tanAdivB(Dw, SampleType(2.0));

        const SampleType C = F11 * DW * DW - SampleType(2.0) * W2 * (F01 - sqrtAmulB(F00, F11));
        const SampleType D = SampleType(2.0) * W2 * (G01 - sqrtAmulB(G00, G11));

        const SampleType A = sqrt(((C + D) / F));
        const SampleType B = sqrt(((Gpow2 * C + GBpow2 * D) / F));

        const SampleType div = SampleType(1.0) + W2 + A;

        b0_ = ((G1 + G0 * W2 + B) / div);
        b1_ = (SampleType(-2.0) * ((G1 - G0 * W2) / div));
        b2_ = ((G1 - B + G0 * W2) / div);
        a0_ = (1.0);
        a1_ = (SampleType(-2.0) * ((SampleType(1.0) - W2) / div));
        a2_ = ((SampleType(1.0) + W2 - A) / div);
    }
}

template class OrfCoefficients<float>;
template class OrfCoefficients<double>;