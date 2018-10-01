FileStream fileIn: 'Node.st'.
FileStream fileIn: 'VariableTable.st'.
FileStream fileIn: 'FunctionTable.st'.
FileStream fileIn: 'LittleStackMachine.st'.


Object subclass: #Tokenize.
Tokenize instanceVariableNames: 'number op parentheses squareBracket parameter closureSplit atom assignment expressionEnd result'.
Tokenize class extend [
    new [
        ^(super new init)
    ]
]
Tokenize extend [
    init [
        number := '^\s?(\d+)(.*)'.
        op := '^\s?([\+\-\*/])(.*)'.
        atom := '^\s?(\w+)(.*)'.
        assignment := '^\s?(:=)(.*)'.
        parentheses := '^\s?([\(\)])(.*)'.
        squareBracket := '^\s?([\[\]])(.*)'.
        parameter := '^\s?:(\w+)(.*)'.
        closureSplit := '^\s?(\|)(.*)'.
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
            ifNotMatched: [self parseClosureSplit: input]
    ]
    parseClosureSplit: input [
        (input =~ closureSplit)
            ifMatched: [:match |
                result add: (NodeClosureSplit set: (match at: 1)).
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


Object subclass: #ProcessParentheses.
ProcessParentheses instanceVariableNames: 'modifyPriority'.
ProcessParentheses extend [
    init [
        modifyPriority := 0.
        ^self
    ]
    run: input [
        modifyPriority := 0.
        input collect: [:next | self isOp: next].
        ^(input reject: [:next | (next class) = NodeParentheses])
    ]
    isOp: node [
        ((node class) = NodeOp)
            ifTrue: [
                ^(node addPriority: modifyPriority)
            ]
            ifFalse: [^(self isParentheses: node)]
    ]
    isParentheses: node [
        ((node class) = NodeParentheses)
            ifTrue: [
                self isLeft: node.
                self isRight: node.
                ^self
            ]
            ifFalse: [^node]
    ]
    isLeft: node [
        ((node value) = '(')
            ifTrue: [
                modifyPriority := modifyPriority + (node priority)
            ]
    ]
    isRight: node [
        ((node value) = ')')
            ifTrue: [
                modifyPriority := modifyPriority - (node priority)
            ]
    ]
]


Object subclass: #ProcessAssignmentPriority.
ProcessAssignmentPriority instanceVariableNames: 'modifyPriority'.
ProcessAssignmentPriority extend [
    init [
        modifyPriority := 0.
        ^self
    ]
    run: input [
        modifyPriority := 0.
        ^(input collect: [:next | self isOp: next])
    ]
    isOp: node [
        ((node class) = NodeOp)
            ifTrue: [
                ^(node addPriority: modifyPriority)
            ]
            ifFalse: [^(self isExpressionEnd: node)]
    ]
    isExpressionEnd: node [
        ((node class) = NodeExpression)
            ifTrue: [
                modifyPriority := 0.
                ^node
            ]
            ifFalse: [^(self isAssignment: node)]
    ]
    isAssignment: node [
        ((node value) = ':=')
            ifTrue: [
                modifyPriority := modifyPriority + (node priority).
                ^node
            ]
            ifFalse: [
                ^(self isOthers: node)
            ]
    ]
    isOthers: node [
        ^node
    ]
]


Object subclass: #ProcessAtom.
ProcessAtom instanceVariableNames: 'result'.
ProcessAtom extend [
    run: input [
        result := input collectWithNext: [:current :next |
            ((next class) = NodeAssignment) & (current class = NodeAtom)
                ifTrue: [AssignVariable upgradeFrom: current]
                ifFalse: [current]
        ].
        result := result collect: [:current |
            ((current class) = NodeAtom)
                ifTrue: [NodeVariable upgradeFrom: current]
                ifFalse: [current]
        ].
        ^(result reject: [:next | (next class) = NodeAssignment])
    ]
]


Object subclass: #ProcessClosure.
ProcessClosure instanceVariableNames: 'result'.
ProcessClosure extend [
    run: input [
        result := input collectWithNext: [:current :next |
            ((next class) = NodeSquareBracket) & (current class = AssignVariable)
                ifTrue: [AssignClosure upgradeFrom: current]
                ifFalse: [current]
        ].
        ^result
    ]
]


Object subclass: #ReplaceVariable.
ReplaceVariable instanceVariableNames: 'table'.
ReplaceVariable extend [
    init [
        table := VariableTable init.
        ^self
    ]
    run: input [
        ^(input collect: [:current | self isAssignVariable: current])
    ]
    isAssignVariable: input [
        |value|
        ((input class) = AssignVariable)
            ifTrue: [
                value := table add: (input value).
                ^(input set: value)
            ]
            ifFalse: [^(self isParameter: input)]
    ]
    isParameter: input [
        |value|
        ((input class) = NodeParameter)
            ifTrue: [
                value := table add: (input value).
                ^(input set: value)
            ]
            ifFalse: [^(self isNodeVariable: input)]
    ]
    isNodeVariable: input [
        |value|
        ((input class) = NodeVariable)
            ifTrue: [
                value := table find: (input value).
                ^(input set: value)
            ]
            ifFalse: [^input]
    ]
]


Object subclass: #ReplaceClosure.
ReplaceClosure instanceVariableNames: 'table'.
ReplaceClosure extend [
    init [
        table := FunctionTable init.
        ^self
    ]
    run: input [
        ^(input collect: [:current | self isAssignClosure: current])
    ]
    isAssignClosure: input [
        |value|
        ((input class) = AssignClosure)
            ifTrue: [
                value := table add: (input value).
                ^(input set: value)
            ]
            ifFalse: [^input]
    ]
]


Object subclass: #InfixToPostfix.
InfixToPostfix instanceVariableNames: 'stack result'.
InfixToPostfix extend [
    run: input [
        result := OrderedCollection new.
        stack := OrderedCollection new.
        input collect: [ :next |
            self isNumber: next
        ].
        result addAll: stack.
        ^result
    ]
    isNumber: value [
        (value class) = NodeNumber
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isVariable: value
            ]
    ]
    isVariable: value [
        (value class) = NodeVariable
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isExpression: value
            ]
    ]
    isExpression: value [
        (value class) = NodeExpression
            ifTrue: [
                result addAll: stack.
                stack := OrderedCollection new.
                result add: value
            ]
            ifFalse: [
                self isStackNull: value
            ]
    ]
    isStackNull: value [
        ((stack size) = 0)
            ifTrue: [
                stack addFirst: value
            ]
            ifFalse: [
                self isSmallerPriority: value
            ]
    ]
    isSmallerPriority: value [
        ((stack at: 1) isSmallerPriority: value)
            ifTrue: [
                stack addFirst: value
            ]
            ifFalse: [
                self isEqualPriority: value
            ]
    ]
    isEqualPriority: value [
        ((stack at: 1) isEqualPriority: value)
            ifTrue: [
                result add: (stack removeFirst).
                stack add: value
            ]
            ifFalse: [
                self isBiggerPriority: value
            ]
    ]
    isBiggerPriority: value [
        ((stack at: 1) isBiggerPriority: value)
            ifTrue: [
                result add: (stack removeFirst).
                stack add: value
            ]
    ]
]


Object subclass: #RemoveClosureSplit.
RemoveClosureSplit extend [
    run: input [
        ^(input reject: [:next | (next class) = NodeClosureSplit])
    ]
]


Object subclass: #RemoveClosureSplit.
RemoveClosureSplit extend [
    run: input [
        ^(input reject: [:next | (next class) = NodeClosureSplit])
    ]
]


Object subclass: #StoreClosure.
StoreClosure instanceVariableNames: 'result stack turn'.
StoreClosure extend [
    run: input [
        turn := false.
        result := OrderedCollection new.
        input collect: [:current | self isLeftBracket: current].
        result := result collectWithNext: [:current :next |
            ((next class) = ClosureBody) & (current class = AssignClosure)
                ifTrue: [NodeClosure upgradeFrom: current and: next]
                ifFalse: [current]
        ].
        ^(result reject: [:current | (current class) = ClosureBody])
    ]
    isLeftBracket: input [
        ((input class) = NodeSquareBracket) & (input value = '[')
            ifTrue: [
                turn := true.
                stack := OrderedCollection new
            ]
            ifFalse: [
                self isRightBracket: input
            ]
    ]
    isRightBracket: input [
        ((input class) = NodeSquareBracket) & (input value = ']')
            ifTrue: [
                turn := false.
                result add: (ClosureBody set: stack).
                stack := OrderedCollection new
            ]
            ifFalse: [
                self isTurnOn: input
            ]
    ]
    isTurnOn: input [
        turn
            ifTrue: [
                stack add: input
            ]
            ifFalse: [
                result add: input
            ]
    ]
]


Object subclass: #Parser.
Parser instanceVariableNames: 'step1 step2 step3 step4 replaceVariable step6
processClosure replaceClosure removeClosureSplit storeClosure virtualMachine'.
Parser class extend [
    new [
        ^(super new init)
    ]
].
Parser extend [
    init [
        step1 := Tokenize new.
        step2 := ProcessParentheses new.
        step3 := ProcessAssignmentPriority new.
        step4 := ProcessAtom new.
        processClosure := ProcessClosure new.
        replaceVariable := ReplaceVariable new init.
        replaceClosure := ReplaceClosure new init.
        step6 := InfixToPostfix new.
        removeClosureSplit := RemoveClosureSplit new.
        storeClosure := StoreClosure new.
        virtualMachine := LittleStackMachine new init.
        ^self
    ]
    run: input [
        |result|
        input printNl.
        result := step1 reset; run: input.
        result := step2 run: result.
        result := step3 run: result.
        result := step4 run: result.
        result := processClosure run: result.
        result := replaceVariable run: result.
        result := replaceClosure run: result.
        result := step6 run: result.
        result := removeClosureSplit run: result.
        result := storeClosure run: result.
        result := virtualMachine run: result.
        result printNl.
        ^result
    ]
]
