/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FuzzPedalAudioProcessorEditor::FuzzPedalAudioProcessorEditor (FuzzPedalAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);
    gain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    gain.setRange(0.0,100.0,1.0);
    gain.setTextBoxStyle(juce::Slider::NoTextBox,false,90,0);
    gain.setPopupDisplayEnabled(true,false,this);
    gain.setTextValueSuffix(" Gain");
    gain.setValue(30);
    gain.setColour(1,juce::Colours::red );
    gain.addListener(this);
    addAndMakeVisible(&gain);

    blend.setSliderStyle(juce::Slider::Rotary);
    blend.setRange(0.0, 1.0, 0.05);
    blend.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    blend.setPopupDisplayEnabled(true, false, this);
    blend.setTextValueSuffix(" Blend");
    blend.setValue(0.6);
    blend.addListener(this);
    addAndMakeVisible(&blend);

    volume.setSliderStyle(juce::Slider::Rotary);
    volume.setRange(0.0, 1.0, 0.05);
    volume.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    volume.setPopupDisplayEnabled(true, false, this);
    volume.setTextValueSuffix(" Volume");
    volume.setValue(0.8);
    volume.addListener(this);
    addAndMakeVisible(&volume);

    tone.setSliderStyle(juce::Slider::Rotary);
    tone.setRange(0.0, 1.0, 0.05);
    tone.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    tone.setPopupDisplayEnabled(true, false, this);
    tone.setTextValueSuffix(" Tone");
    tone.setValue(0.6);
    tone.addListener(this);
    addAndMakeVisible(&tone);

    addAndMakeVisible(&gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gain,false);

    addAndMakeVisible(&volumeLabel);
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volume, false);

    addAndMakeVisible(&toneLabel);
    toneLabel.setText("Tone", juce::dontSendNotification);
    toneLabel.attachToComponent(&tone, false);

    addAndMakeVisible(&blendLabel);
    blendLabel.setText("Blend", juce::dontSendNotification);
    blendLabel.attachToComponent(&blend, false);
    
}

FuzzPedalAudioProcessorEditor::~FuzzPedalAudioProcessorEditor()
{
}

void FuzzPedalAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.gainRef = gain.getValue();
    audioProcessor.blend = blend.getValue();
    audioProcessor.volume = volume.getValue();
}

//==============================================================================
void FuzzPedalAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));


}

void FuzzPedalAudioProcessorEditor::resized()
{
    gain.setBounds(40,30,80,80);
    blend.setBounds(120, 30,80,80);
    volume.setBounds(200, 30, 80, 80);
    tone.setBounds(280, 30, 80, 80);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
