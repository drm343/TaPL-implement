FileStream fileIn: 'Node.st'.


Object subclass: #TokenizeStep.
TokenizeStep instanceVariableNames: 'number op bracket result'.
TokenizeStep class extend [
    new [
        ^(super new init)
    ]
]
TokenizeStep extend [
    init [
        number := '^\s?(\d*)(.*)'.
        op := '^\s?([\+\-\*/])(.*)'.
        bracket := '^\s?([\(\)])(.*)'.
        result := OrderedCollection new.
        ^self
    ]
    reset [
        result := OrderedCollection new.
        ^self
    ]
    run: input [
        input printNl.
        self parseNumber: input.
        ^(result)
    ]
    parseNumber: input [
        (input =~ number)
            ifMatched: [:match |
	        self fixNumberBug: (match at: 1) with: (match at: 2)
            ]
            ifNotMatched: [self parseOp: input]
    ]
    fixNumberBug: value with: input [
        (value = '')
	    ifTrue: [self parseOp: input]
	    ifFalse: [
                result add: (NodeNumber set: (value asNumber)).
                self parseNumber: input
	    ]
    ]
    parseOp: input [
        (input =~ op)
            ifMatched: [:match |
                result add: (NodeOp set: (match at: 1)).
                self parseNumber: (match at: 2)
            ]
            ifNotMatched: [self parseBracket: input]
    ]
    parseBracket: input [
        (input =~ bracket)
            ifMatched: [:match |
                result add: (NodeBracket set: (match at: 1)).
                self parseNumber: (match at: 2)
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


Object subclass: #InfixToPostfixStep.
InfixToPostfixStep instanceVariableNames: 'stack result'.
InfixToPostfixStep extend [
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
                self isStoreNull: value
            ]
    ]
    isStoreNull: value [
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
LittleStackMachine instanceVariableNames: 'stack'.
LittleStackMachine extend [
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
    processOp: op[
        ((op class) = NodeOp)
            ifTrue: [
                self processAdd: op;
                processSub: op;
                processMul: op;
                processDiv: op.
            ]
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
Parser instanceVariableNames: 'step1 step2 step3 step4'.
Parser class extend [
    new [
        ^(super new init)
    ]
].
Parser extend [
    init [
        step1 := TokenizeStep new.
        step2 := ProcessBracket new.
        step3 := InfixToPostfixStep new.
        step4 := LittleStackMachine new.
        ^self
    ]
    run: input [
        |result|
        result := step1 reset; run: input.
        result := step2 run: result.
        result := step3 run: result.
        result := step4 run: result.
        ^(result at: 1)
    ]
]
