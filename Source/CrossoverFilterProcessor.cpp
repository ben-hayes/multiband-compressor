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
    auto index_str = std::to_string(index + 1);
    cutoff_frequency_in_hz_ =
        new AudioParameterFloat(
            "cutoff_" + index_str,
            "Crossover " + index_str + " Frequency",
            40.0,
            20000.0,
            1000.0);
    addParameter(cutoff_frequency_in_hz_);
}

CrossoverFilterProcessor::~CrossoverFilterProcessor()
{
}

void CrossoverFilterProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    sample_rate_in_hz_ = sampleRate;

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
    AudioSampleBuffer low_buffer{
        buffer.getArrayOfWritePointers(),
        2,
        buffer.getNumSamples()};
    AudioSampleBuffer high_buffer{2, buffer.getNumSamples()};
    high_buffer.makeCopyOf(low_buffer);

    updateCoefficients();
    dsp::AudioBlock<float> low_block (low_buffer);
    dsp::AudioBlock<float> high_block (high_buffer);

    low_pass_filter_0.process(dsp::ProcessContextReplacing<float> (low_block));
    low_pass_filter_1.process(dsp::ProcessContextReplacing<float> (low_block));

    high_pass_filter_0.process(dsp::ProcessContextReplacing<float> (high_block));
    high_pass_filter_1.process(dsp::ProcessContextReplacing<float> (high_block));

    buffer.copyFrom(2, 0, high_block.getChannelPointer(0), buffer.getNumSamples());
    buffer.copyFrom(3, 0, high_block.getChannelPointer(1), buffer.getNumSamples());

}

void CrossoverFilterProcessor::reset()
{
    low_pass_filter_0.reset();
    low_pass_filter_1.reset();
    high_pass_filter_0.reset();
    high_pass_filter_1.reset();
}

void CrossoverFilterProcessor::updateCoefficients()
{
    auto lpf_coeffs = *dsp::IIR::Coefficients<float>::makeLowPass(
        sample_rate_in_hz_,
        cutoff_frequency_in_hz_->get());
    auto hpf_coeffs = *dsp::IIR::Coefficients<float>::makeHighPass(
        sample_rate_in_hz_,
        cutoff_frequency_in_hz_->get());
    *low_pass_filter_0.state = lpf_coeffs;
    *low_pass_filter_1.state = lpf_coeffs;
    *high_pass_filter_0.state = hpf_coeffs;
    *high_pass_filter_1.state = hpf_coeffs;
}