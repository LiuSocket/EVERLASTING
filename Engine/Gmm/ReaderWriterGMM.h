#ifndef READERWRITERGMM_H
#define READERWRITERGMM_H

#include <osgDB/ReaderWriter>
#include <fbxsdk/fbxsdk_def.h>


///////////////////////////////////////////////////////////////////////////
// OSG reader plugin for the ".gmm" format.
// See http://www.autodesk.com/fbx
// This plugin requires the FBX SDK version 2013.3 or 2014.1 or later

#if FBXSDK_VERSION_MAJOR < 2013 || (FBXSDK_VERSION_MAJOR == 2013 && FBXSDK_VERSION_MINOR < 3)
#error Wrong FBX SDK version
#endif

class ReaderWriterGMM : public osgDB::ReaderWriter
{
public:
    ReaderWriterGMM()
    {
        supportsExtension("gmm", "GMM format");
        supportsOption("Embedded", "(Write option) Embed textures in GMM file");
        supportsOption("UseFbxRoot", "(Read/write option) If the source OSG root node is a simple group with no stateset, the writer will put its children directly under the GMM root, and vice-versa for reading");
        supportsOption("LightmapTextures", "(Read option) Interpret texture maps as overriding the lighting. 3D Studio Max may export files that should be interpreted in this way.");
        supportsOption("TessellatePolygons", "(Read option) Tessellate mesh polygons. If the model contains concave polygons this may be necessary, however tessellating can be very slow and may erroneously produce triangle shards.");
    }

    const char* className() const { return "GMM reader/writer"; }

    virtual ReadResult readObject(const std::string& filename, const Options* options) const
    {
        return readNode(filename, options);
    }

    virtual WriteResult writeObject(const osg::Node& node, const std::string& filename, const Options* options) const
    {
        return writeNode(node, filename, options);
    }

    virtual ReadResult readNode(const std::string& filename, const Options*) const;
    virtual WriteResult writeNode(const osg::Node&, const std::string& filename, const Options*) const;
};

///////////////////////////////////////////////////////////////////////////

#endif
