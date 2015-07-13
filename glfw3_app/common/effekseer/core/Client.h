
#ifndef	__EFFEKSEER_CLIENT_H__
#define	__EFFEKSEER_CLIENT_H__

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
class Client
{
public:
	Client() {}
	virtual ~Client() {}

	static Client* Create();

	virtual bool Start( char* host, uint16_t port ) = 0;
	virtual void Stop()= 0;

	virtual void Reload( const EFK_CHAR* key, void* data, int32_t size ) = 0;
	virtual void Reload( Manager* manager, const EFK_CHAR* path, const EFK_CHAR* key ) = 0;
	virtual bool IsConnected() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_CLIENT_H__