Object subclass: #Tokenize.
Tokenize instanceVariableNames: 'number op message parentheses squareBracket parameter funtionSplit atom assignment expressionEnd result'.
Tokenize class extend [
    new [
        ^(super new init)
    ]
]
Tokenize extend [
    init [
        number := '^\s?(\d+)(.*)'.
        op := '^\s?([\+\-\*/])(.*)'.
        message := '^\s?(\w+:)(.*)'.
        atom := '^\s?(\w+)(.*)'.
        assignment := '^\s?(:=)(.*)'.
        parentheses := '^\s?([\(\)])(.*)'.
        squareBracket := '^\s?([\[\]])(.*)'.
        parameter := '^\s?:(\w+)(.*)'.
        funtionSplit := '^\s?(\|)(.*)'.
        expressionEnd := '^\s?(\.)(.*)'.
        result := OrderedCollection new.
        ^self
    ]
    reset [
        result := OrderedCollection new.
        ^self
    ]
    run: input [
        self parseNumber: input.
        ^(result)
    ]
    parseNumber: input [
        (input =~ number)
            ifMatched: [:match |
                result add: (NodeNumber set: ((match at: 1) asNumber)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseMessage: input]
    ]
    parseMessage: input [
        (input =~ message)
            ifMatched: [:match |
                result add: (NodeAtomMessage set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseAtom: input]
    ]
    parseAtom: input [
        (input =~ atom)
            ifMatched: [:match |
                result add: (NodeAtom set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseOp: input]
    ]
    parseOp: input [
        (input =~ op)
            ifMatched: [:match |
                result add: (NodeOp set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseAssignment: input]
    ]
    parseAssignment: input [
        (input =~ assignment)
            ifMatched: [:match |
                result add: (NodeAssignment set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseParentheses: input]
    ]
    parseParentheses: input [
        (input =~ parentheses)
            ifMatched: [:match |
                result add: (NodeParentheses set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseSquareBracket: input]
    ]
    parseSquareBracket: input [
        (input =~ squareBracket)
            ifMatched: [:match |
                result add: (NodeSquareBracket set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseParameter: input]
    ]
    parseParameter: input [
        (input =~ parameter)
            ifMatched: [:match |
                result add: (NodeParameter set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseFunctionSplit: input]
    ]
    parseFunctionSplit: input [
        (input =~ funtionSplit)
            ifMatched: [:match |
                result add: (NodeFunctionSplit set: (match at: 1)).
                self run: (match at: 2)
            ]
            ifNotMatched: [self parseExpressionEnd: input]
    ]
    parseExpressionEnd: input [
        (input =~ expressionEnd)
            ifMatched: [:match |
                result add: (NodeExpression set: (match at: 1)).
                self run: (match at: 2)
            ]
    ]
]
