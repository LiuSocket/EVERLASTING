//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMNodeVisitor.h
/// @brief		GMEngine - Node Visitor
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.12
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <osg/NodeVisitor>

namespace GM
{
	/*************************************************************************
	 Class
	*************************************************************************/
	/*!
	 *  @class CGMNodeVisitor
	 *  @brief VrEarth Node Visitor
	 */
	template<class T>
	class CGMNodeVisitor : public osg::NodeVisitor
	{
	// 函数
	public:
		/** @brief 构造 */
		CGMNodeVisitor(TraversalMode eMode = TRAVERSE_ALL_CHILDREN)
			: osg::NodeVisitor(eMode), m_pResultNode(nullptr)
		{}

		/** @brief 记录结果 */
		virtual void apply(T& sNode)
		{
			m_pResultNode = &sNode;
			return;
		}

		/** @brief 获取结果 */
		inline T* GetResult() { return m_pResultNode; }

	// 变量
	private:
		T*					m_pResultNode;			//!< 结果节点
	};

	template<class T>
	class CGMMultiNodeVisitor : public osg::NodeVisitor
	{
		// 函数
	public:
		/** @brief 构造 */
		CGMMultiNodeVisitor(TraversalMode eMode = TRAVERSE_ALL_CHILDREN)
			: osg::NodeVisitor(eMode)
		{}

		/** @brief 记录结果 */
		virtual void apply(T& sNode)
		{
			//m_pResultNode = &sNode;
			m_pResultNode.push_back(&sNode);
			return;
		}

		/** @brief 获取结果 */
		inline std::vector<T*> GetResult() { return m_pResultNode; }

		// 变量
	private:
		std::vector<T*> m_pResultNode;			//!< 结果节点
	};

}	// GM
