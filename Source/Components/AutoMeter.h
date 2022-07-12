/*
  ==============================================================================

    AutoMeter.h
    Created: 11 Jul 2022 3:38:12am
    Author:  StoneyDSP

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AutoMeter : public juce::Component, public juce::Timer
{
public:
    AutoMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
    {
        startTimerHz(60);
    }

    void timerCallback() override
    {
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        const auto level = valueSupplier();

        auto bounds = getLocalBounds().toFloat();

        g.setColour(juce::Colours::white.withBrightness(0.4f));
        g.fillRoundedRectangle(bounds, 5.0f);

        g.setColour(juce::Colours::white);
        const auto scaledX = juce::jmap(level, -60.0f, +6.0f, 0.0f, static_cast<float>(getWidth()));
        g.fillRoundedRectangle(bounds.removeFromLeft(scaledX), 5.0f);
    }


private:
    std::function<float()> valueSupplier;
};
