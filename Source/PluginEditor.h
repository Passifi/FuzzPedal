/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FuzzPedalAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    FuzzPedalAudioProcessorEditor (FuzzPedalAudioProcessor&);
    ~FuzzPedalAudioProcessorEditor() override;

    //==============================================================================
    void sliderValueChanged(juce::Slider*slider) override;
    void paint (juce::Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FuzzPedalAudioProcessor& audioProcessor;

    juce::Slider gain;
    juce::Label gainLabel;
    juce::Slider blend;
    juce::Label blendLabel;
    juce::Slider volume;
    juce::Label volumeLabel;
    juce::Label toneLabel;
    juce::Slider tone;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzPedalAudioProcessorEditor)
};
