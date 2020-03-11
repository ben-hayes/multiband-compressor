#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CompressorComponent.h"

//==============================================================================
/**
*/
class MultibandCompressorAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    MultibandCompressorAudioProcessorEditor (MultibandCompressorAudioProcessor&);
    ~MultibandCompressorAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    MultibandCompressorAudioProcessor& processor;

    OwnedArray<CompressorComponent> compressor_editors_;
    OwnedArray<Slider> crossover_sliders_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressorAudioProcessorEditor)
};
