/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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
    for (auto i = 0; i < NUM_COMPRESSORS; i++)
    {
        compressor_processors_.add(
            std::make_unique<CompressorProcessor>(i));
    }
    for (auto i = 0; i < NUM_CROSSOVERS; i++)
    {
        crossover_processors_.add(
            std::make_unique<CrossoverFilterProcessor>(i));
    }
    for (auto& compressor_processor : compressor_processors_)
    {
        for (auto param : compressor_processor->getParameters())
        {
            addParameter(param);
        }
        compressor_nodes_.add(
            processor_graph_->addNode(std::move(compressor_processor))); 
    }
    for (auto& crossover_processor : crossover_processors_)
    {
        for (auto param : crossover_processor->getParameters())
        {
            addParameter(param);
        }
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
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
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    processor_graph_->processBlock(buffer, midiMessages);
}

//==============================================================================
bool MultibandCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MultibandCompressorAudioProcessor::createEditor()
{
    return new MultibandCompressorAudioProcessorEditor (*this);
}

//==============================================================================
void MultibandCompressorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MultibandCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void MultibandCompressorAudioProcessor::initialiseGraph()
{
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
        node->getProcessor()->setPlayConfigDetails(
            2,
            4,
            getSampleRate(),
            getBlockSize());
    }

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
    for (auto channel = 0; channel < 2; channel++)
        processor_graph_->addConnection(
            {{audio_input_node_->nodeID, channel},
            {crossover_nodes_[0]->nodeID, channel}});
    
    for (auto i = 0; i < crossover_nodes_.size(); i++)
    {
        for (auto channel = 0; channel < 2; channel++)
        {
            processor_graph_->addConnection(
                {{crossover_nodes_[i]->nodeID, channel},
                {compressor_nodes_[i]->nodeID, channel}});
            
            if (i < crossover_nodes_.size() - 1)
            {
                processor_graph_->addConnection(
                    {{crossover_nodes_[i]->nodeID, channel + 2},
                    {crossover_nodes_[i + 1]->nodeID, channel}});
            } else
            {
                processor_graph_->addConnection(
                    {{crossover_nodes_[i]->nodeID, channel + 2},
                    {compressor_nodes_[i + 1]->nodeID, channel}});
            }
            
        }
    }

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
