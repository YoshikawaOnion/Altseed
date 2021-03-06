﻿#pragma once

#include "../asd.ReferenceObject.h"
#include "../asd.Core.Base.h"

namespace asd
{
	/**
		@brief	部分的にファイルを読み込むクラス
	*/
	class StreamFile
		: public IReference
	{
		friend class Accessor;

	protected:
		virtual int32_t Read_(int32_t size) = 0;
		virtual void* GetTempBuffer_() = 0;
		virtual int32_t GetTempBufferSize_() = 0;

	public:
		StreamFile() {}
		virtual ~StreamFile() { };
		
		/**
		@brief	ファイルのサイズを取得する。
		@return	サイズ
		*/
		virtual int32_t GetSize() const = 0;

		/**
		@brief	ファイル内で現在読み込んでいる位置を取得する。
		@return	現在読み込んでいる位置
		*/
		virtual int32_t GetCurrentPosition() const = 0;

#if !SWIG
		/**
			@brief	指定したサイズ分、ファイルを読み込む。
			@param	buffer	出力先
			@param	size	読み込まれるサイズ
		*/
		void Read(std::vector<uint8_t>& buffer, int32_t size)
		{
			auto result = Read_(size);
			buffer.resize(result);
			memcpy(buffer.data(), GetTempBuffer_(), result);
		}
#endif
	};
}