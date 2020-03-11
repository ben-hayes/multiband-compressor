#pragma once

#include <JuceHeader.h>
#include "CompressorProcessor.h"
#include "CrossoverFilterProcessor.h"

using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
using Node = AudioProcessorGraph::Node;
//==============================================================================
/**
*/
class MultibandCompressorAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    MultibandCompressorAudioProcessor();
    ~MultibandCompressorAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    Array<Node::Ptr> crossover_nodes_;
    Array<Node::Ptr> compressor_nodes_;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressorAudioProcessor)
    void initialiseGraph();
    void connectAudioNodes();
    void updateGraph();

    std::unique_ptr<AudioProcessorGraph> processor_graph_;

    Node::Ptr audio_input_node_;
    Node::Ptr audio_output_node_;
};
