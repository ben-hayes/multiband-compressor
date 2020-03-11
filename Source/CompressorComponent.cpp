#include "PluginProcessor.h"
#include "CompressorComponent.h"

LabelledSlider::LabelledSlider(
    const String& name,
    Slider::SliderStyle style,
    Slider::TextEntryBoxPosition textPos)
{
    setText(name);
    setTextLabelPosition(Justification::centredTop);

    slider.setSliderStyle(style);
    slider.setTextBoxStyle(
        textPos,
        false,
        getLocalBounds().reduced(10).getWidth(),
        10);
    addAndMakeVisible(&slider);
}

void LabelledSlider::resized()
{
    slider.setBounds(getLocalBounds().reduced(13));
    slider.setTextBoxStyle(
        slider.getTextBoxPosition(),
        true,
        slider.getWidth(),
        13);
}

void LabelledSlider::paint(Graphics& g)
{
    // make border transparent
    setColour(
        outlineColourId,
        Colour());
    GroupComponent::paint(g);
}

CompressorComponent::CompressorComponent(
        const String& name,
        CompressorProcessor* p)
    : p (p)
{
    setText(name);
    setTextLabelPosition(Justification::topLeft);

    attack.slider.setRange(1.0, 150.0, 1.0);
    attack.slider.setValue(1.0);
    attack.slider.setTextValueSuffix(" ms");

    release.slider.setRange(1.0, 1000.0, 1.0);
    release.slider.setValue(1.0);
    release.slider.setTextValueSuffix(" ms");

    ratio.slider.setRange(1.0, 20.0, 0.1);
    ratio.slider.setValue(1.0);
    ratio.slider.setTextValueSuffix("");

    knee.slider.setRange(0.0, 32.0, 1.0);
    knee.slider.setValue(0.0);
    knee.slider.setTextValueSuffix(" dB");

    makeupGain.slider.setRange(0.0, 32.0, 0.1);
    makeupGain.slider.setValue(0.0);
    makeupGain.slider.setTextValueSuffix(" dB");

    threshold.slider.setRange(-66.6, 0.0, 0.1);
    threshold.slider.setValue(0.0);
    threshold.slider.setTextValueSuffix(" dB");

    addAndMakeVisible(&attack);
    addAndMakeVisible(&release);
    addAndMakeVisible(&ratio);
    addAndMakeVisible(&knee);
    addAndMakeVisible(&makeupGain);
    addAndMakeVisible(&threshold);
}

void CompressorComponent::resized()
{
    attack.setBounds(0, 20, 90, 40);
    release.setBounds(0, 57, 90, 40);
    ratio.setBounds(0, 94, 90, 40);
    knee.setBounds(0, 131, 90, 40);
    threshold.setBounds(70, 20, 80, 151);
    makeupGain.setBounds(130, 20, 80, 151);
}

void CompressorComponent::attachToProcessor()
{
    attack.slider.onValueChange = [this] {
        *(p->attack_in_seconds_) =
            attack.slider.getValue();
    };
    release.slider.onValueChange = [this] {
        *(p->release_in_seconds_) =
            release.slider.getValue();
    };
    ratio.slider.onValueChange = [this] {
        *(p->ratio_) = ratio.slider.getValue();
    };
    knee.slider.onValueChange = [this] {
        *(p->knee_in_db_) = knee.slider.getValue();
    };
    makeupGain.slider.onValueChange = [this] {
        *(p->makeup_gain_in_db_) =
            makeupGain.slider.getValue();
    };
    threshold.slider.onValueChange = [this] {
        *(p->threshold_in_db_) =
            threshold.slider.getValue();
    };
}