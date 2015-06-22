
#ifndef	__EFFEKSEER_COLOR_H__
#define	__EFFEKSEER_COLOR_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ColorMode
{
	COLOR_MODE_RGBA,
	COLOR_MODE_HSVA,
	COLOR_MODE_DWORD = 0x7FFFFFFF
};

/**
	@brief	色
*/
#pragma pack(push,1)
struct Color
{
	/**
		@brief	赤
	*/
	uint8_t		R;

	/**
		@brief	緑
	*/
	uint8_t		G;

	/**
		@brief	青
	*/
	uint8_t		B;
	
	/**
		@brief	透明度
	*/
	uint8_t		A;

	/**
		@brief	コンストラクタ
	*/
	Color();

	/**
		@brief	コンストラクタ
	*/
	Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );

	/**
		@brief	乗算
	*/
	static void Mul( Color& o, const Color& in1, const Color& in2 );
};
#pragma pack(pop)
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_COLOR_H__
