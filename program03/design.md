Original:

<Program> ::= <StmtList>
<StmtList> ::= <Stmt> | <Stmt> <StmtList>
<Stmt> ::= PRINT <Expr> ; | SET ID <Expr> ;
<Expr> ::= <Expr> + <Term> | <Term>
<Term> ::= <Term> * <Primary> | <Primary>
<Primary> ::= ID | <String> | INT | ( <Expr> )
<String> ::= STR | STR [ <Expr> ] | STR [ <Expr> ; <Expr> ]

Fixed:

<Program> ::= <StmtList>
<StmtList> ::= <Stmt> | <Stmt> <StmtList>
<Stmt> ::= PRINT <Expr> ; | SET ID <Expr> ;
<Expr> ::= <Term> <Expr'>
<Expr'> ::= EOL | + <Term> <Expr'>
<Term> ::= <Primary> <Term'>
<Term'> ::= EOL | * <Primary> <Term'>
<Primary> ::= ID | <String> | INT | ( <Expr> )
<String> ::= STR | STR [ <Expr> ] | STR [ <Expr> ; <Expr> ]

Condensed:

<Program> ::= <StmtList>
<StmtList> ::= <Stmt> | <Stmt> <StmtList>
<Stmt> ::= PRINT <Expr> ; | SET ID <Expr> ;
<Expr> ::= <Term> <Expr'>
<Expr'> ::= EOL | + <Expr>
<Term> ::= <Primary> <Term'>
<Term'> ::= EOL | * <Term>
<Primary> ::= ID | <String> | INT | ( <Expr> )
<String> ::= STR | STR [ <Expr> ] | STR [ <Expr> ; <Expr> ]
