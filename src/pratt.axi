pub struct BindingPower {
    pub left: u8,
    pub right: u8,
}

pub fn infix_binding_power(kind: SyntaxKind) -> Option<BindingPower> {
    match kind {
        SyntaxKind.Eq    => Some(BindingPower { left: 2, right: 1 }),
        SyntaxKind.Plus  => Some(BindingPower { left: 9, right: 10 }),
        SyntaxKind.Minus => Some(BindingPower { left: 9, right: 10 }),
        SyntaxKind.Star  => Some(BindingPower { left: 11, right: 12 }),
        SyntaxKind.Slash => Some(BindingPower { left: 11, right: 12 }),
        _ => None,
    }
}

pub fn parse_expression(p: &mut ParserState, min_bp: u8) -> GreenNode {
    let mut lhs = p.bump();

    loop {
        let op = p.current_kind();
        if let Some(bp) = infix_binding_power(op) {
            if bp.left < min_bp {
                break;
            }
            p.consume(op);
            let rhs = parse_expression(p, bp.right);
            lhs = GreenNode.make_binary(op, lhs, rhs);
            continue;
        }
        break;
    }
    lhs
}
