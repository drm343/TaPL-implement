Object subclass: #BaseNode.
BaseNode instanceVariableNames: 'value type priority'.
BaseNode class extend [
    set: input [
        |yourself|
        yourself := super new init.
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
        priority := priority + (node priority).
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
NodeNumber class extend [
    print [
        'is number' printNl
    ]
]
NodeNumber extend [
    init [
        super init.
        type := 'number'.
        ^self
    ]
]


BaseNode subclass: #NodeOp.
NodeOp class extend [
    print [
        'is op' printNl
    ]
]
NodeOp extend [
    init [
        super init.
        type := 'op'.
        priority := 2.
        ^self
    ]
]


BaseNode subclass: #NodeAtom.
NodeAtom class extend [
    print [
        'is atom' printNl
    ]
]
NodeAtom extend [
    init [
        super init.
        type := 'atom'.
        ^self
    ]
]


NodeAtom subclass: #AssingVariable.
AssingVariable class extend [
    print [
        'assign a variable' printNl
    ]
]
AssingVariable extend [
    init [
        super init.
        priority := 1.
        type := 'assign-variable'.
        ^self
    ]
]


NodeAtom subclass: #NodeVariable.
NodeVariable class extend [
    print [
        'is a variable' printNl
    ]
]
NodeVariable extend [
    init [
        super init.
        priority := 0.
        type := 'variable'.
        ^self
    ]
]


NodeAtom subclass: #Variable.
Variable class extend [
    print [
        'is variable' printNl
    ]
]
Variable extend [
    init [
        super init.
        priority := 1.
        type := 'variable'.
        ^self
    ]
]


BaseNode subclass: #NodeAssignment.
NodeAssignment class extend [
    print [
        'is assignment' printNl
    ]
]
NodeAssignment extend [
    init [
        super init.
        type := 'assignment'.
        priority := 10.
        ^self
    ]
]


BaseNode subclass: #NodeBracket.
NodeBracket class extend [
    print [
        'is bracket' printNl
    ]
]
NodeBracket extend [
    init [
        super init.
        priority := 10.
        type := 'bracket'.
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
