FileStream fileIn: 'Node.st'.
FileStream fileIn: 'VariableTable.st'.
FileStream fileIn: 'ClassInfo.st'.
FileStream fileIn: 'MessageTable.st'.
FileStream fileIn: 'LittleStackMachine.st'.


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


Object subclass: #ProcessParentheses.
ProcessParentheses instanceVariableNames: 'code modifyPriority'.
ProcessParentheses extend [
    init [
        modifyPriority := 0.
        ^self
    ]
    run: input [
        modifyPriority := 0.
        input collect: [:next | self isParentheses: next].
        ^(input reject: [:next | (next class) = NodeParentheses])
    ]
    isParentheses: node [
        (node class = NodeParentheses)
            ifTrue: [
                self isLeft: node.
                self isRight: node.
                ^self
            ]
            ifFalse: [^(self isOthers: node)]
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
    isOthers: node [
        ^(node addPriority: modifyPriority)
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
MergeMessage instanceVariableNames: 'tmp stack result previous'.
MergeMessage extend [
    run: input [
        result := OrderedCollection new.
        previous := nil.
        stack := input collect: [:current |
            (current class = AnonymousExecuteFunction)
                ifTrue: [
                    tmp := MergeMessage new.
                    tmp := tmp run: (current body).
                    current setBody: tmp.
                    current
                ]
                ifFalse: [current]
        ].
        stack := input collectWithNext: [:current :next |
            (current class = NodeAtomMessage)
                ifTrue: [previous := (NodeMergedMessage upgradeFrom: current and: next). previous copy]
                ifFalse: [
                    (previous class = NodeMergedMessage)
                        ifTrue: [previous := current. nil]
                        ifFalse: [previous := current. current copy]
                ]
        ].
        stack := (stack reject: [:current | current = nil]) copy.
        previous := nil.
        stack collect: [:current |
            (current class = NodeMergedMessage)
                ifTrue: [
                    (previous = nil)
                        ifTrue: [
                            previous := NodeMessage upgradeFrom: current.
                        ]
                        ifFalse: [
                            previous upgradeWith: current.
                        ].
                ]
                ifFalse: [
                    (previous = nil)
                        ifTrue: [
                            result add: current.
                        ]
                        ifFalse: [
                            result add: previous.
                            result add: current.
                            previous := nil.
                        ].
                ].
            current
        ].
        (previous = nil)
            ifFalse: [
                result add: previous.
                previous := nil.
            ].
        ^result
    ]
]


Object subclass: #ProcessFunction.
ProcessFunction instanceVariableNames: 'result stack turn'.
ProcessFunction extend [
    run: input [
        turn := false.
        result := OrderedCollection new.
        input collect: [:current | self isLeftBracket: current].
        result := result collectWithNext2Item: [:current :next :next2 |
            ((next2 class = NodeExpression) & (next class = FunctionBody) & (current class = AssignVariable))
                ifTrue: [NodeFunction upgradeFrom: current and: (next value)]
                ifFalse: [
                    ((next class ~= NodeExpression) & (current class = FunctionBody))
                        ifTrue: [AnonymousExecuteFunction upgradeFrom: current]
                        ifFalse: [current].
                ]
        ].
        ^(result reject: [:current | (current class) = FunctionBody])
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
                result add: (FunctionBody set: stack).
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


Object subclass: #ReplaceClassInfo.
ReplaceClassInfo instanceVariableNames: 'messageTable table aClassInfo'.
ReplaceClassInfo class extend [
    createWithMessageTable: aMessageTable [
        ^(super new initWithTable: aMessageTable)
    ]
]
ReplaceClassInfo extend [
    initWithTable: aTable [
        messageTable := aTable.
        table := OrderedCollection new.
        ^self
    ]
    run: input [
        ^(input collect: [:current | self isAssignFunction: current])
    ]
    isAssignFunction: node [
        |key value code|
        (node class = NodeFunction)
            ifTrue: [
                value := node value.
                aClassInfo := ClassInfo init: value.
                table add: aClassInfo.
                code := self run: (node body).
                ^(node setInfo: aClassInfo; setBody: code)
            ]
            ifFalse: [^(self isAnonymousExecuteFunction: node)]
    ]
    isAnonymousExecuteFunction: node [
        |key value code|
        (node class = AnonymousExecuteFunction)
            ifTrue: [
                value := node value.
                aClassInfo := ClassInfo init: 'Anonymous-Function'.
                table add: aClassInfo.
                code := self run: (node body).
                ^(node setInfo: aClassInfo; setBody: code)
            ]
            ifFalse: [^(self isParameter: node)]
    ]
    isParameter: node [
        |value|
        (node class = NodeParameter)
            ifTrue: [
                value := aClassInfo addVariable: (node value).
                ^(node set: value)
            ]
            ifFalse: [^(self isNodeVariable: node)]
    ]
    isNodeVariable: node [
        |result|
        (node class = NodeVariable)
            ifTrue: [
                result := aClassInfo findVariable: (node value).
                (result = nil)
                    ifTrue: [^node].
                node set: (result value).
                (result isVariable)
                    ifTrue: [^(NodeProperty upgradeFrom: node)]
                    ifFalse: [^(ExecuteFunction upgradeFrom: node)]
            ]
            ifFalse: [^(self isMessage: node)]
    ]
    isMessage: node [
        |result|
        (node class = NodeMessage)
            ifTrue: [
                result := self run: (node body).
                ^(node setBody: result)
            ]
            ifFalse: [^node]
    ]
]


Object subclass: #ReplaceMessage.
ReplaceMessage instanceVariableNames: 'messageTable'.
ReplaceMessage class extend [
    createWithMessageTable: aMessageTable [
        ^(super new initWithTable: aMessageTable)
    ]
]
ReplaceMessage extend [
    initWithTable: aTable [
        messageTable := aTable.
        ^self
    ]
    run: input [
        ^(input collect: [:current | self isAnonymousExecuteFunction: current])
    ]
    isAnonymousExecuteFunction: node [
        |code|
        (node class = AnonymousExecuteFunction)
            ifTrue: [
                code := node body.
                code := self run: code.
                ^(node setBody: code)
            ]
            ifFalse: [^(self isMessage: node)]
    ]
    isMessage: node [
        |key value code|
        (node class = NodeMessage)
            ifTrue: [
                key := node value.
                value := messageTable findOrCreateBy: key.
                ^(node set: value)
            ]
            ifFalse: [^node]
    ]
]


Object subclass: #ReplaceVariable.
ReplaceVariable instanceVariableNames: 'table'.
ReplaceVariable class extend [
    new [
        ^(super new init)
    ]
]
ReplaceVariable extend [
    init [
        table := VariableTable create.
        ^self
    ]
    run: input [
        ^(input collect: [:current | self isAssignVariable: current])
    ]
    isAssignVariable: node [
        |value|
        ((node class) = AssignVariable)
            ifTrue: [
                value := table add: (node value).
                ^(node set: value)
            ]
            ifFalse: [^(self isMessage: node)]
    ]
    isMessage: node [
        |message|
        (node class = NodeMessage)
            ifTrue: [
                message:= self run: (node body).
                ^(node setBody: message)
            ]
            ifFalse: [^(self isAssignFunction: node)]
    ]
    isAssignFunction: node [
        |key value|
        (node class = NodeFunction)
            ifTrue: [
                key := table addFuntion: (node value).
                node set: key.
                value := self run: (node body).
                ^(node setBody: value)
            ]
            ifFalse: [^(self isAnonymousExecuteFunction: node)]
    ]
    isAnonymousExecuteFunction: node [
        |code|
        (node class = AnonymousExecuteFunction)
            ifTrue: [
                code := self run: (node body).
                ^(node setBody: code)
            ]
            ifFalse: [^(self isNodeVariable: node)]
    ]
    isNodeVariable: input [
        |node|
        ((input class) = NodeVariable)
            ifTrue: [
                node := table find: (input value).
                input set: (node value).
                (node isVariable)
                    ifTrue: [^input]
                    ifFalse: [^(ExecuteFunction upgradeFrom: input)]
            ]
            ifFalse: [^input]
    ]
]


Object subclass: #ProcessFunctionPriority.
ProcessFunctionPriority instanceVariableNames: 'modifyPriority'.
ProcessFunctionPriority extend [
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
        (node class = NodeMessage)
            ifTrue: [
                ^(node addPriority: modifyPriority)
            ]
            ifFalse: [^(self isExecuteFunction: node)]
    ]
    isExecuteFunction: node [
        (node class = ExecuteFunction)
            ifTrue: [
                modifyPriority := modifyPriority + 10.
                ^node
            ]
            ifFalse: [^(self isAnonymousExecuteFunction: node)]
    ]
    isAnonymousExecuteFunction: node [
        (node class = AnonymousExecuteFunction)
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
            self isStoredFunction: next
        ].
        result addAll: stack.
        ^result
    ]
    isStoredFunction: funtion [
        |yourself body value|
        (funtion class = NodeFunction)
            ifTrue: [
                yourself := InfixToPostfix new reset.
                body := funtion body.
                value := yourself run: body.
                stack add: (funtion setBody: value)
            ]
            ifFalse: [
                self isExecuteFunction: funtion
            ]
    ]
    isExecuteFunction: funtion [
        |yourself value|
        (funtion class = ExecuteFunction)
            ifTrue: [
                self isStackNull: funtion
            ]
            ifFalse: [
                self isAnonymousExecuteFunction: funtion
            ]
    ]
    isAnonymousExecuteFunction: funtion [
        |yourself code|
        (funtion class = AnonymousExecuteFunction)
            ifTrue: [
                yourself := InfixToPostfix new reset.
                code := yourself run: (funtion body).
                self isStackNull: (funtion setBody: code)
            ]
            ifFalse: [
                self isNumber: funtion
            ]
    ]
    isNumber: value [
        (value class = NodeNumber)
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isVariable: value
            ]
    ]
    isVariable: value [
        (value class = NodeVariable)
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isParamter: value
            ]
    ]
    isParamter: value [
        (value class = NodeParameter)
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isProperty: value
            ]
    ]
    isProperty: node [
        (node class = NodeProperty)
            ifTrue: [
                result add: node
            ]
            ifFalse: [
                self isFunctionSplit: node
            ]
    ]
    isFunctionSplit: value [
        (value class = NodeFunctionSplit)
            ifTrue: [
                result add: value
            ]
            ifFalse: [
                self isMessage: value
            ]
    ]
    isMessage: node [
        |yourself code|
        (node class = NodeMessage)
            ifTrue: [
                code := (node body) collect: [:current | self isAnonymousFunctionInMessage: current].
                node setBody: code.
                self isStackNull: node
            ]
            ifFalse: [
                self isExpression: node
            ]
    ]
    isAnonymousFunctionInMessage: node [
        |yourself code|
        (node class = AnonymousExecuteFunction)
            ifTrue: [
                yourself := InfixToPostfix new reset.
                code := yourself run: (node body).
                node setBody: code.
            ].
        ^node
    ]
    isExpression: value [
        (value class = NodeExpression)
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
        input collect: [:current | self isAssignFunction: current].
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
    isAssignFunction: input [
        |value|
        (input class = NodeFunction)
            ifTrue: [
                value := self reverse: (input body).
                result add: (input setBody: value).
            ]
            ifFalse: [
                self isAnonymousExecuteFunction: input
            ]
    ]
    isAnonymousExecuteFunction: node [
        |code|
        (node class = AnonymousExecuteFunction)
            ifTrue: [
                code := self reverse: (node body).
                result add: (node setBody: code).
            ]
            ifFalse: [
                self isOthers: node
            ]
    ]
    isOthers: input [
        result add: input
    ]
]


Object subclass: #RemoveFunctionSplit.
RemoveFunctionSplit instanceVariableNames: 'result'.
RemoveFunctionSplit extend [
    run: input [
        result := OrderedCollection new.
        input collect: [:current | self isAssignFunction: current].
        ^result
    ]
    remove: input [
        ^(input reject: [:current | (current class) = NodeFunctionSplit])
    ]
    isAssignFunction: node [
        |code|
        (node class = NodeFunction)
            ifTrue: [
                code := self remove: (node body).
                result add: (node setBody: code).
            ]
            ifFalse: [
                self isAnonymousExecuteFunction: node
            ]
    ]
    isAnonymousExecuteFunction: node [
        |code|
        (node class = AnonymousExecuteFunction)
            ifTrue: [
                code := self remove: (node body).
                result add: (node setBody: code).
            ]
            ifFalse: [
                self isMessage: node
            ]
    ]
    isMessage: node [
        |yourself code|
        (node class = NodeMessage)
            ifTrue: [
                yourself := RemoveFunctionSplit new.
                code := yourself run: (node body).
                result add: (node setBody: code).
            ]
            ifFalse: [
                self isOthers: node
            ]
    ]
    isOthers: node [
        result add: node
    ]
]


Object subclass: #Step.
Step instanceVariableNames: 'step reset'.
Step class extend [
    create: aStep [
        ^(super new init: aStep)
    ]
    createAndNeedReset: aStep [
        |yourself|
        yourself := super new init: aStep.
        ^(yourself needReset)
    ]
]
Step extend [
    init: aStep [
        step := aStep.
        reset := false.
        ^self
    ]
    needReset [
        reset := true.
        ^self
    ]
    run: input [
        reset ifTrue: [step reset].
        ^(step run: input)
    ]
]


Object subclass: #DebugStep.
DebugStep class extend [
    create [
        ^(super new)
    ]
]
DebugStep extend [
    run: input [
        input printNl.
        ^input
    ]
]


Object subclass: #Parser.
Parser instanceVariableNames: 'result messageTable stepOrder'.
Parser class extend [
    new [
        ^(super new init)
    ]
].
Parser extend [
    init [
        messageTable := MessageTable create.

        self startStep: Tokenize andNeedReset: true;
            then: ProcessParentheses;
            then: ProcessAssignmentPriority;
            then: ProcessAtom;
            then: ProcessFunction;
            then: MergeMessage;
            then: ReplaceMessage withMessageTable: messageTable;
            then: ReplaceClassInfo withMessageTable: messageTable;
            then: ReplaceVariable;
            then: ProcessFunctionPriority;
            then: InfixToPostfix andNeedReset: true;
            then: ReverseParameter;
            then: RemoveFunctionSplit;
            debug;
            then: LittleStackMachine withMessageTable: messageTable andNeedReset: true.
        ^self
    ]
    debug [
        stepOrder add: (DebugStep create).
        ^self
    ]
    startStep: aStep andNeedReset: reset [
        stepOrder := OrderedCollection new.
        stepOrder add: (Step createAndNeedReset: (aStep new)).
        ^self
    ]
    then: aStep [
        stepOrder add: (Step create: (aStep new)).
        ^self
    ]
    then: aStep withMessageTable: aMessageTable [
        stepOrder add: (Step create: (aStep createWithMessageTable: aMessageTable)).
        ^self
    ]
    then: aStep withMessageTable: aMessageTable andNeedReset: reset [
        stepOrder add: (Step createAndNeedReset: (aStep createWithMessageTable: aMessageTable)).
        ^self
    ]
    then: aStep andNeedReset: reset [
        stepOrder add: (Step createAndNeedReset: (aStep new)).
        ^self
    ]
    runStep: aStep [
        result := aStep run: result.
    ]
    run: input [
        result := input.
        stepOrder collect: [:current | self runStep: current].
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
