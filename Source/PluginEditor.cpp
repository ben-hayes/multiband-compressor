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
    auto num_compressors = processor.compressor_processors_.size();
    auto width = num_compressors * 220 + 10;
    setSize (width, 190);

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
    auto num_compressors = processor.compressor_processors_.size();
    for (auto i = 0; i < num_compressors; i++)
    {
        auto index = std::to_string(i + 1);
        auto name = "Band " + index;
        auto compressor_processor = dynamic_cast<CompressorProcessor*>(
                processor.compressor_nodes_[i]->getProcessor());
        auto compressor_editor = std::make_shared<CompressorComponent>(
            name,
            compressor_processor);
        addAndMakeVisible(compressor_editor.get());
        auto start = 10 + i * 220;
        compressor_editor->setBounds(start, 10, 210, 170);
        compressor_editor->attachToProcessor();
        compressor_editors_.add(compressor_editor);
    }
}
