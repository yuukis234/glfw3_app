
#ifndef	__EFFEKSEER_TEXTURELOADER_H__
#define	__EFFEKSEER_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	テクスチャ読み込み破棄関数指定クラス
*/
class TextureLoader
{
public:
	/**
		@brief	コンストラクタ
	*/
	TextureLoader() {}

	/**
		@brief	デストラクタ
	*/
	virtual ~TextureLoader() {}

	/**
		@brief	テクスチャを読み込む。
		@param	path	[in]	読み込み元パス
		@return	テクスチャのポインタ
		@note
		テクスチャを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	テクスチャを破棄する。
		@param	data	[in]	テクスチャ
		@note
		テクスチャを破棄する。
		::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload( void* data ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_TEXTURELOADER_H__
