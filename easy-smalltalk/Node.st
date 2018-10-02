Object subclass: #BaseNode.
BaseNode instanceVariableNames: 'value type priority'.
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
        type := nil.
        priority := 0.
        ^self
    ]
    set: input [
        value := input.
        ^self
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


BaseNode subclass: #ClosureBody.
ClosureBody extend [
    init [
        priority := 1.
        type := 'closure-body'.
        ^self
    ]
]


BaseNode subclass: #AnonymousExecuteClosure.
AnonymousExecuteClosure extend [
    init [
        priority := 1.
        type := 'anonymous-execute-closure'.
        ^self
    ]
]


BaseNode subclass: #ExecuteClosure.
ExecuteClosure extend [
    init [
        priority := 1.
        type := 'execute-closure'.
        ^self
    ]
]


BaseNode subclass: #NodeClosure.
NodeClosure instanceVariableNames: 'body'.
NodeClosure class extend [
    upgradeFrom: aNode and: anOtherNode [
        |yourself|
        yourself := super upgradeFrom: aNode.
        ^(yourself setBody: anOtherNode)
    ]
]
NodeClosure extend [
    init [
        super init.
        type := 'closure'.
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
        yourself setBody: anOtherNode.
        ^yourself
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


BaseNode subclass: #NodeClosureSplit.
NodeClosureSplit extend [
    init [
        super init.
        priority := 0.
        type := 'closure-split'.
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
