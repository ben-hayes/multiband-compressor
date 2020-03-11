#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LabelledSlider : public GroupComponent
{
public:
    LabelledSlider(
        const String& name,
        Slider::SliderStyle style,
        Slider::TextEntryBoxPosition textPos);
    
    void resized() override;
    void paint(Graphics& g) override;

    Slider slider;
};

class CompressorComponent : public GroupComponent
{
public:
    CompressorComponent(const String& name);
    void resized() override;

    LabelledSlider attack {
        "Attack",
        Slider::RotaryHorizontalVerticalDrag,
        Slider::TextBoxBelow};
    LabelledSlider release {
        "Release",
        Slider::RotaryHorizontalVerticalDrag,
        Slider::TextBoxBelow};
    LabelledSlider ratio {
        "Ratio",
        Slider::RotaryHorizontalVerticalDrag,
        Slider::TextBoxBelow};
    LabelledSlider knee {
        "Knee",
        Slider::RotaryHorizontalVerticalDrag,
        Slider::TextBoxBelow};
    LabelledSlider makeupGain {
        "Make-up Gain",
        Slider::RotaryHorizontalVerticalDrag,
        Slider::TextBoxBelow};
    LabelledSlider threshold {
        "Threshold",
        Slider::LinearVertical,
        Slider::TextBoxBelow};
};