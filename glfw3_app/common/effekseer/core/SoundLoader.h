
#ifndef	__EFFEKSEER_SOUNDLOADER_H__
#define	__EFFEKSEER_SOUNDLOADER_H__

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
	@brief	�T�E���h�ǂݍ��ݔj���֐��w��N���X
*/
class SoundLoader
{
public:
	/**
		@brief	�R���X�g���N�^
	*/
	SoundLoader() {}

	/**
		@brief	�f�X�g���N�^
	*/
	virtual ~SoundLoader() {}

	/**
		@brief	�T�E���h��ǂݍ��ށB
		@param	path	[in]	�ǂݍ��݌��p�X
		@return	�T�E���h�̃|�C���^
		@note
		�T�E���h��ǂݍ��ށB
		::Effekseer::Effect::Create���s���Ɏg�p�����B
	*/
	virtual void* Load( const EFK_CHAR* path ) { return NULL; }

	/**
		@brief	�T�E���h��j������B
		@param	data	[in]	�T�E���h
		@note
		�T�E���h��j������B
		::Effekseer::Effect�̃C���X�^���X���j�����ꂽ���Ɏg�p�����B
	*/
	virtual void Unload( void* source ) {}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUNDLOADER_H__