#include "plugin.h"


namespace util {
	// @note: @es3n1n: Get current window based on context menu item that was clicked on
	//
	GUISELECTIONTYPE get_window_for_ctx_item( const e_menu_ctx_item it ) {
		if ( it >= e_menu_ctx_item::DISASM_BEGIN &&
			 it <= e_menu_ctx_item::DISASM_END )
			return GUISELECTIONTYPE::GUI_DISASSEMBLY;

		if ( it >= e_menu_ctx_item::DUMP_BEGIN &&
			 it <= e_menu_ctx_item::DUMP_END )
			return GUISELECTIONTYPE::GUI_DUMP;

		dprintf( "Error! Unknown ctx item %d\n", it );
		return GUISELECTIONTYPE::GUI_DISASSEMBLY;
	}

	// @note: @es3n1n: Format and copy data to clipboard
	//
	void set_clipboard_data( const char* fmt, ... ) {
		char buffer[ 1024 ];
		va_list va;
		va_start( va, fmt );
		_vsnprintf_s( buffer, 1024, fmt, va );
		va_end( va );

		dprintf( "Copying '%s'\n", buffer );

		auto txt_handle = GlobalAlloc( GHND | GMEM_DDESHARE, std::strlen( buffer ) );
		if ( !txt_handle ) {
			dprintf( "Error! Unable to allocate handle for text\n" );
			return;
		}

		if ( !OpenClipboard( NULL ) ) {
			dprintf( "Error! Unable to open clipboard\n" );
			GlobalFree( txt_handle );
			return;
		}

		SetClipboardData( CF_TEXT, txt_handle );
		CloseClipboard( );
		GlobalFree( txt_handle );
	}
}


namespace core {
	// @note: @es3n1n: Resolving region base address and current address
	// using x64dbg script api
	//
	selected_addr_t get_selected_addr( const e_menu_ctx_item item ) {
		if ( !DbgIsDebugging( ) )
			return { false, 0, 0 };

		SELECTIONDATA sel;
		GuiSelectionGet( util::get_window_for_ctx_item( item ), &sel );
		if ( !sel.start )
			return { false, 0, 0 };

		// @note: @es3n1n: If base address is inside of some loaded module
		//
		std::uintptr_t mod_base = Script::Module::BaseFromAddr( sel.start );
		if ( mod_base )
			return { true, mod_base, sel.start };

		return { true, Script::Memory::GetBase( sel.start ), sel.start };
	}
}


namespace handlers {
	// @note: @es3n1n: Context menu "Copy RVA" handler
	//
	void copy_rva( const e_menu_ctx_item item ) {
		auto result = core::get_selected_addr( item );

		if ( !result.m_success ) {
			dprintf( "Error! Address or base was invalid\n" );
			return;
		}

		util::set_clipboard_data( 
			"0x%p", 
			reinterpret_cast< void* >( result.m_address - result.m_base ) 
		);
	}


	// @note: @es3n1n: Context menu "Copy Base + RVA" handler
	//
	static void copy_base_plus_rva( const e_menu_ctx_item item ) {
		auto result = core::get_selected_addr( item );

		if ( !result.m_success ) {
			dprintf( "Error! Address or base was invalid\n" );
			return;
		}

		util::set_clipboard_data( 
			"0x%p + 0x%p", 
			reinterpret_cast< void* >( result.m_base ), 
			reinterpret_cast< void* >( result.m_address - result.m_base ) 
		);
	}


	// @note: @es3n1n: Context menu "Copy VA - Base" handler
	//
	static void copy_va_minus_base( const e_menu_ctx_item item ) {
		auto result = core::get_selected_addr( item );

		if ( !result.m_success ) {
			dprintf( "Error! Address or base was invalid\n" );
			return;
		}

		util::set_clipboard_data(
			"0x%p - 0x%p",
			reinterpret_cast< void* >( result.m_address ),
			reinterpret_cast< void* >( result.m_base )
		);
	}
}


namespace plugin {
	// @note: @es3n1n: Init context menu buttons
	//
	void initial( ) {
		ICONDATA clown_icon;
		clown_icon.data = icons::clown;
		clown_icon.size = sizeof( icons::clown );

		_plugin_menuseticon( g::disasm_menu, &clown_icon );
		_plugin_menuseticon( g::dump_menu, &clown_icon );

	#define ADD_ENTRY( en, name ) _plugin_menuaddentry( g::disasm_menu, static_cast< std::uint32_t >( e_menu_ctx_item::##en##_disasm ), "&" name ); \
	_plugin_menuaddentry( g::dump_menu, static_cast< std::uint32_t >( e_menu_ctx_item::##en##_dump ), "&" name );

		ADD_ENTRY( copy_rva, "Copy RVA" );
		ADD_ENTRY( copy_base_plus_rva, "Copy Base + RVA" );
		ADD_ENTRY( copy_va_minus_base, "Copy VA - Base" );

	#undef ADD_ENTRY

		dprintf( "Initialized! Plugin version: %d. Author: https://github.com/es3n1n\n", PLUGIN_VERSION );
	}

	// @note: @es3n1n: Handle context menu button click
	//
	void menu_click( PLUG_CB_MENUENTRY* info ) {
		const auto entry = static_cast< e_menu_ctx_item >( info->hEntry );

		switch ( entry ) {
			case e_menu_ctx_item::copy_rva_disasm:
			case e_menu_ctx_item::copy_rva_dump:
				return handlers::copy_rva( entry );

			case e_menu_ctx_item::copy_base_plus_rva_disasm:
			case e_menu_ctx_item::copy_base_plus_rva_dump:
				return handlers::copy_base_plus_rva( entry );

			case e_menu_ctx_item::copy_va_minus_base_disasm:
			case e_menu_ctx_item::copy_va_minus_base_dump:
				return handlers::copy_va_minus_base( entry );
		}
	}
}
