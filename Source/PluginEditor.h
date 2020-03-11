/*
    ==============================================================================

      This file was auto-generated!

      It contains the basic framework code for a JUCE plugin editor.

    ==============================================================================
*/

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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultibandCompressorAudioProcessor& processor;
    OwnedArray<CompressorComponent> compressor_editors_;
    OwnedArray<Slider> crossover_sliders_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressorAudioProcessorEditor)
};
