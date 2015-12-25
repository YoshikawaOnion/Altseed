﻿
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../asd.common.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	/**
	@brief	色
	*/
	struct Color
	{
	public:

		/**
		@brief	赤
		*/
		uint8_t	R;

		/**
		@brief	緑
		*/
		uint8_t	G;

		/**
		@brief	青
		*/
		uint8_t	B;

		/**
		@brief	透明度
		*/
		uint8_t	A;

		/**
		@brief	コンストラクタ
		*/
		Color();

		/**
		@brief	コンストラクタ
		@param	r	赤
		@param	g	緑
		@param	b	青
		@param	a	透明度
		*/
		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		Color operator*(const Color& right);

		Color& operator*=(const Color& right);
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
