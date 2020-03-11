#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultibandCompressorAudioProcessorEditor::MultibandCompressorAudioProcessorEditor (MultibandCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    auto num_compressors = processor.compressor_nodes_.size();

    // First we create as many compressor editors as we have compressors:
    for (auto i = 0; i < num_compressors; i++)
    {
        // Name it:
        auto index = std::to_string(i + 1);
        auto name = "Band " + index;

        // Get a reference to our compressor processor:
        auto compressor_processor = dynamic_cast<CompressorProcessor*>(
            processor.compressor_nodes_[i]->getProcessor());

        // Setup our compressor editor:
        auto compressor_editor = new CompressorComponent(
            name,
            compressor_processor);

        // Add it to our UI
        addAndMakeVisible(compressor_editor);
        // Hook up its controls to the parameters of the processor
        compressor_editor->attachToProcessor();
        // Store it in a juce::OwnedArray
        compressor_editors_.add(compressor_editor);
    }

    // Then we create as many frequency sliders as we have crossover filters:
    for (auto i = 0; i < num_compressors - 1; i++)
    {
        // Logarithmically divide up the frequency range:
        auto initial_value = 40.0 + pow(
            2,
            (i + 3) * log2(19960.0) / (num_compressors + 2));

        // Get a pointer to the appropriate CrossoverFilterProcessor
        auto crossover_processor = dynamic_cast<CrossoverFilterProcessor*>(
            processor.crossover_nodes_[i]->getProcessor());

        // Create a slider UI control 
        auto index = std::to_string(i + 1);
        auto crossover_slider = new Slider("crossover_" + index);
        // And set all the appropriate parameters:
        crossover_slider->setSliderStyle(Slider::LinearBar);
        crossover_slider->setRange(40.0, 20000.0, 1.0);
        crossover_slider->setValue(initial_value);
        crossover_slider->setTextValueSuffix(" Hz");
        crossover_slider->setSkewFactor(0.4);
        addAndMakeVisible(crossover_slider);

        // Add it to a juce::OwnedArray.
        crossover_sliders_.add(crossover_slider);
    }

    // Set our editor width based on the number of compressors we have:
    auto width = num_compressors * 220 + 10;
    setSize (width, 230);
}

MultibandCompressorAudioProcessorEditor::~MultibandCompressorAudioProcessorEditor()
{
}

//==============================================================================
void MultibandCompressorAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MultibandCompressorAudioProcessorEditor::resized()
{
    // Iterate over our compressors editors and position them:
    auto num_compressors = processor.compressor_nodes_.size();
    for (auto i = 0; i < num_compressors; i++)
    {
        auto start = 10 + i * 220;
        compressor_editors_[i]->setBounds(start, 10, 210, 170);
    }

    // Iterate over our crossover sliders and position them:
    auto num_crossovers = processor.crossover_nodes_.size();
    for (auto i = 0; i < num_crossovers; i++)
    {
        auto start = 185 + i * 220;
        crossover_sliders_[i]->setBounds(start, 190, 90, 30);
    }
}
