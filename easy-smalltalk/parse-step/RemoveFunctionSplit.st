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
