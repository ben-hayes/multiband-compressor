/*
  ==============================================================================

    CrossoverFilterProcessor.h
    Created: 10 Mar 2020 1:14:53pm
    Author:  Ben Hayes

  ==============================================================================
*/

#pragma once

#include "ProcessorBase.h"

class CrossoverFilterProcessor : public ProcessorBase
{
public:
    CrossoverFilterProcessor(int index);
    ~CrossoverFilterProcessor();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(
        AudioSampleBuffer& buffer,
        MidiBuffer& midiMessages) override;
    const String getName() const override { return "Crossover Filter"; }
    std::unique_ptr<AudioProcessorParameterGroup> getParameterTree();
    void reset() override;

    AudioParameterFloat* cutoff_frequency_in_hz_;
private:
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,
                             dsp::IIR::Coefficients<float>> low_pass_filter_0;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,
                             dsp::IIR::Coefficients<float>> low_pass_filter_1;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,
                             dsp::IIR::Coefficients<float>> high_pass_filter_0;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,
                             dsp::IIR::Coefficients<float>> high_pass_filter_1;

    float sample_rate_in_hz_;

    void updateCoefficients();
};