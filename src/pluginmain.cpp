#include "pluginmain.h"
#include "plugin.h"


namespace g {
	int disasm_menu = 0;
	int dump_menu = 0;
}


PLUGIN_EXPORT bool pluginit( PLUG_INITSTRUCT* initStruct ) {
	initStruct->pluginVersion = PLUGIN_VERSION;
	initStruct->sdkVersion = PLUG_SDKVERSION;
	strncpy_s( initStruct->pluginName, PLUGIN_NAME, _TRUNCATE );
	return true;
}


PLUGIN_EXPORT bool plugstop( ) {
	_plugin_menuclear( g::disasm_menu );
	_plugin_menuclear( g::dump_menu );
	return true;
}


PLUGIN_EXPORT void plugsetup( PLUG_SETUPSTRUCT* setupStruct ) {
	g::disasm_menu = setupStruct->hMenuDisasm;
	g::dump_menu = setupStruct->hMenuDump;
	plugin::initial( );
}


PLUGIN_EXPORT void CBMENUENTRY( CBTYPE cbType, PLUG_CB_MENUENTRY* info ) {
	plugin::menu_click( info );
}
