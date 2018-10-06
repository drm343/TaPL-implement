Object subclass: #BaseNode.
BaseNode instanceVariableNames: 'info value type priority'.
BaseNode class extend [
    set: input [
        |yourself|
        yourself := super new init.
        comment := 'basenode'.
        ^(yourself set: input)
    ]
    upgradeFrom: node [
        |yourself|
        yourself := super new init.
        ^(yourself setByNode: node)
    ]
]
BaseNode extend [
    init [
        info := nil.
        type := nil.
        priority := 0.
        ^self
    ]
    set: input [
        value := input.
        ^self
    ]
    setInfo: input [
        info := input.
        ^self
    ]
    info [
        ^info
    ]
    setByNode: node [
        value := node value.
        priority := (node priority).
        ^self
    ]
    print [
        ('(%1 %2 %3) ' % {type . value . priority}) print
    ]
    printNl [
        ('(%1 %2 %3) ' % {type . value . priority}) printNl
    ]
    asString [
        ^('(%1 %2 %3) ' % {type . value . priority})
    ]
    priority [^priority]
    value [^value]
    isSmallerPriority: object [
        ^(priority < (object priority))
    ]
    isEqualPriority: object [
        ^(priority = (object priority))
    ]
    isBiggerPriority: object [
        ^(priority > (object priority))
    ]
    addPriority: value [
        priority := priority + value.
        ^self
    ]
]


BaseNode subclass: #NodeNumber.
NodeNumber extend [
    init [
        super init.
        type := 'number'.
        ^self
    ]
]


BaseNode subclass: #NodeOp.
NodeOp extend [
    init [
        super init.
        type := 'op'.
        priority := 2.
        ^self
    ]
]


BaseNode subclass: #NodeAtom.
NodeAtom extend [
    init [
        super init.
        type := 'atom'.
        ^self
    ]
]


NodeAtom subclass: #AssignVariable.
AssignVariable extend [
    init [
        super init.
        priority := 1.
        type := 'assign-variable'.
        ^self
    ]
]


BaseNode subclass: #FunctionBody.
FunctionBody extend [
    init [
        priority := 1.
        type := 'funtion-body'.
        ^self
    ]
]


BaseNode subclass: #AnonymousExecuteFunction.
AnonymousExecuteFunction instanceVariableNames: 'body processed'.
AnonymousExecuteFunction class extend [
    upgradeFrom: aNode [
        |yourself|
        yourself := super upgradeFrom: aNode.
        ^(yourself set: 0; setBody: (aNode value))
    ]
]
AnonymousExecuteFunction extend [
    init [
        processed := false.
        priority := 1.
        value := 0.
        type := 'anonymous-execute-funtion'.
        ^self
    ]
    setByNode: node [
        priority := (node priority).
        ^self
    ]
    setBody: aNode [
        body := aNode.
        ^self
    ]
    body [
        ^body
    ]
    setProcessed [
        processed := true.
        ^self
    ]
    asString [
        |result|
        result := ''.
        processed
            ifTrue: [result := body]
            ifFalse: [
                body collect: [ :current |
                    result := ('%1 %2' % {result . current asString})
                ].
            ].
        ^('(%1 (%2) %3 (%4)) ' % {type . value . priority . result})
    ]
]


BaseNode subclass: #ExecuteFunction.
ExecuteFunction extend [
    init [
        priority := 1.
        type := 'execute-funtion'.
        ^self
    ]
]


BaseNode subclass: #NodeFunction.
NodeFunction instanceVariableNames: 'body'.
NodeFunction class extend [
    upgradeFrom: aNode and: anOtherNode [
        |yourself|
        yourself := super upgradeFrom: aNode.
        ^(yourself setBody: anOtherNode)
    ]
]
NodeFunction extend [
    init [
        super init.
        type := 'funtion'.
        ^self
    ]
    setBody: aNode [
        body := aNode.
        ^self
    ]
    body [
        ^body
    ]
    show [
        body printNl.
        ^self
    ]
]


NodeAtom subclass: #NodeVariable.
NodeVariable extend [
    init [
        super init.
        priority := 0.
        type := 'variable'.
        ^self
    ]
]


NodeVariable subclass: #NodeProperty.
NodeProperty extend [
    init [
        priority := 0.
        type := 'property'.
        ^self
    ]
]


BaseNode subclass: #NodeAssignment.
NodeAssignment extend [
    init [
        super init.
        type := 'assignment'.
        priority := 10.
        ^self
    ]
]


BaseNode subclass: #NodeParentheses.
NodeParentheses extend [
    init [
        super init.
        priority := 10.
        type := 'parentheses'.
        ^self
    ]
]


BaseNode subclass: #NodeSquareBracket.
NodeSquareBracket extend [
    init [
        super init.
        priority := 0.
        type := 'square-bracket'.
        ^self
    ]
]


BaseNode subclass: #NodeParameter.
NodeParameter extend [
    init [
        super init.
        priority := 0.
        type := 'parameter'.
        ^self
    ]
]


BaseNode subclass: #NodeMessage.
NodeMessage extend [
    init [
        super init.
        priority := 0.
        type := 'message'.
        ^self
    ]
]


NodeMessage subclass: #NodeMergedMessage.
NodeMergedMessage instanceVariableNames: 'body'.
NodeMergedMessage class extend [
    upgradeFrom: aNode and: anOtherNode [
        |yourself|
        yourself := super upgradeFrom: aNode.
        ^(yourself setBody: anOtherNode)
    ]
]
NodeMergedMessage extend [
    init [
        type := 'merged-message'.
        ^self
    ]
    setBody: aBody [
        body := aBody.
        ^self
    ]
    body [
        ^body
    ]
    asString [
        ^('(%1 %2 %3 %4) ' % {type . value . priority . body asString})
    ]
]


BaseNode subclass: #NodeFunctionSplit.
NodeFunctionSplit extend [
    init [
        super init.
        priority := 0.
        type := 'funtion-split'.
        ^self
    ]
]


BaseNode subclass: #NodeExpression.
NodeExpression extend [
    init [
        super init.
        priority := 0.
        type := 'expression-end'.
        ^self
    ]
]
