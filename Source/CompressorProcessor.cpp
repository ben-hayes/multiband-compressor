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
    // Make an index string so our parameters are properly numbered in the
    // host:
    auto index_str = std::to_string(index + 1);

    // Set up all our compressor's parameters:
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
        0.0f,
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
    // Store our sample rate as a member variable to save lookup cost:
    sample_rate_in_hz_ = sampleRate;
}

void CompressorProcessor::processBlock(
    AudioSampleBuffer& buffer,
    MidiBuffer& midiMessages)
{
    // Function names should make this pretty self explanatory. See individual
    // functions for more implementation detail.
    // We iterate over each sample in the incoming buffer.

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
    // Return a unique pointer to a tree containing references to all our
    // parameters and let the PluginProcessor take care of them. Makes our class
    // less portable but prevents memory leaks.
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
    // Timing coefficients are calculated based on the time constant describing
    // the time taken to reach 1 - 1 / e of the target value.
    alpha_a_ =
        exp(-1.0f / (attack_in_seconds_->get() * 0.001f * sample_rate_in_hz_));
    alpha_r_ =
        exp(-1.0f / (release_in_seconds_->get() * 0.001f * sample_rate_in_hz_));
}

float CompressorProcessor::detectLevelOverThreshold(
    AudioSampleBuffer& buffer,
    int sample_index)
{
    // This is our gain detector, so let's take the mean of our channels to get
    // only one control signal:
    auto summed_sample = 0.0f;
    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        summed_sample += buffer.getSample(channel, sample_index);
    }
    summed_sample /= buffer.getNumChannels();

    // Take the absolute value of the sample (peak detection) and convert to dB:
    auto sample_height = fabs(summed_sample);
    auto sample_height_in_db =
        Decibels::gainToDecibels(sample_height);

    // return the difference between this value and our threshold
    return sample_height_in_db - threshold_in_db_->get();
}

float CompressorProcessor::computeGain(float sample_over_threshold)
{
    auto gain_in_db = 0.0f;
    // Just to save us repeating this multiply:
    auto half_knee = knee_in_db_->get() * 0.5f;

    if (sample_over_threshold >= half_knee)
    {
        // If we're above both the threshold and the upper end of the knee,
        // then the gain is straightforward:
        gain_in_db = sample_over_threshold * (1.0f / ratio_->get() - 1.0f);
    } else if (sample_over_threshold >= -half_knee)
    {
        // If we're within the knee region, the gain is given by this
        // interpolation formula:
        gain_in_db = 1.0f / ratio_->get() - 1.0f;
        gain_in_db *= powf(sample_over_threshold + half_knee, 2);
        gain_in_db *= 1.0f / (2 * knee_in_db_->get());
    }

    return gain_in_db;
}

float CompressorProcessor::applyTimeSmoothing(float gain_in_db)
{
    // Time smoothing is applied essentially as a first order IIR filter.
    // We treat attack and release separately, so we need to check whether the
    // gain is going up or down:
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
    // Calculate linear gain from dB ratio:
    auto gain_factor =
        powf(10.0f, (makeup_gain_in_db_->get() + gain_in_db) / 20.0f); 

    // And apply the gain to each channel:
    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto sample = buffer.getSample(channel, sample_index);
        buffer.setSample(channel, sample_index, sample * gain_factor);
    }
}