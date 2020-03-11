#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LabelledSlider : public GroupComponent
{
// A GroupComponent derivative that creates a slider with an attached label
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
// A GroupComponent derivative that wraps all the controls necessary for a
// compressor processor.
public:
    CompressorComponent(const String& name, CompressorProcessor* p);
    void resized() override;
    void attachToProcessor();

    LabelledSlider attack {
        "Attack",
        Slider::LinearBar,
        Slider::TextBoxBelow};
    LabelledSlider release {
        "Release",
        Slider::LinearBar,
        Slider::TextBoxBelow};
    LabelledSlider ratio {
        "Ratio",
        Slider::LinearBar,
        Slider::TextBoxBelow};
    LabelledSlider knee {
        "Knee",
        Slider::LinearBar,
        Slider::TextBoxBelow};
    LabelledSlider makeupGain {
        "Make-up Gain",
        Slider::LinearVertical,
        Slider::TextBoxBelow};
    LabelledSlider threshold {
        "Threshold",
        Slider::LinearVertical,
        Slider::TextBoxBelow};
private:
    CompressorProcessor* p;
};