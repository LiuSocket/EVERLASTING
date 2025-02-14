#include <osg/CameraView>
#include <osg/Notify>

#include <osgDB/ReadFile>

#if defined(_MSC_VER)
#pragma warning( disable : 4505 )
#pragma warning( default : 4996 )
#endif
#include <fbxsdk.h>

#include "gmmReader.h"

osgDB::ReaderWriter::ReadResult OsgFbxReader::readFbxCamera(FbxNode* pNode)
{
    const FbxCamera* gmmCamera = FbxCast<FbxCamera>(pNode->GetNodeAttribute());

    if (!gmmCamera)
    {
        return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
    }

    osg::CameraView* osgCameraView = new osg::CameraView;

    if (gmmCamera->FieldOfView.IsValid())
    {
        osgCameraView->setFieldOfView(gmmCamera->FieldOfView.Get());
    }

    if (gmmCamera->FocalLength.IsValid())
    {
        osgCameraView->setFocalLength(gmmCamera->FocalLength.Get());
    }

    if (gmmCamera->ApertureMode.IsValid())
    {
        switch (gmmCamera->ApertureMode.Get())
        {
        case FbxCamera::eHorizontal:
            osgCameraView->setFieldOfViewMode(osg::CameraView::HORIZONTAL);
            break;
        case FbxCamera::eVertical:
            osgCameraView->setFieldOfViewMode(osg::CameraView::VERTICAL);
            break;
        case FbxCamera::eHorizAndVert:
        case FbxCamera::eFocalLength:
        default:
            OSG_WARN << "readFbxCamera: Unsupported Camera aperture mode." << std::endl;
            break;
        }
    }

    return osgDB::ReaderWriter::ReadResult(osgCameraView);
}
