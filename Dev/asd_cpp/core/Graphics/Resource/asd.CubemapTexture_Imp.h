﻿
#include "asd.CubemapTexture.h"
#include "../asd.DeviceObject.h"

namespace asd
{
	class CubemapTexture_Imp
		: public CubemapTexture
		, public DeviceObject
	{
	protected:
		CubemapTexture_Imp(Graphics* graphics, TextureFormat format, Vector2DI size, int32_t mipmapCount);
		virtual ~CubemapTexture_Imp();


		Vector2DI	size;
		int32_t		mipmapCount = 0;
		TextureFormat	format;
	public:

		Vector2DI GetSize() const { return size; }
		int32_t GetMipmapCount() const { return mipmapCount; }

		// IReferenceを継承したデバイスオブジェクト向け定義
#if !SWIG
	public:
		virtual int GetRef() { return ReferenceObject::GetRef(); }
		virtual int AddRef() { return ReferenceObject::AddRef(); }
		virtual int Release() { return ReferenceObject::Release(); }
#endif
	};
}