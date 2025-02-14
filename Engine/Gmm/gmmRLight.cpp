#include <osg/LightSource>

#include <osgDB/ReadFile>

#if defined(_MSC_VER)
#pragma warning( disable : 4505 )
#pragma warning( default : 4996 )
#endif
#include <fbxsdk.h>

#include "gmmReader.h"

osgDB::ReaderWriter::ReadResult OsgFbxReader::readFbxLight(FbxNode* pNode, int& nLightCount)
{
    const FbxLight* gmmLight = FbxCast<FbxLight>(pNode->GetNodeAttribute());

    if (!gmmLight)
    {
        return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
    }

    osg::Light* osgLight = new osg::Light;
    osgLight->setLightNum(nLightCount++);

    osg::LightSource* osgLightSource = new osg::LightSource;
    osgLightSource->setLight(osgLight);

    FbxLight::EType gmmLightType = gmmLight->LightType.IsValid() ?
        gmmLight->LightType.Get() : FbxLight::ePoint;

    osgLight->setPosition(osg::Vec4(0,0,0,gmmLightType != FbxLight::eDirectional));

    if (gmmLightType == FbxLight::eSpot)
    {
        double coneAngle = gmmLight->OuterAngle.Get();
        double hotSpot = gmmLight->InnerAngle.Get();
        const float MIN_HOTSPOT = 0.467532f;

        osgLight->setSpotCutoff(static_cast<float>(coneAngle));

        //Approximate the hotspot using the GL light exponent.
        // This formula maps a hotspot of 180 to exponent 0 (uniform light
        // distribution) and a hotspot of 45 to exponent 1 (effective light
        // intensity is attenuated by the cosine of the angle between the
        // direction of the light and the direction from the light to the vertex
        // being lighted). A hotspot close to 0 maps to exponent 128 (maximum).
        float exponent = (180.0f / (std::max)(static_cast<float>(hotSpot),
            MIN_HOTSPOT) - 1.0f) / 3.0f;
        osgLight->setSpotExponent(exponent);
    }

    if (gmmLight->DecayType.IsValid() &&
        gmmLight->DecayStart.IsValid())
    {
        double gmmDecayStart = gmmLight->DecayStart.Get();

        switch (gmmLight->DecayType.Get())
        {
        case FbxLight::eNone:
            break;
        case FbxLight::eLinear:
            osgLight->setLinearAttenuation(gmmDecayStart);
            break;
        case FbxLight::eQuadratic:
        case FbxLight::eCubic:
            osgLight->setQuadraticAttenuation(gmmDecayStart);
            break;
        }
    }

    osg::Vec3f osgDiffuseSpecular(1.0f, 1.0f, 1.0f);
    osg::Vec3f osgAmbient(0.0f, 0.0f, 0.0f);
    if (gmmLight->Color.IsValid())
    {
        FbxDouble3 gmmColor = gmmLight->Color.Get();
        osgDiffuseSpecular.set(
            static_cast<float>(gmmColor[0]),
            static_cast<float>(gmmColor[1]),
            static_cast<float>(gmmColor[2]));
    }
    if (gmmLight->Intensity.IsValid())
    {
        osgDiffuseSpecular *= static_cast<float>(gmmLight->Intensity.Get()) * 0.01f;
    }
    if (gmmLight->ShadowColor.IsValid())
    {
        FbxDouble3 gmmShadowColor = gmmLight->ShadowColor.Get();
        osgAmbient.set(
            static_cast<float>(gmmShadowColor[0]),
            static_cast<float>(gmmShadowColor[1]),
            static_cast<float>(gmmShadowColor[2]));
    }

    osgLight->setDiffuse(osg::Vec4f(osgDiffuseSpecular, 1.0f));
    osgLight->setSpecular(osg::Vec4f(osgDiffuseSpecular, 1.0f));
    osgLight->setAmbient(osg::Vec4f(osgAmbient, 1.0f));

    return osgDB::ReaderWriter::ReadResult(osgLightSource);
}
