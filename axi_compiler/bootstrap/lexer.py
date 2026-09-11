import re
from enum import Enum, auto
from dataclasses import dataclass
from typing import List


class SyntaxKind(Enum):
    Whitespace = auto()
    Comment = auto()
    Indent = auto()
    Dedent = auto()
    ErrorToken = auto()
    Ident = auto()
    IntLit = auto()
    KwFn = auto()
    KwPub = auto()
    KwStruct = auto()
    KwEnum = auto()
    KwMatch = auto()
    KwLet = auto()
    KwMut = auto()
    KwLoop = auto()
    KwBreak = auto()
    KwContinue = auto()
    KwIf = auto()
    KwElse = auto()
    KwReturn = auto()
    Plus = auto()
    Minus = auto()
    Star = auto()
    Slash = auto()
    Eq = auto()
    EqEq = auto()
    Colon = auto()
    Semi = auto()
    Comma = auto()
    Dot = auto()
    LBrace = auto()
    RBrace = auto()
    LParen = auto()
    RParent = auto()
    LBracket = auto()
    RBracket = auto()
    Arrow = auto()
    FatArrow = auto()
    At = auto()
    Ampersand = auto()
    LAngle = auto()
    RAngle = auto()
    Tick = auto()


@dataclass
class Token:
    kind: SyntaxKind
    text: str
    offset: int


class AxiLexer:
    def __init__(self, source: str):
        self.source = source
        self.offset = 0
        self.tokens: List[Token] = []

        # Indent tracking
        self.indent_stack = [0]
        self.is_start_of_line = True

    def lex(self) -> List[Token]:
        lines = self.source.splitlines(keepends=True)
        for line in lines:
            self._process_line(line)

        # Emit dedents at EOF
        while len(self.indent_stack) > 1:
            self.indent_stack.pop()
            self.tokens.append(Token(SyntaxKind.Dedent, "", self.offset))

        return self.tokens

    def _process_line(self, line: str):
        # 1. Process Indentation
        spaces = len(line) - len(line.lstrip(" "))
        stripped = line.lstrip()

        if stripped and not stripped.startswith("//"):
            if spaces > self.indent_stack[-1]:
                self.indent_stack.append(spaces)
                self.tokens.append(Token(SyntaxKind.Indent, " " * spaces, self.offset))
            elif spaces < self.indent_stack[-1]:
                while len(self.indent_stack) > 1 and spaces < self.indent_stack[-1]:
                    self.indent_stack.pop()
                    self.tokens.append(Token(SyntaxKind.Dedent, "", self.offset))

        # 2. Tokenize line content
        pos = spaces
        while pos < len(line):
            match = None

            # Skip whitespace
            ws_match = re.match(r"\s+", line[pos:])
            if ws_match:
                # self.tokens.append(Token(SyntaxKind.Whitespace, ws_match.group(0), self.offset + pos))
                pos += len(ws_match.group(0))
                continue

            # Comments
            if line[pos:].startswith("//"):
                self.tokens.append(
                    Token(SyntaxKind.Comment, line[pos:], self.offset + pos)
                )
                break

            # Keywords and Identifiers
            ident_match = re.match(r"[a-zA-Z_]\w*", line[pos:])
            if ident_match:
                text = ident_match.group(0)
                kind = SyntaxKind.Ident
                keywords = {
                    "pub": SyntaxKind.KwPub,
                    "fn": SyntaxKind.KwFn,
                    "struct": SyntaxKind.KwStruct,
                    "enum": SyntaxKind.KwEnum,
                    "match": SyntaxKind.KwMatch,
                    "let": SyntaxKind.KwLet,
                    "mut": SyntaxKind.KwMut,
                    "loop": SyntaxKind.KwLoop,
                    "break": SyntaxKind.KwBreak,
                    "continue": SyntaxKind.KwContinue,
                    "if": SyntaxKind.KwIf,
                    "else": SyntaxKind.KwElse,
                    "return": SyntaxKind.KwReturn,
                }
                if text in keywords:
                    kind = keywords[text]

                self.tokens.append(Token(kind, text, self.offset + pos))
                pos += len(text)
                continue

            # Operators
            symbols = {
                "->": SyntaxKind.Arrow,
                "=>": SyntaxKind.FatArrow,
                "==": SyntaxKind.EqEq,
                "=": SyntaxKind.Eq,
                "+": SyntaxKind.Plus,
                "-": SyntaxKind.Minus,
                "*": SyntaxKind.Star,
                "/": SyntaxKind.Slash,
                ":": SyntaxKind.Colon,
                ";": SyntaxKind.Semi,
                ",": SyntaxKind.Comma,
                ".": SyntaxKind.Dot,
                "{": SyntaxKind.LBrace,
                "}": SyntaxKind.RBrace,
                "(": SyntaxKind.LParen,
                ")": SyntaxKind.RParent,
                "[": SyntaxKind.LBracket,
                "]": SyntaxKind.RBracket,
                "@": SyntaxKind.At,
                "&": SyntaxKind.Ampersand,
                "<": SyntaxKind.LAngle,
                ">": SyntaxKind.RAngle,
                "'": SyntaxKind.Tick,
            }

            for sym, kind in symbols.items():
                if line[pos:].startswith(sym):
                    self.tokens.append(Token(kind, sym, self.offset + pos))
                    pos += len(sym)
                    match = True
                    break

            if match:
                continue

            # Int Literals
            int_match = re.match(r"\d+", line[pos:])
            if int_match:
                self.tokens.append(
                    Token(SyntaxKind.IntLit, int_match.group(0), self.offset + pos)
                )
                pos += len(int_match.group(0))
                continue

            # Error fallback
            self.tokens.append(
                Token(SyntaxKind.ErrorToken, line[pos], self.offset + pos)
            )
            pos += 1

        self.offset += len(line)


if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        with open(sys.argv[1], "r") as f:
            code = f.read()
            lexer = AxiLexer(code)
            tokens = lexer.lex()
            for t in tokens:
                print(f"{t.kind.name: <15} '{t.text.strip()}'")

