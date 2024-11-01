#include "GMTangentSpaceGenerator.h"

#include <osg/Notify>
#include <osg/io_utils>

using namespace GM;

CGMTangentSpaceGenerator::CGMTangentSpaceGenerator()
	: osg::Referenced(),
	T_(new osg::Vec4Array),
	B_(new osg::Vec4Array),
	N_(new osg::Vec4Array)
{
	T_->setBinding(osg::Array::BIND_PER_VERTEX); T_->setNormalize(false);
	B_->setBinding(osg::Array::BIND_PER_VERTEX); T_->setNormalize(false);
	N_->setBinding(osg::Array::BIND_PER_VERTEX); T_->setNormalize(false);
}

CGMTangentSpaceGenerator::CGMTangentSpaceGenerator(const CGMTangentSpaceGenerator &copy, const osg::CopyOp &copyop)
	: osg::Referenced(copy),
	T_(static_cast<osg::Vec4Array *>(copyop(copy.T_.get()))),
	B_(static_cast<osg::Vec4Array *>(copyop(copy.B_.get()))),
	N_(static_cast<osg::Vec4Array *>(copyop(copy.N_.get())))
{
}

void CGMTangentSpaceGenerator::generate(osg::Geometry *geo, int normal_map_tex_unit)
{
	const osg::Array *vx = geo->getVertexArray();
	const osg::Array *nx = geo->getNormalArray();
	const osg::Array *tx = geo->getTexCoordArray(normal_map_tex_unit);

	if (!vx || !tx) return;


	unsigned int vertex_count = vx->getNumElements();
	T_->assign(vertex_count, osg::Vec4());
	B_->assign(vertex_count, osg::Vec4());
	N_->assign(vertex_count, osg::Vec4());

	unsigned int i; // VC6 doesn't like for-scoped variables

	for (unsigned int pri = 0; pri < geo->getNumPrimitiveSets(); ++pri) {
		osg::PrimitiveSet *pset = geo->getPrimitiveSet(pri);

		unsigned int N = pset->getNumIndices();

		switch (pset->getMode()) {

		case osg::PrimitiveSet::TRIANGLES:
			for (i = 0; i < N; i += 3) {
				compute(pset, vx, nx, tx, i, i + 1, i + 2);
			}
			break;

		case osg::PrimitiveSet::QUADS:
			for (i = 0; i < N; i += 4) {
				compute(pset, vx, nx, tx, i, i + 1, i + 2);
				compute(pset, vx, nx, tx, i + 2, i + 3, i);
			}
			break;

		case osg::PrimitiveSet::TRIANGLE_STRIP:
			if (pset->getType() == osg::PrimitiveSet::DrawArrayLengthsPrimitiveType) {
				osg::DrawArrayLengths *dal = static_cast<osg::DrawArrayLengths *>(pset);
				unsigned int j = 0;
				for (osg::DrawArrayLengths::const_iterator pi = dal->begin(); pi != dal->end(); ++pi) {
					unsigned int iN = static_cast<unsigned int>(*pi - 2);
					for (i = 0; i < iN; ++i, ++j) {
						if ((i % 2) == 0) {
							compute(pset, vx, nx, tx, j, j + 1, j + 2);
						}
						else {
							compute(pset, vx, nx, tx, j + 1, j, j + 2);
						}
					}
					j += 2;
				}
			}
			else {
				for (i = 0; i < N - 2; ++i) {
					if ((i % 2) == 0) {
						compute(pset, vx, nx, tx, i, i + 1, i + 2);
					}
					else {
						compute(pset, vx, nx, tx, i + 1, i, i + 2);
					}
				}
			}
			break;

		case osg::PrimitiveSet::QUAD_STRIP:
			if (pset->getType() == osg::PrimitiveSet::DrawArrayLengthsPrimitiveType) {
				osg::DrawArrayLengths *dal = static_cast<osg::DrawArrayLengths *>(pset);
				unsigned int j = 0;
				for (osg::DrawArrayLengths::const_iterator pi = dal->begin(); pi != dal->end(); ++pi) {
					unsigned int iN = static_cast<unsigned int>(*pi - 2);
					for (i = 0; i < iN; ++i, ++j) {
						if ((i % 2) == 0) {
							compute(pset, vx, nx, tx, j, j + 2, j + 1);
						}
						else {
							compute(pset, vx, nx, tx, j, j + 1, j + 2);
						}
					}
					j += 2;
				}
			}
			else {
				for (i = 0; i < N - 2; ++i) {
					if ((i % 2) == 0) {
						compute(pset, vx, nx, tx, i, i + 2, i + 1);
					}
					else {
						compute(pset, vx, nx, tx, i, i + 1, i + 2);
					}
				}
			}
			break;

		case osg::PrimitiveSet::TRIANGLE_FAN:
		case osg::PrimitiveSet::POLYGON:
			if (pset->getType() == osg::PrimitiveSet::DrawArrayLengthsPrimitiveType) {
				osg::DrawArrayLengths *dal = static_cast<osg::DrawArrayLengths *>(pset);
				unsigned int j = 0;
				for (osg::DrawArrayLengths::const_iterator pi = dal->begin(); pi != dal->end(); ++pi) {
					unsigned int iN = static_cast<unsigned int>(*pi - 2);
					for (i = 0; i < iN; ++i) {
						compute(pset, vx, nx, tx, 0, j + 1, j + 2);
					}
					j += 2;
				}
			}
			else {
				for (i = 0; i < N - 2; ++i) {
					compute(pset, vx, nx, tx, 0, i + 1, i + 2);
				}
			}
			break;

		case osg::PrimitiveSet::POINTS:
		case osg::PrimitiveSet::LINES:
		case osg::PrimitiveSet::LINE_STRIP:
		case osg::PrimitiveSet::LINE_LOOP:
		case osg::PrimitiveSet::LINES_ADJACENCY:
		case osg::PrimitiveSet::LINE_STRIP_ADJACENCY:
			break;

		default: OSG_WARN << "Warning: CGMTangentSpaceGenerator: unknown primitive mode " << pset->getMode() << "\n";
		}
	}

	// normalize basis vectors and force the normal vector to match
	// the triangle normal's direction
	unsigned int attrib_count = vx->getNumElements();
	for (i = 0; i < attrib_count; ++i) {
		osg::Vec4 &vT = (*T_)[i];
		osg::Vec4 &vB = (*B_)[i];
		osg::Vec4 &vN = (*N_)[i];

		osg::Vec3 txN = osg::Vec3(vT.x(), vT.y(), vT.z()) ^ osg::Vec3(vB.x(), vB.y(), vB.z());
		bool flipped = txN * osg::Vec3(vN.x(), vN.y(), vN.z()) < 0;

		if (flipped) {
			vN = osg::Vec4(-txN, 0);
		}
		else {
			vN = osg::Vec4(txN, 0);
		}

		vT.normalize();
		vB.normalize();
		vN.normalize();

		vT[3] = flipped ? -1.0f : 1.0f;
	}
	/* TO-DO: if indexed, compress the attributes to have only one
	 * version of each (different indices for each one?) */
}

void CGMTangentSpaceGenerator::compute(osg::PrimitiveSet* pset,
	const osg::Array* vx,
	const osg::Array* nx,
	const osg::Array* tx,
	int iA, int iB, int iC)
{
	iA = pset->index(iA);
	iB = pset->index(iB);
	iC = pset->index(iC);

	osg::Vec3 P1;
	osg::Vec3 P2;
	osg::Vec3 P3;

	int i; // VC6 doesn't like for-scoped variables

	switch (vx->getType())
	{
	case osg::Array::Vec2ArrayType:
		for (i = 0; i < 2; ++i) {
			P1.ptr()[i] = static_cast<const osg::Vec2Array&>(*vx)[iA].ptr()[i];
			P2.ptr()[i] = static_cast<const osg::Vec2Array&>(*vx)[iB].ptr()[i];
			P3.ptr()[i] = static_cast<const osg::Vec2Array&>(*vx)[iC].ptr()[i];
		}
		break;

	case osg::Array::Vec3ArrayType:
		P1 = static_cast<const osg::Vec3Array&>(*vx)[iA];
		P2 = static_cast<const osg::Vec3Array&>(*vx)[iB];
		P3 = static_cast<const osg::Vec3Array&>(*vx)[iC];
		break;

	case osg::Array::Vec4ArrayType:
		for (i = 0; i < 3; ++i) {
			P1.ptr()[i] = static_cast<const osg::Vec4Array&>(*vx)[iA].ptr()[i];
			P2.ptr()[i] = static_cast<const osg::Vec4Array&>(*vx)[iB].ptr()[i];
			P3.ptr()[i] = static_cast<const osg::Vec4Array&>(*vx)[iC].ptr()[i];
		}
		break;

	default:
		OSG_WARN << "Warning: CGMTangentSpaceGenerator: vertex array must be Vec2Array, Vec3Array or Vec4Array" << std::endl;
	}

	osg::Vec3 N1;
	osg::Vec3 N2;
	osg::Vec3 N3;

	if (nx)
	{
		switch (nx->getType())
		{
		case osg::Array::Vec2ArrayType:
			for (i = 0; i < 2; ++i) {
				N1.ptr()[i] = static_cast<const osg::Vec2Array&>(*nx)[iA].ptr()[i];
				N2.ptr()[i] = static_cast<const osg::Vec2Array&>(*nx)[iB].ptr()[i];
				N3.ptr()[i] = static_cast<const osg::Vec2Array&>(*nx)[iC].ptr()[i];
			}
			break;

		case osg::Array::Vec3ArrayType:
			N1 = static_cast<const osg::Vec3Array&>(*nx)[iA];
			N2 = static_cast<const osg::Vec3Array&>(*nx)[iB];
			N3 = static_cast<const osg::Vec3Array&>(*nx)[iC];
			break;

		case osg::Array::Vec4ArrayType:
			for (i = 0; i < 3; ++i) {
				N1.ptr()[i] = static_cast<const osg::Vec4Array&>(*nx)[iA].ptr()[i];
				N2.ptr()[i] = static_cast<const osg::Vec4Array&>(*nx)[iB].ptr()[i];
				N3.ptr()[i] = static_cast<const osg::Vec4Array&>(*nx)[iC].ptr()[i];
			}
			break;

		default:
			OSG_WARN << "Warning: CGMTangentSpaceGenerator: normal array must be Vec2Array, Vec3Array or Vec4Array" << std::endl;
		}
	}

	osg::Vec2 uv1;
	osg::Vec2 uv2;
	osg::Vec2 uv3;

	switch (tx->getType())
	{
	case osg::Array::Vec2ArrayType:
		uv1 = static_cast<const osg::Vec2Array&>(*tx)[iA];
		uv2 = static_cast<const osg::Vec2Array&>(*tx)[iB];
		uv3 = static_cast<const osg::Vec2Array&>(*tx)[iC];
		break;

	case osg::Array::Vec3ArrayType:
		for (i = 0; i < 2; ++i) {
			uv1.ptr()[i] = static_cast<const osg::Vec3Array&>(*tx)[iA].ptr()[i];
			uv2.ptr()[i] = static_cast<const osg::Vec3Array&>(*tx)[iB].ptr()[i];
			uv3.ptr()[i] = static_cast<const osg::Vec3Array&>(*tx)[iC].ptr()[i];
		}
		break;

	case osg::Array::Vec4ArrayType:
		for (i = 0; i < 2; ++i) {
			uv1.ptr()[i] = static_cast<const osg::Vec4Array&>(*tx)[iA].ptr()[i];
			uv2.ptr()[i] = static_cast<const osg::Vec4Array&>(*tx)[iB].ptr()[i];
			uv3.ptr()[i] = static_cast<const osg::Vec4Array&>(*tx)[iC].ptr()[i];
		}
		break;

	default:
		OSG_WARN << "Warning: CGMTangentSpaceGenerator: texture coord array must be Vec2Array, Vec3Array or Vec4Array" << std::endl;
	}

	// 为了解决UV重合时，切线和伴法线出错的bug，必须让UV相同的顶点做一些微小的UV偏移
	if (uv1 == uv2)
	{
		uv1 -= osg::Vec2(1.1e-4f, 1.3e-4f);
		uv2 += osg::Vec2(1.7e-4f, 1.9e-4f);
	}
	if (uv1 == uv3)
	{
		uv1 -= osg::Vec2(2.3e-4f, 2.9e-4f);
		uv3 += osg::Vec2(3.1e-4f, 3.7e-4f);
	}
	if (uv2 == uv3)
	{
		uv2 += osg::Vec2(3.9e-4f, 4.1e-4f);
		uv3 += osg::Vec2(4.3e-4f, 4.7e-4f);
	}

	if (nx) {
		osg::Vec3 V, T1, T2, T3, B1, B2, B3;

		V = osg::Vec3(P2.x() - P1.x(), uv2.x() - uv1.x(), uv2.y() - uv1.y()) ^
			osg::Vec3(P3.x() - P1.x(), uv3.x() - uv1.x(), uv3.y() - uv1.y());
		if (V.x() != 0) {
			V.normalize();
			T1.x() += -V.y() / V.x();
			B1.x() += -V.z() / V.x();
			T2.x() += -V.y() / V.x();
			B2.x() += -V.z() / V.x();
			T3.x() += -V.y() / V.x();
			B3.x() += -V.z() / V.x();
		}

		V = osg::Vec3(P2.y() - P1.y(), uv2.x() - uv1.x(), uv2.y() - uv1.y()) ^
			osg::Vec3(P3.y() - P1.y(), uv3.x() - uv1.x(), uv3.y() - uv1.y());
		if (V.x() != 0) {
			V.normalize();
			T1.y() += -V.y() / V.x();
			B1.y() += -V.z() / V.x();
			T2.y() += -V.y() / V.x();
			B2.y() += -V.z() / V.x();
			T3.y() += -V.y() / V.x();
			B3.y() += -V.z() / V.x();
		}

		V = osg::Vec3(P2.z() - P1.z(), uv2.x() - uv1.x(), uv2.y() - uv1.y()) ^
			osg::Vec3(P3.z() - P1.z(), uv3.x() - uv1.x(), uv3.y() - uv1.y());
		if (V.x() != 0) {
			V.normalize();
			T1.z() += -V.y() / V.x();
			B1.z() += -V.z() / V.x();
			T2.z() += -V.y() / V.x();
			B2.z() += -V.z() / V.x();
			T3.z() += -V.y() / V.x();
			B3.z() += -V.z() / V.x();
		}

		osg::Vec3 tempvec;
		tempvec = N1 ^ T1;
		(*T_)[iA] = osg::Vec4(tempvec ^ N1, 0);
		tempvec = B1 ^ N1;
		(*B_)[iA] = osg::Vec4(N1 ^ tempvec, 0);
		tempvec = N2 ^ T2;
		(*T_)[iB] = osg::Vec4(tempvec ^ N2, 0);
		tempvec = B2 ^ N2;
		(*B_)[iB] = osg::Vec4(N2 ^ tempvec, 0);
		tempvec = N3 ^ T3;
		(*T_)[iC] = osg::Vec4(tempvec ^ N3, 0);
		tempvec = B3 ^ N3;
		(*B_)[iC] = osg::Vec4(N3 ^ tempvec, 0);

		(*N_)[iA] += osg::Vec4(N1, 0);
		(*N_)[iB] += osg::Vec4(N2, 0);
		(*N_)[iC] += osg::Vec4(N3, 0);
	}
	else {
		osg::Vec3 face_normal = (P2 - P1) ^ (P3 - P1);

		osg::Vec3 V;

		V = osg::Vec3(P2.x() - P1.x(), uv2.x() - uv1.x(), uv2.y() - uv1.y()) ^
			osg::Vec3(P3.x() - P1.x(), uv3.x() - uv1.x(), uv3.y() - uv1.y());
		if (V.x() != 0) {
			V.normalize();
			(*T_)[iA].x() += -V.y() / V.x();
			(*B_)[iA].x() += -V.z() / V.x();
			(*T_)[iB].x() += -V.y() / V.x();
			(*B_)[iB].x() += -V.z() / V.x();
			(*T_)[iC].x() += -V.y() / V.x();
			(*B_)[iC].x() += -V.z() / V.x();
		}

		V = osg::Vec3(P2.y() - P1.y(), uv2.x() - uv1.x(), uv2.y() - uv1.y()) ^
			osg::Vec3(P3.y() - P1.y(), uv3.x() - uv1.x(), uv3.y() - uv1.y());
		if (V.x() != 0) {
			V.normalize();
			(*T_)[iA].y() += -V.y() / V.x();
			(*B_)[iA].y() += -V.z() / V.x();
			(*T_)[iB].y() += -V.y() / V.x();
			(*B_)[iB].y() += -V.z() / V.x();
			(*T_)[iC].y() += -V.y() / V.x();
			(*B_)[iC].y() += -V.z() / V.x();
		}

		V = osg::Vec3(P2.z() - P1.z(), uv2.x() - uv1.x(), uv2.y() - uv1.y()) ^
			osg::Vec3(P3.z() - P1.z(), uv3.x() - uv1.x(), uv3.y() - uv1.y());
		if (V.x() != 0) {
			V.normalize();
			(*T_)[iA].z() += -V.y() / V.x();
			(*B_)[iA].z() += -V.z() / V.x();
			(*T_)[iB].z() += -V.y() / V.x();
			(*B_)[iB].z() += -V.z() / V.x();
			(*T_)[iC].z() += -V.y() / V.x();
			(*B_)[iC].z() += -V.z() / V.x();
		}

		(*N_)[iA] += osg::Vec4(face_normal, 0);
		(*N_)[iB] += osg::Vec4(face_normal, 0);
		(*N_)[iC] += osg::Vec4(face_normal, 0);
	}
}

//void retrieve_mesh_data(
//	float* in_positions,
//	int* in_loop_verts, int loop_count,
//	int* in_loop_tris,
//	int* in_loop_tri_polys, int loop_tri_count,
//	int* in_mat_indices,
//	float* out_positions, unsigned int** out_indices, unsigned int* out_index_lengths)
//{
//	for (int i = 0; i < loop_count; i++)
//	{
//		out_positions[i * 3 + 0] = in_positions[in_loop_verts[i] * 3 + 0];
//		out_positions[i * 3 + 1] = in_positions[in_loop_verts[i] * 3 + 1];
//		out_positions[i * 3 + 2] = in_positions[in_loop_verts[i] * 3 + 2];
//	}
//
//	unsigned int* mat_i = out_index_lengths;
//
//	for (int i = 0; i < loop_tri_count; i++)
//	{
//		int mat = in_mat_indices ? in_mat_indices[in_loop_tri_polys[i]] : 0;
//		out_indices[mat][mat_i[mat]++] = in_loop_tris[i * 3 + 0];
//		out_indices[mat][mat_i[mat]++] = in_loop_tris[i * 3 + 1];
//		out_indices[mat][mat_i[mat]++] = in_loop_tris[i * 3 + 2];
//	}
//}
//
//bool mesh_tangents(
//	int* in_indices, int index_len,
//	float* in_positions, float* in_normals, float* in_uvs,
//	float* out_tangents)
//{
//	struct MData
//	{
//		int* indices;
//		int index_len;
//		float* positions;
//		float* normals;
//		float* uvs;
//		float* tangents;
//	};
//
//	MData data = {
//	  in_indices,
//	  index_len,
//	  in_positions,
//	  in_normals,
//	  in_uvs,
//	  out_tangents,
//	};
//
//	SMikkTSpaceInterface mti = { 0 };
//	mti.m_getNumFaces = [](const SMikkTSpaceContext* pContext) {
//		return ((MData*)pContext->m_pUserData)->index_len / 3;
//		};
//
//	mti.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* pContext, const int iFace) {
//		return 3;
//		};
//
//	mti.m_getPosition = [](const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert) {
//		MData* m = (MData*)pContext->m_pUserData;
//		int i = iFace * 3 + iVert;
//		fvPosOut[0] = m->positions[m->indices[i] * 3 + 0];
//		fvPosOut[1] = m->positions[m->indices[i] * 3 + 1];
//		fvPosOut[2] = m->positions[m->indices[i] * 3 + 2];
//		};
//
//	mti.m_getNormal = [](const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert) {
//		MData* m = (MData*)pContext->m_pUserData;
//		int i = iFace * 3 + iVert;
//		fvNormOut[0] = m->normals[m->indices[i] * 3 + 0];
//		fvNormOut[1] = m->normals[m->indices[i] * 3 + 1];
//		fvNormOut[2] = m->normals[m->indices[i] * 3 + 2];
//		};
//
//	mti.m_getTexCoord = [](const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert) {
//		MData* m = (MData*)pContext->m_pUserData;
//		int i = iFace * 3 + iVert;
//		fvTexcOut[0] = m->uvs[m->indices[i] * 2 + 0];
//		fvTexcOut[1] = m->uvs[m->indices[i] * 2 + 1];
//		};
//
//	mti.m_setTSpaceBasic = [](const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert) {
//		MData* m = (MData*)pContext->m_pUserData;
//		int i = iFace * 3 + iVert;
//		m->tangents[m->indices[i] * 4 + 0] = fvTangent[0];
//		m->tangents[m->indices[i] * 4 + 1] = fvTangent[1];
//		m->tangents[m->indices[i] * 4 + 2] = fvTangent[2];
//		m->tangents[m->indices[i] * 4 + 3] = fSign;
//		};
//
//	SMikkTSpaceContext mtc;
//	mtc.m_pInterface = &mti;
//	mtc.m_pUserData = &data;
//
//	return genTangSpaceDefault(&mtc);
//}
