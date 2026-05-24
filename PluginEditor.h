// PluginEditor.h

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class KHighPass2AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    KHighPass2AudioProcessorEditor (KHighPass2AudioProcessor&);
    ~KHighPass2AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    KHighPass2AudioProcessor& audioProcessor;

    juce::Slider inputSlider;
    juce::Slider outputSlider;
    juce::Slider cutoffSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<Attachment> inputAttach;
    std::unique_ptr<Attachment> outputAttach;
    std::unique_ptr<Attachment> cutoffAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KHighPass2AudioProcessorEditor)
};