/*
    ==============================================================================

      This file was auto-generated!

      It contains the basic framework code for a JUCE plugin editor.

    ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultibandCompressorAudioProcessorEditor::MultibandCompressorAudioProcessorEditor (MultibandCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 340);
    //compressor.attack.slider.onValueChange = [this] {
    //    *(processor.cmp->attack_in_seconds_) =
    //        compressor.attack.slider.getValue();
    //};
    //compressor.release.slider.onValueChange = [this] {
    //    *(processor.cmp->release_in_seconds_) =
    //        compressor.release.slider.getValue();
    //};
    //compressor.ratio.slider.onValueChange = [this] {
    //    *(processor.cmp->ratio_) = compressor.ratio.slider.getValue();
    //};
    //compressor.knee.slider.onValueChange = [this] {
    //    *(processor.cmp->knee_in_db_) = compressor.knee.slider.getValue();
    //};
    //compressor.makeupGain.slider.onValueChange = [this] {
    //    *(processor.cmp->makeup_gain_in_db_) =
    //        compressor.makeupGain.slider.getValue();
    //};
    //compressor.threshold.slider.onValueChange = [this] {
    //    *(processor.cmp->threshold_in_db_) =
    //        compressor.threshold.slider.getValue();
    //};
    addAndMakeVisible(&compressor);
}

MultibandCompressorAudioProcessorEditor::~MultibandCompressorAudioProcessorEditor()
{
}

//==============================================================================
void MultibandCompressorAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

//    g.setColour (Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void MultibandCompressorAudioProcessorEditor::resized()
{
    compressor.setBounds(10, 10, 250, 320);
}
