/*
  ==============================================================================

    CrossoverFilterProcessor.cpp
    Created: 10 Mar 2020 1:14:53pm
    Author:  Ben Hayes

  ==============================================================================
*/

#include "CrossoverFilterProcessor.h"

CrossoverFilterProcessor::CrossoverFilterProcessor(int index)
    : low_pass_filter_0(
          dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f)),
      low_pass_filter_1(
          dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f)),
      high_pass_filter_0(
          dsp::IIR::Coefficients<float>::makeHighPass(44100, 20000.0f)),
      high_pass_filter_1(
          dsp::IIR::Coefficients<float>::makeHighPass(44100, 20000.0f))
{
    // We only need to expose one parameter: the crossover frequency.
    auto index_str = std::to_string(index + 1);
    cutoff_frequency_in_hz_ =
        new AudioParameterFloat(
            "cutoff_" + index_str,
            "Crossover " + index_str + " Frequency",
            40.0,
            20000.0,
            1000.0);
}

CrossoverFilterProcessor::~CrossoverFilterProcessor()
{
}

void CrossoverFilterProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    // Store a local copy of the sample rate
    sample_rate_in_hz_ = sampleRate;

    // juce::dsp objects require a little extra setup. In particular, they
    // require an instance of the dsp::ProcessSpec struct on preparation to
    // play.
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;

    low_pass_filter_0.prepare(spec);
    low_pass_filter_1.prepare(spec);
    high_pass_filter_0.prepare(spec);
    high_pass_filter_1.prepare(spec);
}

void CrossoverFilterProcessor::processBlock(
    AudioSampleBuffer& buffer,
    MidiBuffer& midiMessages)
{
    // Make a two channel buffer that points to the first two channels in
    // the buffer passed into the function
    AudioSampleBuffer low_buffer{
        buffer.getArrayOfWritePointers(),
        2,
        buffer.getNumSamples()};
    
    // Make another two channel buffer containing a copy of the first two
    // channels, so that we can process it independently:
    AudioSampleBuffer high_buffer{2, buffer.getNumSamples()};
    high_buffer.makeCopyOf(low_buffer);

    updateCoefficients();

    // juce::dsp processors require input to be an AudioBlock<T> instance,
    // so we need to construct these from our processing buffers
    dsp::AudioBlock<float> low_block (low_buffer);
    dsp::AudioBlock<float> high_block (high_buffer);

    // Process one pair of channels with a cascade of second order low pass
    // filters. These filters were constructed inside of an instance of
    // dsp::ProcessorDuplicator which takes care of processing multiple 
    // channels independently.
    low_pass_filter_0.process(dsp::ProcessContextReplacing<float> (low_block));
    low_pass_filter_1.process(dsp::ProcessContextReplacing<float> (low_block));

    // Process the other pair of channels with a cascade of second order high
    // pass filters:
    high_pass_filter_0.process(dsp::ProcessContextReplacing<float> (high_block));
    high_pass_filter_1.process(dsp::ProcessContextReplacing<float> (high_block));

    // low_buffer already pointed to the raw sample data, so we don't need to 
    // copy that, but high_buffer needs copying into the 3rd and 4th channel
    // of the output buffer:
    buffer.copyFrom(2, 0, high_block.getChannelPointer(0), buffer.getNumSamples());
    buffer.copyFrom(3, 0, high_block.getChannelPointer(1), buffer.getNumSamples());

}

std::unique_ptr<AudioProcessorParameterGroup> 
    CrossoverFilterProcessor::getParameterTree()
{
    // Return a unique pointer to a tree containing a reference to our
    // parameter and let the PluginProcessor take care of it. Makes our class
    // less portable but prevents memory leaks.
    auto parameter_tree = std::make_unique<AudioProcessorParameterGroup>();
    parameter_tree->addChild(
        std::unique_ptr<AudioParameterFloat>(cutoff_frequency_in_hz_));
    return std::move(parameter_tree);
}

void CrossoverFilterProcessor::reset()
{
    // Pass the reset call onto our filters
    low_pass_filter_0.reset();
    low_pass_filter_1.reset();
    high_pass_filter_0.reset();
    high_pass_filter_1.reset();
}

void CrossoverFilterProcessor::updateCoefficients()
{
    // Calculate coefficients using makeLowPass and makeHighPass static methods
    // from dsp::IIR::Coefficients class.
    // Not passing in a Q factor defaults to a Q factor of 1/sqrt(2), and
    // therefore returns a second order Butterworth filter. This function
    // also takes care of the frequency pre-warping
    auto lpf_coeffs = *dsp::IIR::Coefficients<float>::makeLowPass(
        sample_rate_in_hz_,
        cutoff_frequency_in_hz_->get());
    auto hpf_coeffs = *dsp::IIR::Coefficients<float>::makeHighPass(
        sample_rate_in_hz_,
        cutoff_frequency_in_hz_->get());
    
    // Update the filters to use the calculated coefficients:
    *low_pass_filter_0.state = lpf_coeffs;
    *low_pass_filter_1.state = lpf_coeffs;
    *high_pass_filter_0.state = hpf_coeffs;
    *high_pass_filter_1.state = hpf_coeffs;
}