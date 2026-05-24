// PluginEditor.cpp

#include "PluginProcessor.h"
#include "PluginEditor.h"

KHighPass2AudioProcessorEditor::KHighPass2AudioProcessorEditor (KHighPass2AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (420, 180);

    auto setupSlider = [&](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(s);
    };

    setupSlider(inputSlider);
    setupSlider(outputSlider);
    setupSlider(cutoffSlider);

    inputAttach  = std::make_unique<Attachment>(audioProcessor.apvts, "INPUT", inputSlider);
    outputAttach = std::make_unique<Attachment>(audioProcessor.apvts, "OUTPUT", outputSlider);
    cutoffAttach = std::make_unique<Attachment>(audioProcessor.apvts, "CUTOFF", cutoffSlider);
}

KHighPass2AudioProcessorEditor::~KHighPass2AudioProcessorEditor()
{
}

void KHighPass2AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("KHighPass2", 0, 10, getWidth(), 30, juce::Justification::centred, 1);

    g.setFont(16.0f);

    g.drawFittedText("Input", 40, 130, 80, 20, juce::Justification::centred, 1);
    g.drawFittedText("Cutoff", 170, 130, 80, 20, juce::Justification::centred, 1);
    g.drawFittedText("Output", 300, 130, 80, 20, juce::Justification::centred, 1);
}

void KHighPass2AudioProcessorEditor::resized()
{
    inputSlider .setBounds(30, 45, 90, 90);
    cutoffSlider.setBounds(160, 45, 90, 90);
    outputSlider.setBounds(290, 45, 90, 90);
}