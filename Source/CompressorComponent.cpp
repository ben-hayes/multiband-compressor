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
    slider.setBounds(getLocalBounds().reduced(10));
    slider.setTextBoxStyle(
        slider.getTextBoxPosition(),
        true,
        slider.getWidth(),
        13);
}

void LabelledSlider::paint(Graphics& g)
{
    setColour(
        outlineColourId,
        getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    GroupComponent::paint(g);
}

CompressorComponent::CompressorComponent(const String& name)
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

    threshold.slider.setRange(-72.0, 0.0, 0.1);
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
    attack.setBounds(10, 13, 80, 100);
    release.setBounds(90, 13, 80, 100);
    ratio.setBounds(10, 113, 80, 100);
    knee.setBounds(90, 113, 80, 100);
    makeupGain.setBounds(90, 213, 80, 100);
    threshold.setBounds(170, 13, 70, 300);
}