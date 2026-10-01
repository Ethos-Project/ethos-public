pub struct McpSymbolQuery {
    pub symbol_hash: u64,
    pub verify_bounds: bool,
}

pub struct McpSymbolResponse {
    pub symbol_hash: u64,
    pub is_verified: bool,
    pub contract_count: u8,
}

// Native entry point for editor (LSP) and AI agent (MCP) context requests
pub fn handle_mcp_symbol_request(db: &QueryDb, query: McpSymbolQuery) -> McpSymbolResponse {
    let verified = verify_item_contracts(db, query.symbol_hash);

    McpSymbolResponse {
        symbol_hash: query.symbol_hash,
        is_verified: verified,
        contract_count: 2,
    }
}
