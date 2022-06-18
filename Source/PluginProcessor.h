/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#define current_Version_name "Fuzzv6"
#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class FuzzPedalAudioProcessor  : public juce::AudioProcessor
{
public:
    float gainRef;
    float blend;
    float volume;
    float tone = 1000.0;
    enum {
        filterIndex
    };
    
    //==============================================================================
    FuzzPedalAudioProcessor();
    ~FuzzPedalAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void updateFilter();
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    

private:
    
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>,juce::dsp::IIR::Coefficients <float>> lowPassFilter;
    float lastSampleRate;
    juce::Random rng;
    bool clipped = false;
    juce::Time time;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzPedalAudioProcessor)
};
