FileStream fileIn: 'Node.st'.
FileStream fileIn: 'VariableTable.st'.
FileStream fileIn: 'ClassInfo.st'.
FileStream fileIn: 'MessageTable.st'.
FileStream fileIn: 'LittleStackMachine.st'.

FileStream fileIn: 'parse-step/Load.st'.


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
