import sys
from typing import List, Optional
from dataclasses import dataclass
from lexer import AxiLexer, Token, SyntaxKind


@dataclass
class ExprNode:
    pass


@dataclass
class LiteralNode(ExprNode):
    value: str


@dataclass
class IdentNode(ExprNode):
    name: str


@dataclass
class BinaryNode(ExprNode):
    op: SyntaxKind
    lhs: ExprNode
    rhs: ExprNode


class PrattParser:
    def __init__(self, tokens: List[Token]):
        # Filter out whitespace and comments for the syntax tree parser
        self.tokens = [
            t
            for t in tokens
            if t.kind
            not in (
                SyntaxKind.Whitespace,
                SyntaxKind.Comment,
                SyntaxKind.Indent,
                SyntaxKind.Dedent,
            )
        ]
        self.pos = 0

    def current(self) -> Optional[Token]:
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return None

    def bump(self) -> Optional[Token]:
        t = self.current()
        self.pos += 1
        return t

    def prefix_binding_power(self, kind: SyntaxKind) -> Optional[int]:
        if kind in (SyntaxKind.Minus, SyntaxKind.Plus):
            return 13
        return None

    def infix_binding_power(self, kind: SyntaxKind) -> Optional[tuple[int, int]]:
        if kind == SyntaxKind.Eq:
            return (2, 1)  # Right associative
        elif kind in (SyntaxKind.Plus, SyntaxKind.Minus):
            return (9, 10)  # Left associative
        elif kind in (SyntaxKind.Star, SyntaxKind.Slash):
            return (11, 12)  # Left associative
        return None

    def parse_expression(self, min_bp: int = 0) -> ExprNode:
        token = self.bump()
        if not token:
            raise RuntimeError("Unexpected EOF")

        # Parse prefix or atom
        if token.kind == SyntaxKind.IntLit:
            lhs = LiteralNode(token.text)
        elif token.kind == SyntaxKind.Ident:
            lhs = IdentNode(token.text)
        elif token.kind == SyntaxKind.LParen:
            lhs = self.parse_expression(0)
            if self.current() and self.current().kind == SyntaxKind.RParent:
                self.bump()
            else:
                raise RuntimeError("Expected closing parenthesis")
        else:
            bp = self.prefix_binding_power(token.kind)
            if bp is not None:
                rhs = self.parse_expression(bp)
                # For simplicity, representing unary as binary with no LHS
                lhs = BinaryNode(token.kind, None, rhs)
            else:
                raise RuntimeError(f"Unexpected token {token.kind.name}")

        # Parse infix
        while True:
            op_token = self.current()
            if not op_token:
                break

            bp = self.infix_binding_power(op_token.kind)
            if bp is None:
                break

            left_bp, right_bp = bp
            if left_bp < min_bp:
                break

            self.bump()  # consume operator
            rhs = self.parse_expression(right_bp)
            lhs = BinaryNode(op_token.kind, lhs, rhs)

        return lhs


def print_ast(node: ExprNode, indent=0):
    pad = "  " * indent
    if isinstance(node, LiteralNode):
        print(f"{pad}Literal({node.value})")
    elif isinstance(node, IdentNode):
        print(f"{pad}Ident({node.name})")
    elif isinstance(node, BinaryNode):
        print(f"{pad}Binary({node.op.name})")
        if node.lhs:
            print_ast(node.lhs, indent + 1)
        print_ast(node.rhs, indent + 1)


if __name__ == "__main__":
    # A simple test for the Pratt parser
    equation = "velocity = distance / time + acceleration * time"
    print(f"Parsing equation: {equation}")
    lexer = AxiLexer(equation)
    tokens = lexer.lex()
    parser = PrattParser(tokens)

    ast = parser.parse_expression(0)
    print("\nRed-Green CST Output:")
    print_ast(ast)

