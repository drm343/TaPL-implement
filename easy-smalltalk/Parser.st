FileStream fileIn: 'Node.st'.
FileStream fileIn: 'ModifyOrderedCollection.st'.
FileStream fileIn: 'VariableTable.st'.


Object subclass: #Tokenize.
Tokenize instanceVariableNames: 'number op bracket atom assignment expressionEnd result'.
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
        bracket := '^\s?([\(\)])(.*)'.
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
            ifNotMatched: [self parseBracket: input]
    ]
    parseBracket: input [
        (input =~ bracket)
            ifMatched: [:match |
                result add: (NodeBracket set: (match at: 1)).
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


Object subclass: #ProcessBracket.
ProcessBracket instanceVariableNames: 'modifyPriority'.
ProcessBracket extend [
    init [
        modifyPriority := 0.
        ^self
    ]
    run: input [
        modifyPriority := 0.
        input collect: [:next | self isOp: next].
        ^(input reject: [:next | (next class) = NodeBracket])
    ]
    isOp: node [
        ((node class) = NodeOp)
            ifTrue: [
                ^(node addPriority: modifyPriority)
            ]
            ifFalse: [^(self isBracket: node)]
    ]
    isBracket: node [
        ((node class) = NodeBracket)
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
        result := input reduceWith: [:current :next |
            ((next class) = NodeAssignment)
                ifTrue: [AssingVariable upgradeFrom: current]
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
        ((input class) = AssingVariable)
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


Object subclass: #LittleStackMachine.
LittleStackMachine instanceVariableNames: 'stack data'.
LittleStackMachine extend [
    init [
        data := OrderedCollection new.
        ^self
    ]
    run: input [
        stack := OrderedCollection new.
        input collect: [:next | self processNumber: next].
        ^stack
    ]
    processNumber: value [
        ((value class) = NodeNumber)
            ifTrue: [ stack add: value ]
            ifFalse: [ self processOp: value ]
    ]
    processOp: op [
        ((op class) = NodeOp)
            ifTrue: [
                self processAdd: op;
                processSub: op;
                processMul: op;
                processDiv: op.
            ]
            ifFalse: [ self processAssingVariable: op ]
    ]
    processAssingVariable: variable [
        ((variable class) = AssingVariable)
            ifTrue: [
                (variable value) > (data size)
                    ifTrue: [
                        data := OrderedCollection copyFrom: data withSize: (variable value).
                    ].
                data at: (variable value) put: (stack last copy).
            ]
            ifFalse: [ self processVariable: variable ]
    ]
    processVariable: variable [
        ((variable class) = NodeVariable)
            ifTrue: [
                stack add: (data at: (variable value) copy).
            ]
            ifFalse: [ self processExpression: variable ]
    ]
    processExpression: value [
        ((value class) = NodeExpression)
            ifTrue: [ (stack removeLast) printNl ]
    ]
    processAdd: op [
        |r1 r2|
        ((op value) = '+')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 + r2))
            ].
        ^self
    ]
    processSub: op [
        |r1 r2|
        ((op value) = '-')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 - r2))
            ].
        ^self
    ]
    processMul: op [
        |r1 r2|
        ((op value) = '*')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 * r2))
            ].
        ^self
    ]
    processDiv: op [
        |r1 r2|
        ((op value) = '/')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 / r2))
            ].
        ^self
    ]
]


Object subclass: #Parser.
Parser instanceVariableNames: 'step1 step2 step3 step4 replaceStep step6 virtualMachine'.
Parser class extend [
    new [
        ^(super new init)
    ]
].
Parser extend [
    init [
        step1 := Tokenize new.
        step2 := ProcessBracket new.
        step3 := ProcessAssignmentPriority new.
        step4 := ProcessAtom new.
        replaceStep := ReplaceVariable new init.
        step6 := InfixToPostfix new.
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
        result := replaceStep run: result.
        result := step6 run: result.
        result := virtualMachine run: result.
        result printNl.
        ^result
    ]
]
