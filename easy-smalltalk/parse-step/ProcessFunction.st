FileStream fileIn: 'ModifyOrderedCollection.st'.


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
