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
