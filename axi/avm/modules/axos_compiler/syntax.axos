// Fixed-width tagged enum defining lossy & lossless syntax primitives
pub enum SyntaxKind : u16 {
    // Trivia & Tokens
    Whitespace = 0,
    Comment,
    Indent,
    Dedent,
    ErrorToken,
    Ident,
    IntLit,

    // Keywords & Operators
    KwFn,
    Kwaxi,
    KwContract,
    Plus,
    Minus,
    Star,
    Slash,
    Eq,

    // Composite CST Nodes
    SourceFile,
    FnDef,
    ExprBinary,
}

// Immutable Green Node: Position-independent for structural sharing
pub struct GreenNode {
    pub kind: SyntaxKind,
    pub text_len: u32,
    pub child_count: u16,
    pub children: [GreenNode; 16], // Pre-allocated or arena-backed
}

// Lazy Red Node: Ephemeral facade calculating absolute coordinates on demand
pub struct RedNode<'a> {
    pub green: &'a GreenNode,
    pub abs_offset: u32,
    pub parent: Option<&'a RedNode<'a>>,
}
