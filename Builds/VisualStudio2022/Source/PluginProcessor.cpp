/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FuzzPedalAudioProcessor::FuzzPedalAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FuzzPedalAudioProcessor::~FuzzPedalAudioProcessor()
{
}

//==============================================================================
const juce::String FuzzPedalAudioProcessor::getName() const
{
    return current_Version_name;
}

bool FuzzPedalAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FuzzPedalAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FuzzPedalAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FuzzPedalAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FuzzPedalAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FuzzPedalAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FuzzPedalAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FuzzPedalAudioProcessor::getProgramName (int index)
{
    return {};
}

void FuzzPedalAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FuzzPedalAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FuzzPedalAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FuzzPedalAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FuzzPedalAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    float offPos = gainRef;
    float offNeg = gainRef*-1;
    float clipOff = 0.85;
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    /*juce::String message;
    message << "Preparing to play audio...\n";
    */

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        
        float gain = gainRef;
        auto* channelIn = buffer.getReadPointer(channel);
        auto* channelOut = buffer.getWritePointer (channel);
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            float overdrive = atan(channelIn[sample] * gainRef) * (2.0 / 3.1424231235);
            float fuzzPart = 0;
            if (clipped)
                gain *= 0.7; // make this one a slider
            else
                gain = gainRef;
            
            if (abs(channelIn[sample] * gain) < clipOff)
            {
                fuzzPart = channelIn[sample] * gain;
                clipped = false;
                
            }
            else
            {
                clipped = true;
                
               /* message << " In Sample = " << channelIn[sample] << "\n";
                message << " Gained = " << channelIn[sample] * gain << "\n";
                juce::Logger::getCurrentLogger()->writeToLog(message);*/
                if(channelIn[sample] > 0)
                    fuzzPart = (clipOff /*+ (rng.nextFloat()*0.1 -0.05)*/);
                else 
                    fuzzPart = (clipOff /*+ (rng.nextFloat() * 0.1 - 0.05)*/)*-1;
            }

            channelOut[sample] = ((fuzzPart*blend + overdrive*(1-blend))/2)*volume;

        }

        // ..do something to the data...
    }
}

//==============================================================================
bool FuzzPedalAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FuzzPedalAudioProcessor::createEditor()
{
    return new FuzzPedalAudioProcessorEditor (*this);
}

//==============================================================================
void FuzzPedalAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FuzzPedalAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FuzzPedalAudioProcessor();
}
