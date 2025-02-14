//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		CGMSoftRigTransformSoftware.h
/// @brief		GMEngine - Soft RigTransformSoftware
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.12
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMSoftBone.h"
#include "GMSoftRigTransform.h"

namespace GM
{
    class CGMSoftRigGeometry;

    /// This class manage format for software skinning
    class CGMSoftRigTransformSoftware : public CGMSoftRigTransform
    {
    public:
        CGMSoftRigTransformSoftware();
        CGMSoftRigTransformSoftware(const CGMSoftRigTransformSoftware& rts,const osg::CopyOp& copyop);

        virtual void operator()(CGMSoftRigGeometry&);
        //to call when a skeleton is reacheable from the rig to prepare technic data
        virtual bool prepareData(CGMSoftRigGeometry&);

        typedef std::pair<unsigned int, float> LocalBoneIDWeight;
        class SoftBonePtrWeight : LocalBoneIDWeight
        {
        public:
            SoftBonePtrWeight(unsigned int id, float weight, CGMSoftBone* bone = 0) : LocalBoneIDWeight(id, weight), _boneptr(bone) {}
            SoftBonePtrWeight(const SoftBonePtrWeight& bw2) : LocalBoneIDWeight(bw2.getBoneID(), bw2.getWeight()), _boneptr(bw2._boneptr.get()) {}
            inline const float& getWeight() const { return second; }
            inline void setWeight(float b) { second = b; }
            inline const unsigned int& getBoneID() const { return first; }
            inline void setBoneID(unsigned int b) { first = b; }
            inline bool operator< (const SoftBonePtrWeight& b1) const {
                if (second > b1.second) return true;
                if (second < b1.second) return false;
                return (first > b1.first);
            }
            ///set Bone pointer
            inline const CGMSoftBone* getBonePtr() const { return _boneptr.get(); }
            inline void setBonePtr(CGMSoftBone* b) { _boneptr = b; }
        protected:
            osg::observer_ptr< CGMSoftBone > _boneptr;
        };

        typedef std::vector<SoftBonePtrWeight> SoftBonePtrWeightList;

        /// map a set of boneinfluence to a list of vertex indices sharing this set
        class SoftVertexGroup
        {
        public:
            inline SoftBonePtrWeightList& getBoneWeights() { return _boneweights; }

            inline osgAnimation::IndexList& getVertices() { return _vertexes; }

            inline void resetMatrix()
            {
                _result.set(0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 1);
            }
            inline void accummulateMatrix(const osg::Matrix& invBindMatrix, const osg::Matrix& matrix, osg::Matrix::value_type weight)
            {
                osg::Matrix m = invBindMatrix * matrix;
                osg::Matrix::value_type* ptr = m.ptr();
                osg::Matrix::value_type* ptrresult = _result.ptr();
                ptrresult[0] += ptr[0] * weight;
                ptrresult[1] += ptr[1] * weight;
                ptrresult[2] += ptr[2] * weight;

                ptrresult[4] += ptr[4] * weight;
                ptrresult[5] += ptr[5] * weight;
                ptrresult[6] += ptr[6] * weight;

                ptrresult[8] += ptr[8] * weight;
                ptrresult[9] += ptr[9] * weight;
                ptrresult[10] += ptr[10] * weight;

                ptrresult[12] += ptr[12] * weight;
                ptrresult[13] += ptr[13] * weight;
                ptrresult[14] += ptr[14] * weight;
            }
            inline void computeMatrixForVertexSet()
            {
                if (_boneweights.empty())
                {
                    osg::notify(osg::WARN) << this << " CGMSoftRigTransformSoftware::VertexGroup no bones found" << std::endl;
                    _result = osg::Matrix::identity();
                    return;
                }
                resetMatrix();

                for(SoftBonePtrWeightList::iterator bwit=_boneweights.begin(); bwit!=_boneweights.end(); ++bwit )
                {
                    const CGMSoftBone* bone = bwit->getBonePtr();
                    if (!bone)
                    {
                        osg::notify(osg::WARN) << this << " RigTransformSoftware::computeMatrixForVertexSet Warning a bone is null, skip it" << std::endl;
                        continue;
                    }
                    const osg::Matrix& invBindMatrix = bone->getInvBindMatrixInSkeletonSpace();
                    const osg::Matrix& matrix = bone->getMatrixInSkeletonSpace();
                    osg::Matrix::value_type w = bwit->getWeight();
                    accummulateMatrix(invBindMatrix, matrix, w);
                }
            }
            void normalize();
            inline const osg::Matrix& getMatrix() const { return _result; }
        protected:
            SoftBonePtrWeightList _boneweights;
            osgAnimation::IndexList _vertexes;
            osg::Matrix _result;
        };

        template <class V>
        inline void compute(const osg::Matrix& transform, const osg::Matrix& invTransform, const V* src, V* dst)
        {
            // the result of matrix mult should be cached to be used for vertices transform and normal transform and maybe other computation
            for(SoftVertexGroupList::iterator itvg=_uniqVertexGroupList.begin(); itvg!=_uniqVertexGroupList.end(); ++itvg)
            {
                SoftVertexGroup& uniq = *itvg;
                uniq.computeMatrixForVertexSet();
                osg::Matrix matrix =  transform * uniq.getMatrix() * invTransform;

                const osgAnimation::IndexList& vertices = uniq.getVertices();
                for(osgAnimation::IndexList::const_iterator vertIDit=vertices.begin(); vertIDit!=vertices.end(); ++vertIDit)
                {
                    dst[*vertIDit] = src[*vertIDit] * matrix;
                }

            }
        }

        template <class V>
        inline void computeNormal(const osg::Matrix& transform, const osg::Matrix& invTransform, const V* src, V* dst)
        {
            for(SoftVertexGroupList::iterator itvg=_uniqVertexGroupList.begin(); itvg!=_uniqVertexGroupList.end(); ++itvg)
            {
                SoftVertexGroup& uniq = *itvg;
                uniq.computeMatrixForVertexSet();
                osg::Matrix matrix =  transform * uniq.getMatrix() * invTransform;

                const osgAnimation::IndexList& vertices = uniq.getVertices();
                for(osgAnimation::IndexList::const_iterator vertIDit=vertices.begin(); vertIDit!=vertices.end(); ++vertIDit)
                {
                    dst[*vertIDit] = osg::Matrix::transform3x3(src[*vertIDit],matrix);
                }
            }
        }

    protected:

        bool _needInit;

        virtual bool init(CGMSoftRigGeometry&);

        std::map<std::string, bool> _invalidInfluence;

        typedef std::vector<SoftVertexGroup> SoftVertexGroupList;
        SoftVertexGroupList _uniqVertexGroupList;

        void buildMinimumUpdateSet(const CGMSoftRigGeometry&rig );

    };
} // namespace GM
