/*
    ==============================================================================

      CompressorProcessor.h
      Created: 2 Mar 2020 3:45:35pm
      Author:  Ben Hayes

    ==============================================================================
*/

#pragma once

#include "ProcessorBase.h"

class CompressorProcessor : public ProcessorBase
{
public:
    CompressorProcessor(int index);
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock (
        AudioSampleBuffer& buffer,
        MidiBuffer& midiMessages) override;
    const String getName() const override { return "Compressor"; }
    std::unique_ptr<AudioProcessorParameterGroup> getParameterTree();

    AudioParameterFloat* attack_in_seconds_;
    AudioParameterFloat* release_in_seconds_;
    AudioParameterFloat* makeup_gain_in_db_;
    AudioParameterFloat* knee_in_db_;
    AudioParameterFloat* threshold_in_db_;
    AudioParameterFloat* ratio_;

private:
    float sample_rate_in_hz_;
    float last_gain_in_db_ = 0.0f;
    float alpha_a_;
    float alpha_r_;

    void calculateTimingCoefficients();
    float detectLevelOverThreshold(
        AudioSampleBuffer& buffer,
        int sample_index);
    float computeGain(float sample_over_threshold);
    float applyTimeSmoothing(float gain_in_db);
    void applyGain(
        float gain_in_db,
        AudioSampleBuffer& buffer,
        int sample_index);
};
