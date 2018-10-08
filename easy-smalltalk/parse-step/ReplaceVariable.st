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
