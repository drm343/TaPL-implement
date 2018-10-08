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
