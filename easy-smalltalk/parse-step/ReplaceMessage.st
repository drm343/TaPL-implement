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
