﻿#pragma once
#include <memory>
#include "asd.CoreGeometryObject2D.h"
#include "../../Shape/asd.CoreShape.h"
#include "asd.CoreObject2D_Imp.h"
#include "asd.DrawnParentInfo2D.h"

namespace asd
{

	class CoreGeometryObject2D_Imp
		: public CoreGeometryObject2D
		, public CoreObject2D_Imp
		, public ReferenceObject
	{
		CoreShape* m_shape;
		int drawingPriority;
		AlphaBlendMode alphaBlendMode;
		Vector2DF centerPosition;
		Color m_color;
		TextureFilterType m_textureFilterType;
		Texture2D* m_texture;

	public:
		Object2DType GetObjectType() const override { return Object2DType::Geometry; }

		CoreGeometryObject2D_Imp(Graphics_Imp* graphics);
		virtual ~CoreGeometryObject2D_Imp();

		CoreShape* GetShape() const;
		void SetShape(CoreShape* shape);

		int GetDrawingPriority() const ;
		void SetDrawingPriority(int priority) ;

		AlphaBlendMode GetAlphaBlendMode() const ;
		void SetAlphaBlendMode(AlphaBlendMode alphaBlend);

		void SetCenterPosition(Vector2DF position) ;
		Vector2DF GetCenterPosition() const ;

		void SetColor(Color color) ;
		Color GetColor() const ;

		void SetTextureFilterType(TextureFilterType textureFilterType) ;
		TextureFilterType GetTextureFilterType() const ;

		void SetTexture(Texture2D* texture) ;
		Texture2D* GetTexture() const;
#if !SWIG

		void Draw(Renderer2D* renderer) override;

		virtual void CalculateBoundingCircle() override;

		void SetIsUpdateBoundingCircle();
#endif

#include "asd.CoreObject2D_Imp_Methods.h"
#include "asd.CoreDrawnObject2D_Imp_Methods.h"

	};
}