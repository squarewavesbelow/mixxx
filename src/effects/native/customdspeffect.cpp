#include "effects/native/customdspeffect.h"

#include "util/sample.h"

// static
QString CustomDspEffect::getId() {
    return "org.mixxx.effects.customdsp";
}

// static
EffectManifest CustomDspEffect::getManifest() {
    EffectManifest manifest;
    manifest.setId(getId());
    manifest.setName(QObject::tr("CustomDsp"));
    manifest.setAuthor("sqrwvzblw");
    manifest.setVersion("0.1");
    manifest.setDescription(QObject::tr(
        "The CustomDsp is an effect that can be used as an example "
    	"of how to implement custom dsp effects. A simple level/gain "
        "effect with mute button is implemented."));
    manifest.setEffectRampsFromDry(true);

    EffectManifestParameter* paramA = manifest.addParameter();
    paramA->setId("knob_A");
    paramA->setName(QObject::tr("Gain"));
    paramA->setDescription(QObject::tr("Adjusts Gain."));
    paramA->setControlHint(EffectManifestParameter::ControlHint::KNOB_LOGARITHMIC);
    paramA->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    paramA->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    paramA->setDefaultLinkType(EffectManifestParameter::LinkType::LINKED);
    paramA->setDefaultLinkInversion(EffectManifestParameter::LinkInversion::INVERTED);
    paramA->setNeutralPointOnScale(1.0);
    paramA->setDefault(1.0);
    paramA->setMinimum(0.00);
    paramA->setMaximum(2.0);

    EffectManifestParameter* switchA = manifest.addParameter();
    switchA->setId("switch_A");
    switchA->setName(QObject::tr("Mute"));
    switchA->setDescription(QObject::tr("Mute audio passing through this effect block"));
    switchA->setControlHint(EffectManifestParameter::ControlHint::TOGGLE_STEPPING);
    switchA->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    switchA->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    switchA->setDefault(1);
    switchA->setMinimum(0);
    switchA->setMaximum(1);




    return manifest;
}

CustomDspEffect::CustomDspEffect(EngineEffect* pEffect,
                                   const EffectManifest& manifest)
        : m_pKnobA(pEffect->getParameterById("knob_A")),
		  m_pSwitchA(pEffect->getParameterById("switch_A")) {
    Q_UNUSED(manifest);
}

CustomDspEffect::~CustomDspEffect() {
    //qDebug() << debugString() << "destroyed";
}

void CustomDspEffect::processChannel(const ChannelHandle& handle,
                                      CustomDspGroupState* pState,
                                      const CSAMPLE* pInput, CSAMPLE* pOutput,
                                      const unsigned int numSamples,
                                      const unsigned int sampleRate,
                                      const EffectProcessor::EnableState enableState,
                                      const GroupFeatureState& groupFeatures) {
    Q_UNUSED(handle);
    Q_UNUSED(groupFeatures);
    Q_UNUSED(sampleRate); // we are normalized to 1
    Q_UNUSED(enableState); // no need to ramp, it is just a customdsp ;-)



    CSAMPLE effect_gain = m_pKnobA ?
    		m_pKnobA->value() : 0.0;


    const int kChannels = 2;
    for (unsigned int i = 0; i < numSamples; i += kChannels) {

        // mute switch check
        if (!m_pSwitchA->toBool()) {
        	effect_gain = 0.0;
        }

        pState->hold_l = pInput[i] * effect_gain;
        pState->hold_r = pInput[i+1] * effect_gain;

        pOutput[i] = pState->hold_l;
        pOutput[i+1] = pState->hold_r;
    }
}
