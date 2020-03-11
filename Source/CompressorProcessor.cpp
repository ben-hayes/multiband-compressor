/*
    ==============================================================================

      CompressorProcessor.cpp
      Created: 2 Mar 2020 3:45:35pm
      Author:  Ben Hayes

    ==============================================================================
*/

#include "CompressorProcessor.h"

CompressorProcessor::CompressorProcessor(int index)
{
    auto index_str = std::to_string(index + 1);
    attack_in_seconds_ = new AudioParameterFloat(
        "band_" + index_str + "_attack",
        "Band " + index_str + " Attack",
        1.0f,
        150.0f,
        10.0f);
    release_in_seconds_ = new AudioParameterFloat(
        "band_" + index_str + "_relase",
        "Band " + index_str + " Release",
        10.0f,
        1000.0f,
        100.0f);
    makeup_gain_in_db_ = new AudioParameterFloat(
        "band_" + index_str + "_makeup_gain",
        "Band " + index_str + " Makeup Gain",
        -32.0f,
        32.0f,
        0.0f);
    knee_in_db_ = new AudioParameterFloat(
        "band_" + index_str + "_knee",
        "Band " + index_str + " Knee",
        0.0f,
        18.0f,
        0.0f);
    threshold_in_db_ = new AudioParameterFloat(
        "band_" + index_str + "_threshold",
        "Band " + index_str + " Threshold",
        -66.6f,
        0.0f,
        -6.0f);
    ratio_ = new AudioParameterFloat(
        "band_" + index_str + "_ratio",
        "Band " + index_str + " Ratio",
        1.0f,
        20.0f,
        2.0f);
}

void CompressorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sample_rate_in_hz_ = sampleRate;
}

void CompressorProcessor::processBlock(
    AudioSampleBuffer& buffer,
    MidiBuffer& midiMessages)
{
    calculateTimingCoefficients();
    for (auto n = 0; n < buffer.getNumSamples(); n++)
    {
        auto sample_over_threshold = detectLevelOverThreshold(buffer, n);
        auto gain_in_db = computeGain(sample_over_threshold);
        auto smoothed_gain_in_db = applyTimeSmoothing(gain_in_db);
        applyGain(smoothed_gain_in_db, buffer, n);
    }
}

std::unique_ptr<AudioProcessorParameterGroup> 
    CompressorProcessor::getParameterTree()
{
    auto parameter_tree = std::make_unique<AudioProcessorParameterGroup>();
    parameter_tree->addChild(
        std::unique_ptr<AudioParameterFloat>(attack_in_seconds_));
    parameter_tree->addChild(
        std::unique_ptr<AudioParameterFloat>(release_in_seconds_));
    parameter_tree->addChild(
        std::unique_ptr<AudioParameterFloat>(makeup_gain_in_db_));
    parameter_tree->addChild(
        std::unique_ptr<AudioParameterFloat>(knee_in_db_));
    parameter_tree->addChild(
        std::unique_ptr<AudioParameterFloat>(threshold_in_db_));
    parameter_tree->addChild(
        std::unique_ptr<AudioParameterFloat>(ratio_));

    return std::move(parameter_tree);
}

void CompressorProcessor::calculateTimingCoefficients()
{
    alpha_a_ =
        exp(-1.0f / (attack_in_seconds_->get() * 0.001f * sample_rate_in_hz_));
    alpha_r_ =
        exp(-1.0f / (release_in_seconds_->get() * 0.001f * sample_rate_in_hz_));
}

float CompressorProcessor::detectLevelOverThreshold(
    AudioSampleBuffer& buffer,
    int sample_index)
{
    auto summed_sample = 0.0f;
    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        summed_sample += buffer.getSample(channel, sample_index);
    }
    summed_sample /= buffer.getNumChannels();
    auto sample_height = fabs(summed_sample);
    auto sample_height_in_db =
        Decibels::gainToDecibels(sample_height);
    
    return sample_height_in_db - threshold_in_db_->get();
}

float CompressorProcessor::computeGain(float sample_over_threshold)
{
    auto gain_in_db = 0.0f;
    auto half_knee = knee_in_db_->get() * 0.5f;
    if (sample_over_threshold >= half_knee)
    {
        gain_in_db = sample_over_threshold * (1.0f / ratio_->get() - 1.0f);
    } else if (sample_over_threshold >= -half_knee)
    {
        gain_in_db = 1.0f / ratio_->get() - 1.0f;
        gain_in_db *= powf(sample_over_threshold + half_knee, 2);
        gain_in_db *= 1.0f / (2 * knee_in_db_->get());
    }

    return gain_in_db;
}

float CompressorProcessor::applyTimeSmoothing(float gain_in_db)
{
    auto smoothed_gain_in_db = 0.0f;
    if (gain_in_db < last_gain_in_db_)
    {
        smoothed_gain_in_db =
            alpha_a_ * last_gain_in_db_ + (1.0f - alpha_a_) * gain_in_db;
    } else
    {
        smoothed_gain_in_db =
            alpha_r_ * last_gain_in_db_ + (1.0f - alpha_r_) * gain_in_db;
    }
    last_gain_in_db_ = smoothed_gain_in_db;
    return smoothed_gain_in_db;
}

void CompressorProcessor::applyGain(
    float gain_in_db,
    AudioSampleBuffer& buffer,
    int sample_index)
{
    auto gain_factor =
        powf(10.0f, (makeup_gain_in_db_->get() + gain_in_db) / 20.0f); 

    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto sample = buffer.getSample(channel, sample_index);
        buffer.setSample(channel, sample_index, sample * gain_factor);
    }
}