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
