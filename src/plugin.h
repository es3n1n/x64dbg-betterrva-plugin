#pragma once
#include "pluginmain.h"
#include "icons.h"

// @note: @es3n1n: Context item ids
// Still kinda sucks that we need to register each
// unique ctx item to each context menu, but whatever :shrug:
//
enum class e_menu_ctx_item : std::uint32_t {
	DISASM_BEGIN = 0,
	copy_rva_disasm,
	copy_base_plus_rva_disasm,
	copy_va_minus_base_disasm,
	DISASM_END,

	DUMP_BEGIN,
	copy_rva_dump,
	copy_base_plus_rva_dump,
	copy_va_minus_base_dump,
	DUMP_END,
};

namespace util {
	GUISELECTIONTYPE get_window_for_ctx_item( const e_menu_ctx_item it );
	void set_clipboard_data( const char* fmt, ... );
}

namespace core {
	struct selected_addr_t {
	public:
		bool m_success = false;
		std::uintptr_t m_base = 0x0;
		std::uintptr_t m_address = 0x0;
	};
	selected_addr_t get_selected_addr( const e_menu_ctx_item item );
}

namespace handlers {
	void copy_rva( const e_menu_ctx_item item );
	void copy_base_plus_rva( const e_menu_ctx_item item );
	void copy_va_minus_base( const e_menu_ctx_item item );
}

namespace plugin {
	void initial( );
	void menu_click( PLUG_CB_MENUENTRY* info );
}