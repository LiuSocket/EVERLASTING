/* 
 * COPYRIGHT NOTICE
 * Copyright (c) 2020~2030, LiuTao
 * All rights reserved.

 * @file		CGMTangentSpaceGenerator.h
 * @brief		to solve the "tang bug" when UV of two vertices are the same
 * @version	1.0
 * @author		LiuTao
 * @date		2023.03.28
 */
#pragma once

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Array>
#include <osg/Geometry>

namespace GM
{

	/**
	 The CGMTangentSpaceGenerator class generates three arrays containing tangent-space basis vectors.
	 It takes a texture-mapped Geometry object as input, traverses its primitive sets and computes
	 Tangent, Normal and Binormal vectors for each vertex, storing them into arrays.
	 The resulting arrays can be used as vertex program varying (per-vertex) parameters,
	 enabling advanced effects like bump-mapping.
	 To use this class, simply call the generate() method specifying the Geometry object
	 you want to process and the texture unit that contains UV mapping for the normal map;
	 then you can retrieve the TBN arrays by calling getTangentArray(), getNormalArray()
	 and getBinormalArray() methods.
	 */
	class CGMTangentSpaceGenerator : public osg::Referenced {
	public:
		CGMTangentSpaceGenerator();
		CGMTangentSpaceGenerator(const CGMTangentSpaceGenerator &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

		void generate(osg::Geometry *geo, int normal_map_tex_unit = 0);

		inline osg::Vec4Array *getTangentArray() { return T_.get(); }
		inline const osg::Vec4Array *getTangentArray() const { return T_.get(); }
		inline void setTangentArray(osg::Vec4Array *array) { T_ = array; }

		inline osg::Vec4Array *getNormalArray() { return N_.get(); }
		inline const osg::Vec4Array *getNormalArray() const { return N_.get(); }
		inline void setNormalArray(osg::Vec4Array *array) { N_ = array; }

		inline osg::Vec4Array *getBinormalArray() { return B_.get(); }
		inline const osg::Vec4Array *getBinormalArray() const { return B_.get(); }
		inline void setBinormalArray(osg::Vec4Array *array) { B_ = array; }

		inline osg::IndexArray *getIndices() { return indices_.get(); }

	protected:

		virtual ~CGMTangentSpaceGenerator() {}
		CGMTangentSpaceGenerator &operator=(const CGMTangentSpaceGenerator &) { return *this; }

		void compute(osg::PrimitiveSet *pset,
			const osg::Array *vx,
			const osg::Array *nx,
			const osg::Array *tx,
			int iA, int iB, int iC);

		osg::ref_ptr<osg::Vec4Array> T_;
		osg::ref_ptr<osg::Vec4Array> B_;
		osg::ref_ptr<osg::Vec4Array> N_;
		osg::ref_ptr<osg::UIntArray> indices_;
	};
}