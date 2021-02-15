#include "SWF.h"

#define REGISTER_HANDLER( name )	\
	name.Register(this, &FlashEngine::HANDLERNAME(name));	\
	AddHandler(#name, new Functor<void, TYPELIST_2(FlashEngine*, TiXmlHandle*)>(&name, &decltype(name)::Dispatch));

void FlashEngine::RegisterHandlers() {
	REGISTER_HANDLER(ext2);
}