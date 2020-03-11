#include "PluginProcessor.h"
#include "PluginEditor.h"

#define NUM_COMPRESSORS 5
#define NUM_CROSSOVERS NUM_COMPRESSORS - 1

//==============================================================================
MultibandCompressorAudioProcessor::MultibandCompressorAudioProcessor()
    : AudioProcessor (BusesProperties()
                    .withInput  ("Input",  AudioChannelSet::stereo(), true)
                    .withOutput ("Output", AudioChannelSet::stereo(), true)),
      processor_graph_ (std::make_unique<AudioProcessorGraph>())
{
    // To setup the plugin processor, we need to create our compressors and
    // crossover filters. These are all assigned dynamically, so we can set
    // the number of compressors using the NUM_COMPRESSORS macro at the top of
    // this file. The AudioProcessorGraph and the plugin's editor will then
    // automatically adapt.

    // First we create NUM_COMPRESSORS compressors:
    for (auto i = 0; i < NUM_COMPRESSORS; i++)
    {
        // Make a unique pointer to a compressor
        auto compressor_processor = std::make_unique<CompressorProcessor>(i);

        // Pull out its parameters
        addParameterGroup(compressor_processor->getParameterTree());

        // Add them to the processor graph and store a reference to the
        // resulting node.
        compressor_nodes_.add(
            processor_graph_->addNode(std::move(compressor_processor))); 
    }

    // We do exactly the same for our crossover filters, but we store their
    // nodes in a separate juce::Array, so that we can easily access them:
    for (auto i = 0; i < NUM_CROSSOVERS; i++)
    {
        auto crossover_processor =
            std::make_unique<CrossoverFilterProcessor>(i);
        addParameterGroup(crossover_processor->getParameterTree());
        crossover_nodes_.add(
            processor_graph_->addNode(std::move(crossover_processor)));
    }
}

MultibandCompressorAudioProcessor::~MultibandCompressorAudioProcessor()
{
}

//==============================================================================
const String MultibandCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultibandCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultibandCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultibandCompressorAudioProcessor::getNumPrograms()
{
    return 1;
}

int MultibandCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultibandCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const String MultibandCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultibandCompressorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MultibandCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // We tell the graph how many input and output channels we would like,
    // and initialise it:
    processor_graph_->setPlayConfigDetails(
        getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        sampleRate,
        samplesPerBlock);
    initialiseGraph();
    processor_graph_->prepareToPlay(sampleRate, samplesPerBlock);

}

void MultibandCompressorAudioProcessor::releaseResources()
{
    processor_graph_->releaseResources();
}

bool MultibandCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void MultibandCompressorAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    // Thanks to abstractions, our processBlock is pretty simple.

    // First, we run some JUCE boilerplate code that clears out any superfluous
    // channels in the buffer:
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Then, we simply pass our buffer into our processor graph, and it will
    // cascade through all of our processors.
    processor_graph_->processBlock(buffer, midiMessages);
}

//==============================================================================
bool MultibandCompressorAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* MultibandCompressorAudioProcessor::createEditor()
{
    return new MultibandCompressorAudioProcessorEditor (*this);
}

//==============================================================================
void MultibandCompressorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
}

void MultibandCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

void MultibandCompressorAudioProcessor::initialiseGraph()
{
    // Simply iterate over our compressor and crossover nodes, calling
    // setPlayConfigDetails.
    for (auto& node : compressor_nodes_)
    {
        node->getProcessor()->setPlayConfigDetails(
            getMainBusNumInputChannels(),
            getMainBusNumOutputChannels(),
            getSampleRate(),
            getBlockSize());
    }
    for (auto& node : crossover_nodes_)
    {
        // Note that each crossover takes a stereo input and gives two stereo
        // outputs
        node->getProcessor()->setPlayConfigDetails(
            2,
            4,
            getSampleRate(),
            getBlockSize());
    }

    // We setup our input and output nodes, and connect up our graph:
    audio_input_node_ = processor_graph_->addNode(
        std::make_unique<AudioGraphIOProcessor>(
            AudioGraphIOProcessor::audioInputNode));
    audio_output_node_ = processor_graph_->addNode(
        std::make_unique<AudioGraphIOProcessor>(
            AudioGraphIOProcessor::audioOutputNode));

    connectAudioNodes();
}

void MultibandCompressorAudioProcessor::connectAudioNodes()
{
    // Firstly, we setup our input node, and connect it to our first crossover
    for (auto channel = 0; channel < 2; channel++)
        processor_graph_->addConnection(
            {{audio_input_node_->nodeID, channel},
            {crossover_nodes_[0]->nodeID, channel}});

    // Then we iterate over our crossovers and channels: 
    for (auto i = 0; i < crossover_nodes_.size(); i++)
    {
        for (auto channel = 0; channel < 2; channel++)
        {
            // For the first stereo output of each crossover, we connect it
            // to the compressor with the same index:
            processor_graph_->addConnection(
                {{crossover_nodes_[i]->nodeID, channel},
                {compressor_nodes_[i]->nodeID, channel}});
            
            if (i < crossover_nodes_.size() - 1)
            {
                // For the second stereo output, if we are not at our last
                // crossover filter, we connect it to the next crossover...
                processor_graph_->addConnection(
                    {{crossover_nodes_[i]->nodeID, channel + 2},
                    {crossover_nodes_[i + 1]->nodeID, channel}});
            } else
            {
                // But if it's our last crossover, we connect its high pass
                // output to our final compressor
                processor_graph_->addConnection(
                    {{crossover_nodes_[i]->nodeID, channel + 2},
                    {compressor_nodes_[i + 1]->nodeID, channel}});
            }
            
        }
    }

    // And finally, we sum the outputs of all our compressors:
    for (auto& compressor_node : compressor_nodes_)
    {
        for (auto channel = 0; channel < 2; channel++)
            processor_graph_->addConnection(
                {{compressor_node->nodeID, channel},
                {audio_output_node_->nodeID, channel}});
    }
}

void MultibandCompressorAudioProcessor::updateGraph()
{
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultibandCompressorAudioProcessor();
}
