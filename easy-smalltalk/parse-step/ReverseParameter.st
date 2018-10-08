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
