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

    for (auto i = 0; i < num_compressors; i++)
    {
        auto index = std::to_string(i + 1);
        auto name = "Band " + index;

        auto compressor_processor = dynamic_cast<CompressorProcessor*>(
            processor.compressor_nodes_[i]->getProcessor());

        auto compressor_editor = new CompressorComponent(
            name,
            compressor_processor);

        addAndMakeVisible(compressor_editor);
        compressor_editor->attachToProcessor();
        compressor_editors_.add(compressor_editor);
    }

    for (auto i = 0; i < num_compressors - 1; i++)
    {
        auto initial_value = 40.0 + pow(
            2,
            (i + 3) * log2(20000.0) / (num_compressors + 2));
        auto index = std::to_string(i + 1);
        auto name = "Crossover " + index + " Frequency";

        auto crossover_processor = dynamic_cast<CrossoverFilterProcessor*>(
            processor.crossover_nodes_[i]->getProcessor());
        
        auto crossover_slider = new Slider("crossover_" + index);

        crossover_slider->setSliderStyle(Slider::LinearBar);
        crossover_slider->setRange(40.0, 20000.0, 1.0);
        crossover_slider->setValue(initial_value);
        crossover_slider->setTextValueSuffix(" Hz");
        crossover_slider->setSkewFactor(0.5);
        addAndMakeVisible(crossover_slider);

        crossover_sliders_.add(crossover_slider);
    }

    auto width = num_compressors * 220 + 10;
    setSize (width, 230);
}

MultibandCompressorAudioProcessorEditor::~MultibandCompressorAudioProcessorEditor()
{
}

//==============================================================================
void MultibandCompressorAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MultibandCompressorAudioProcessorEditor::resized()
{
    auto num_compressors = processor.compressor_processors_.size();
    for (auto i = 0; i < num_compressors; i++)
    {
        auto start = 10 + i * 220;
        compressor_editors_[i]->setBounds(start, 10, 210, 170);
    }

    auto num_crossovers = processor.crossover_processors_.size();
    for (auto i = 0; i < num_crossovers; i++)
    {

        auto start = 185 + i * 220;
        crossover_sliders_[i]->setBounds(start, 190, 90, 30);
    }
}
