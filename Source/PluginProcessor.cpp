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

    juce::dsp::ProcessSpec spec;
    lastSampleRate = sampleRate;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
    
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

void FuzzPedalAudioProcessor::updateFilter()
{
#ifdef DEBUG
    float freq = tone;
#else
    float freq = tone;
#endif // DEBUG

        float resonance = 0.9f;
        *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate,freq,resonance);
}


void FuzzPedalAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    

    float offPos = gainRef;
    float offNeg = gainRef*-1;
    float clipOff = 0.85;
    
    /*for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());*/

    
    


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
       

        
      
    }

    juce::dsp::AudioBlock<float> block(buffer);
    updateFilter();
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
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
