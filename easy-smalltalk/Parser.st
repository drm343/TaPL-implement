FileStream fileIn: 'Node.st'.
FileStream fileIn: 'VariableTable.st'.
FileStream fileIn: 'LittleStackMachine.st'.


Object subclass: #Tokenize.
Tokenize instanceVariableNames: 'number op message parentheses squareBracket parameter closureSplit atom assignment expressionEnd result'.
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
            ifNotMatched: [self parseMessage: input]
    ]
    parseMessage: input [
        (input =~ message)
            ifMatched: [:match |
                result add: (NodeMessage set: (match at: 1)).
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


Object subclass: #MergeMessage.
MergeMessage instanceVariableNames: 'result stack turn previous'.
MergeMessage extend [
    run: input [
        previous := nil.
        result := input collectWithNext: [:current :next |
            (current class = NodeMessage)
                ifTrue: [previous := (NodeMergedMessage upgradeFrom: current and: next). previous]
                ifFalse: [
                    (previous class = NodeMergedMessage)
                        ifTrue: [previous := current. nil]
                        ifFalse: [previous := current. current]
                ]
        ].
        ^(result reject: [:current | current = nil])
    ]
]


Object subclass: #ProcessClosure.
ProcessClosure instanceVariableNames: 'result stack turn'.
ProcessClosure extend [
    run: input [
        turn := false.
        result := OrderedCollection new.
        input collect: [:current | self isLeftBracket: current].
        result := result collectWithNext2Item: [:current :next :next2 |
            ((next2 class = NodeExpression) & (next class = ClosureBody) & (current class = AssignVariable))
                ifTrue: [NodeClosure upgradeFrom: current and: (next value)]
                ifFalse: [
                    ((next class ~= NodeExpression) & (current class = ClosureBody))
                        ifTrue: [AnonymousExecuteClosure upgradeFrom: current]
                        ifFalse: [current].
                ]
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
            ifFalse: [^(self isAssignClosure: input)]
    ]
    isAssignClosure: input [
        |value|
        (input class = NodeClosure)
            ifTrue: [
                value := table addFuntion: (input value).
                input set: value.
                value := self run: (input body).
                ^(input setBody: value)
            ]
            ifFalse: [^(self isAnonymousExecuteClosure: input)]
    ]
    isAnonymousExecuteClosure: input [
        |value|
        ((input class) = AnonymousExecuteClosure)
            ifTrue: [
                value := (self run: (input value)).
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
        |node|
        ((input class) = NodeVariable)
            ifTrue: [
                node := table find: (input value).
                input set: (node value).
                (node isVariable)
                    ifTrue: [^input]
                    ifFalse: [^(ExecuteClosure upgradeFrom: input)]
            ]
            ifFalse: [^input]
    ]
]


"Object subclass: #ReplaceClosure.
ReplaceClosure instanceVariableNames: 'table'.
ReplaceClosure extend [
    init [
        table := FunctionTable init.
        ^self
    ]
    run: input [
        ^(input collect: [:current | self isClosure: current])
    ]
    isClosure: input [
        |value|
        ((input class) = NodeClosure)
            ifTrue: [
                value := table add: (input value).
                ^(input set: value)
            ]
            ifFalse: [^input]
    ]
]"


Object subclass: #ProcessClosurePriority.
ProcessClosurePriority instanceVariableNames: 'modifyPriority'.
ProcessClosurePriority extend [
    run: input [
        |result|
        modifyPriority := 0.
        result := input collect: [:next | self isExpression: next].
        ^result
    ]
    isExpression: node [
        (node class = NodeExpression)
            ifTrue: [
                modifyPriority := 0.
                ^node
            ]
            ifFalse: [^(self isOp: node)]
    ]
    isOp: node [
        (node class = NodeOp)
            ifTrue: [
                ^(node addPriority: modifyPriority)
            ]
            ifFalse: [^(self isMessage: node)]
    ]
    isMessage: node [
        (node class = NodeMergedMessage)
            ifTrue: [
                ^(node addPriority: modifyPriority)
            ]
            ifFalse: [^(self isExecuteClosure: node)]
    ]
    isExecuteClosure: node [
        (node class = ExecuteClosure)
            ifTrue: [
                modifyPriority := modifyPriority + 10.
                ^node
            ]
            ifFalse: [^(self isAnonymousExecuteClosure: node)]
    ]
    isAnonymousExecuteClosure: node [
        (node class = AnonymousExecuteClosure)
            ifTrue: [
                modifyPriority := modifyPriority + 10.
            ].
        ^node
    ]
]


Object subclass: #InfixToPostfix.
InfixToPostfix instanceVariableNames: 'stack result'.
InfixToPostfix extend [
    reset [
        result := OrderedCollection new.
        stack := OrderedCollection new.
        ^self
    ]
    run: input [
        input collect: [ :next |
            self isStoredClosure: next
        ].
        result addAll: stack.
        ^result
    ]
    isStoredClosure: closure [
        |yourself body value|
        (closure class = NodeClosure)
            ifTrue: [
                yourself := InfixToPostfix new reset.
                body := closure body.
                value := yourself run: body.
                stack add: (closure setBody: value)
            ]
            ifFalse: [
                self isExecuteClosure: closure
            ]
    ]
    isExecuteClosure: closure [
        |yourself value|
        (closure class = ExecuteClosure)
            ifTrue: [
                self isStackNull: closure
            ]
            ifFalse: [
                self isAnonymousExecuteClosure: closure
            ]
    ]
    isAnonymousExecuteClosure: closure [
        |yourself value|
        ((closure class) = AnonymousExecuteClosure)
            ifTrue: [
                yourself := InfixToPostfix new reset.
                value := (yourself run: (closure value)).
                self isStackNull: (closure set: value)
            ]
            ifFalse: [
                self isNumber: closure
            ]
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
                self isParamter: value
            ]
    ]
    isParamter: value [
        (value class) = NodeParameter
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isClosureSplit: value
            ]
    ]
    isClosureSplit: value [
        (value class = NodeClosureSplit)
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isMessage: value
            ]
    ]
    isMessage: value [
        (value class = NodeMergedMessage)
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


Object subclass: #ReverseParameter.
ReverseParameter instanceVariableNames: 'result'.
ReverseParameter extend [
    run: input [
        result := OrderedCollection new.
        input collect: [:current | self isAssignClosure: current].
        ^result
    ]
    reverse: input [
        |code|
        code := OrderedCollection new.
        input collect: [:current |
            (current class = NodeParameter)
                ifTrue: [code addFirst: current]
                ifFalse: [code add: current]
        ].
        ^code
    ]
    isAssignClosure: input [
        |value|
        (input class = NodeClosure)
            ifTrue: [
                value := (self reverse: (input body)).
                result add: (input setBody: value).
            ]
            ifFalse: [
                self isAnonymousExecuteClosure: input
            ]
    ]
    isAnonymousExecuteClosure: input [
        |value|
        (input class = AnonymousExecuteClosure)
            ifTrue: [
                value := (self reverse: (input value)).
                result add: (input set: value).
            ]
            ifFalse: [
                self isOthers: input
            ]
    ]
    isOthers: input [
        result add: input
    ]
]


Object subclass: #RemoveClosureSplit.
RemoveClosureSplit instanceVariableNames: 'result'.
RemoveClosureSplit extend [
    run: input [
        result := OrderedCollection new.
        input collect: [:current | self isAssignClosure: current].
        ^result
    ]
    remove: input [
        ^(input reject: [:current | (current class) = NodeClosureSplit])
    ]
    isAssignClosure: input [
        |value|
        (input class = NodeClosure)
            ifTrue: [
                value := (self remove: (input body)).
                result add: (input setBody: value).
            ]
            ifFalse: [
                self isAnonymousExecuteClosure: input
            ]
    ]
    isAnonymousExecuteClosure: input [
        |value|
        (input class = AnonymousExecuteClosure)
            ifTrue: [
                value := (self remove: (input value)).
                result add: (input set: value).
            ]
            ifFalse: [
                self isOthers: input
            ]
    ]
    isOthers: input [
        result add: input
    ]
]


Object subclass: #Parser.
Parser instanceVariableNames: 'step1 step2 step3 step4 replaceVariable infixToPostfixStep
mergeMessage processClosurePriority reverseParameter
processClosure removeClosureSplit virtualMachine'.
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
        mergeMessage := MergeMessage new.
        processClosure := ProcessClosure new.
        replaceVariable := ReplaceVariable new init.
        processClosurePriority := ProcessClosurePriority new.
        infixToPostfixStep := InfixToPostfix new.
        reverseParameter := ReverseParameter new.
        removeClosureSplit := RemoveClosureSplit new.
        virtualMachine := LittleStackMachine new init.
        ^self
    ]
    run: input [
        |result|
        result := step1 reset; run: input.
        result := step2 run: result.
        result := step3 run: result.
        result := step4 run: result.
        result := mergeMessage run: result.
        result := processClosure run: result.
        result := replaceVariable run: result.
        result := processClosurePriority run: result.
        result := infixToPostfixStep reset; run: result.
        result := reverseParameter run: result.
        result := removeClosureSplit run: result.
        result := virtualMachine run: result.
        ^result
    ]
    runAndPrint: input [
        |result|
        input printNl.
        result := self run: input.
        result printNl.
        ^result
    ]
]
