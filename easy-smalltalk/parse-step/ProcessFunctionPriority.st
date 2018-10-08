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
