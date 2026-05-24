// PluginProcessor.cpp

#include "PluginProcessor.h"
#include "PluginEditor.h"

KHighPass2AudioProcessor::KHighPass2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
       apvts(*this, nullptr, "Parameters", createParameters())
{
}

KHighPass2AudioProcessor::~KHighPass2AudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout KHighPass2AudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "INPUT", "Input",
        juce::NormalisableRange<float>(0.0f, 4.0f, 0.01f),
        1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "OUTPUT", "Output",
        juce::NormalisableRange<float>(0.0f, 4.0f, 0.01f),
        1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CUTOFF", "Cutoff",
        juce::NormalisableRange<float>(0.001f, 0.3f, 0.0001f),
        0.035f));

    return { params.begin(), params.end() };
}

const juce::String KHighPass2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KHighPass2AudioProcessor::acceptsMidi() const { return false; }
bool KHighPass2AudioProcessor::producesMidi() const { return false; }
bool KHighPass2AudioProcessor::isMidiEffect() const { return false; }
double KHighPass2AudioProcessor::getTailLengthSeconds() const { return 0.0; }

int KHighPass2AudioProcessor::getNumPrograms() { return 1; }
int KHighPass2AudioProcessor::getCurrentProgram() { return 0; }
void KHighPass2AudioProcessor::setCurrentProgram (int index) {}
const juce::String KHighPass2AudioProcessor::getProgramName (int index) { return {}; }
void KHighPass2AudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void KHighPass2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    s1L = s2L = 0.0f;
    s1R = s2R = 0.0f;
}

void KHighPass2AudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KHighPass2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}
#endif

void KHighPass2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float input  = *apvts.getRawParameterValue("INPUT");
    float output = *apvts.getRawParameterValue("OUTPUT");
    float cutoff = *apvts.getRawParameterValue("CUTOFF");

    float g = cutoff * cutoff;

    auto* left  = buffer.getWritePointer(0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float l = left[i] * input;
        float r = right ? right[i] * input : l;

        // Stage 1
        s1L += g * (l - s1L);
        s1R += g * (r - s1R);

        // Stage 2
        s2L += g * (s1L - s2L);
        s2R += g * (s1R - s2R);

        // Hann/binomial-style smooth mass
        float lpL = 0.5f * s1L + 0.5f * s2L;
        float lpR = 0.5f * s1R + 0.5f * s2R;

        // complementary HP
        float hpL = l - lpL;
        float hpR = r - lpR;

        left[i] = hpL * output;

        if (right)
            right[i] = hpR * output;
    }
}

bool KHighPass2AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* KHighPass2AudioProcessor::createEditor()
{
    return new KHighPass2AudioProcessorEditor (*this);
}

void KHighPass2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void KHighPass2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr)
    {
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KHighPass2AudioProcessor();
}